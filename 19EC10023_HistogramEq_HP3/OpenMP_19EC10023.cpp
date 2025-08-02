#include<bits/stdc++.h>
#include<iostream>
#include<omp.h>
#include<opencv2/opencv.hpp>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#define ll long long
using namespace std;
using namespace cv;


Mat histo_equalize(Mat jpg,double histg[],double prob_img[]);

int main()
{
    cv::Mat jpg;
       jpg = cv::imread("sample1.png" , CV_8UC1);
 
    if(! jpg.data ) {
      cout <<"jpg could not be opened or found!"<<endl ;
      return -1;
    }
    uchar pixel = jpg.at<uchar>(0, 0);
    cout << int(pixel) << "\n";
    double histg[256];
    double prob[256];
    int i=0;
    while(i<256)
    {
        histg[i]=0;
        prob[i]=0.0;
        i++;
    }
    double wt1,wt2;

    wt1 = omp_get_wtime();
    omp_set_num_threads(1);
    
        histo_equalize(jpg,histg,prob);
        
    wt2 = omp_get_wtime();
    cout<<endl<<"Time:"<<wt2-wt1<<"\n";
    
   

    waitKey(4000);

    return 0;
}

Mat histo_equalize(Mat jpg,double hist[],double prob_img[])
{
    double histg[256];
    for(int i=0;i<256;i++)
    {
        histg[i]=0;
    }
  #pragma omp parallel for reduction(+:histg)
    for (int i = 0; i < jpg.rows; i++)
    {
        for (int j = 0; j < jpg.cols; j++)
        {
            uchar pix = jpg.at<uchar>(i, j);
            int val = pix;
            histg[val]+=1;

        }
    }

  
    double size_img=(jpg.rows)*(jpg.cols);
    prob_img[0] = 255 * (histg[0] /size_img);
    
    for (int i = 1; i < 256; i++)
    {
        prob_img[i] = prob_img[i - 1] + 255 * (histg[i] / size_img);
    }
    
    for (int i = 0; i < 256; i++)
    {
        cout << prob_img[i] << "\n";
    }
    Mat img(jpg.rows, jpg.cols, CV_8UC1, Scalar(70));

  #pragma omp parallel for
    for (int i = 0; i < jpg.rows; i++)
    {
        for (int j = 0; j < jpg.cols; j++)
        {
            uchar intensity = jpg.at<uchar>(i, j);
            img.at<uchar>(i, j) = round(prob_img[intensity]);

        }
    }
    Mat probx;
    resize(img, probx, Size(1280,720), INTER_LINEAR);
    imshow("equalisedpng",probx);
    return img;
}