% %%
% clc;
% clear all;
% close all;
% %% Get the Data
% load('imu_data_dr_natesan.mat');
% sample = world_acc;
% divergence1 = gradient(sample(:,1)) + gradient(sample(:, 2)) + gradient(sample(:,3));
% divergence2 = diff(sample(:,1)) + diff(sample(:, 2)) + diff(sample(:,3));
% 
% 
plot3(linear_acc(1:2000,1), linear_acc(1:2000,2), linear_acc(1:2000, 3))
xlabel('x')
ylabel('y')
zlabel('z')
grid on
axis square