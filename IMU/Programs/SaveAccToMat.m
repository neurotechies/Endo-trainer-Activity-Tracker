clc;
clear all
close all

%%   Data Acquisition

% Read the raw data file
rawdata = csvread('Z:\14_NETS_Camera_Backup\imu study\NETS-WS14-Study_Endoscopy\NETS-WS14-DG1\IMU\exp_PickNPlace\Recording-1.csv');
%rawdata = csvread('D:\IMU\IMU_data\imu_sample_data_experts\Dr Ari Chaco.csv');


quat = rawdata(:, 2:5);
aa = quat > 2;
quat(aa) = quat(aa) - 4;

%derive the gravity values from quaternions
%     v -> x = 2 * (q -> x*q -> z - q -> w*q -> y);
%     v -> y = 2 * (q -> w*q -> x + q -> y*q -> z);
%     v -> z = q -> w*q -> w - q -> x*q -> x - q -> y*q -> y + q -> z*q -> z;
gravity_x = 2.* ((quat(:,2) .* quat(:,4)) - (quat(:,1) .* quat(:,3)));
gravity_y = 2.* ((quat(:,1) .* quat(:,2)) + (quat(:,3) .* quat(:,4)));
gravity_z = (quat(:,1).* quat(:,1)) - (quat(:,2).* quat(:,2)) - (quat(:,3).* quat(:,3)) - (quat(:,4).* quat(:,4));



%% Get the accerleration values (in bytes)
accer_byte = rawdata(:, 6:end);
% convert byte to signed bit 
temp_x = [];
temp_y = [];
temp_z = [];
for i = 1: length(accer_byte)
    
a = fi(accer_byte(i,:),1,8,0);
% concatenate 2 bytes to unsigned bit (dono why it does so)
ax = bitconcat(a(:,1),a(:,2));
ay = bitconcat(a(:,3),a(:,4));
az = bitconcat(a(:,5),a(:,6));
% concatenate unsigned bit to signed bit (gotcha)
% Derive the raw acceleration values 
raw_acc_x = typecast(uint16(bin2dec(bin(ax))), 'int16');
raw_acc_y = typecast(uint16(bin2dec(bin(ay))), 'int16');
raw_acc_z = typecast(uint16(bin2dec(bin(az))), 'int16');
temp_x = [temp_x;raw_acc_x];
temp_y = [temp_y;raw_acc_y];
temp_z = [temp_z;raw_acc_z];

end
raw_acc = [temp_x temp_y temp_z];

% %% Load acceleration data from file.
% save('Dr_Natesan_Drill_1','raw_acc');
% 

%% Load acceleration data from file.
%load('raw_acc_drnatesan.mat');
raw_acc_x = raw_acc(:,1);
raw_acc_y = raw_acc(:,2);
raw_acc_z = raw_acc(:,3);

linear_acc_x = int16(raw_acc_x) - int16(gravity_x .* 8192);
linear_acc_y = int16(raw_acc_y) - int16(gravity_y .* 8192);
linear_acc_z = int16(raw_acc_z) - int16(gravity_z .* 8192);
linear_acc_temp = [linear_acc_x linear_acc_y linear_acc_z];

linear_acc_process = double(linear_acc_temp) ./ 16384;

%Get the quaternion values (in float)
%timestamp = diff(rawdata(:,1))/1000;
%avgtimestamp = sum(timestamp)./length(timestamp);
Sampling_Freq = 100;

time_taken = rawdata(:,1);
timeo = 1:size(time_taken,1);
% load('acc_data_drnatesan.mat');
T = (rawdata(end,1)- rawdata(1,1)) * Sampling_Freq / 10^6;
xx = linspace(1,size(time_taken,1),ceil(T));


hu_x = double(linear_acc_process(:,1))';
cs_x = spline(timeo,hu_x);
uu_x = ppval(cs_x,xx);
% plot(xx,uu,'-');


hu_y = double(linear_acc_process(:,2))';
cs_y = spline(timeo,hu_y);
uu_y = ppval(cs_y,xx);


hu_z = double(linear_acc_process(:,3))';
cs_z = spline(timeo,hu_z);
uu_z = ppval(cs_z,xx);

linear_acc = [uu_x; uu_y; uu_z];
linear_acc = linear_acc';

save('Z:\14_NETS_Camera_Backup\imu study\NETS-WS14-Study_Endoscopy\NETS-WS14-DG1\IMU\exp_PickNPlace\Recording-1', 'linear_acc');
%save('D:\IMU\IMU_data\imu_sample_data_experts\Dr Ari Chaco', 'linear_acc');