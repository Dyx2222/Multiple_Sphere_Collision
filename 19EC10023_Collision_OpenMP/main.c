#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

//Defining total number of simulation steps
#define tot_steps 720000

int main(int argc, char** argv){

    double wtime = omp_get_wtime();
    if(argc != 2){
        printf("Incorrect input!\nCorrect format: executable_file n_threads\n");
        exit(-1);
    }

    //User-provided number of threads
    int n_threads = atoi(argv[1]);

    //Taking user inputs
    int length, width, depth, N;
    float r, m, dt, *pos_x, *pos_y, *pos_z;
    scanf("%d %d %d %d %f %f %f", &length, &width, &depth, &N, &r, &m, &dt);
    pos_x = (float *)malloc(sizeof(float) * N);
    pos_y = (float *)malloc(sizeof(float) * N);
    pos_z = (float *)malloc(sizeof(float) * N);
    for(int i=0;i<N;i++){
        scanf("%f %f %f", &pos_x[i], &pos_y[i], &pos_z[i]);
    }

    //File Handling
    FILE *fptr;
    fptr = fopen("./output_1.txt","w");
    if(fptr == NULL)
    {
      printf("Error!");   
      exit(1);             
    }

    //Initializing net_force and velocity arrays for 3 dimensions
    float *net_fx, *net_fy, *net_fz, *vx, *vy, *vz, fx, fy, fz, dx, dy, dz, den;
    net_fx = (float *)calloc(sizeof(float), N);
    net_fy = (float *)calloc(sizeof(float), N);
    net_fz = (float *)calloc(sizeof(float), N);
    vx = (float *)calloc(sizeof(float), N);
    vy = (float *)calloc(sizeof(float), N);
    vz = (float *)calloc(sizeof(float), N);

    int particle;
    //Simulation begins
    for(int step=0;step<tot_steps;step++){
        for(particle=0;particle<N;particle++){   
            #pragma omp for reduction (+: net_fx[particle],net_fy[particle],net_fz[particle])
            for(int cur_particle=0;cur_particle<N;cur_particle++){
                if(cur_particle!=particle){
                    dx = pos_x[cur_particle] - pos_x[particle];
                    dy = pos_y[cur_particle] - pos_y[particle];
                    dz = pos_z[cur_particle] - pos_z[particle];
                    den = pow(dx*dx + dy*dy + dz*dz, 1.5);
                    fx = m*m*dx/den;
                    fy = m*m*dy/den;
                    fz = m*m*dz/den;
                    net_fx[particle] += fx;
                    net_fy[particle] += fy;
                    net_fz[particle] += fz;
                }
            }
            vx[particle] += net_fx[particle]*dt/2/m;
            vy[particle] += net_fy[particle]*dt/2/m;
            vz[particle] += net_fz[particle]*dt/2/m;
        }
        #pragma omp parallel for num_threads(n_threads)
        for(particle=0;particle<N;particle++){
            
            if(pos_x[particle] <= r || pos_x[particle] >= width-r){
                vx[particle] = -vx[particle];
            }
            else if(pos_y[particle] <= r || pos_y[particle] >= length-r){
                vy[particle] = -vy[particle];
            }
            else if(pos_z[particle] <= r || pos_z[particle] >= depth-r){
                vz[particle] = -vz[particle];
            }
            else{
                for(int cur_particle=particle+1;cur_particle<N;cur_particle++){            
                    dx = pos_x[cur_particle] - pos_x[particle];
                    dy = pos_y[cur_particle] - pos_y[particle];
                    dz = pos_z[cur_particle] - pos_z[particle];
                    if(abs(dx) <= 2*r || abs(dy) <= 2*r || abs(dz) <= 2*r){
                        float temp = vx[particle];
                        vx[particle] = vx[cur_particle];
                        vx[cur_particle] = temp;
                        temp = vy[particle];
                        vy[particle] = vy[cur_particle];
                        vy[cur_particle] = temp;
                        temp = vz[particle];
                        vz[particle] = vz[cur_particle];
                        vz[cur_particle] = temp;
                    }
                }
            }
            pos_x[particle] += vx[particle]*dt;
            pos_y[particle] += vy[particle]*dt;
            pos_z[particle] += vz[particle]*dt;

            vx[particle] += net_fx[particle]*dt/2/m;
            vy[particle] += net_fy[particle]*dt/2/m;
            vz[particle] += net_fz[particle]*dt/2/m;
        }
        //Writing to file
        if(step%100 == 0){
            for(int i=0;i<N;i++){
                fprintf(fptr,"%.2f\t%.2f\t%.2f\n", pos_x[i], pos_y[i], pos_z[i]);
            }
            fprintf(fptr,"\n");
            //printf("%d steps done\n",step);
        }
    }
    fclose(fptr);
    wtime = omp_get_wtime() - wtime;
    printf("%f\n",wtime);
    return 0;
}