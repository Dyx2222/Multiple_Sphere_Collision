close all;
clear all;
clc;
filepath = 'output_2.txt';  
fileId = fopen(filepath, 'r');
w=100;
l=200;
d=400;
N=100;
r=0.5;
t_step=0.01;
n_step=7200;
dat = zeros(3, N, n_step);
color = rand(N, 3);
figure("Name", "Many Body Graphics", 'units','normalized','outerposition',[0 0 1 1]);
for i = 1:n_step
    clf;
    line = fgetl(fileId);  
    coord = fscanf(fileId, '%f %f %f', [3,N]);
scatter3(coord(1, :), coord(2, :), coord(3, :), r*120, color, 'filled');
    hold on;
    grid on;
    xlabel('x');
    ylabel('y');
    zlabel('z');
    title(["Position of all the bodies at iteration = ",num2str(i)], "Color", 'r');
    axis([0, w, 0, l, 0, d]);
    hold off;
    currframe(i) = getframe;
    drawnow();
    dat(:, :, i) = coord;
    disp("Iteration "+int2str(i)+" Data Loaded");
    
end

myVideo = VideoWriter('ManyBodyGraphical');
myVideo.FrameRate = 60;

open(myVideo);
writeVideo(myVideo, currframe);
close(myVideo);

fclose(fileId);