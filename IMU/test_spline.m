close all;
clc;

time_taken = rawdata(:,1);
timeo = 1:size(time_taken,1);
load('acc_data_drnatesan.mat');
hu = double(linear_acc(:,1))';
cs = spline(timeo,hu);
xx = linspace(1,size(time_taken,1),15000);
uu = ppval(cs,xx);
plot(xx,uu,'-');


