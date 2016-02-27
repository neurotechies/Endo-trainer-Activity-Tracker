clc;
clear all
close all

%%   Data Acquisition

% Read the raw data file
rawdata_ = csvread('C:\IMU\IMU_data\sample_mpu\Dr_Natesan_07_07_2014_1830_proper.csv');

%rawdata_ = csvread('C:\IMU\new data\test_data_with_movement.csv');

%Get the quaternion values (in float)
timestamp = diff(rawdata_(:,1));
as = timestamp > 0;
rawdata = rawdata_(as, :);
timestamp = timestamp(as);
avgtimestamp = sum(timestamp)./length(timestamp);
Sampling_Freq = 1 / avgtimestamp * 10^3;

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




%% Load acceleration data from file.
load('test_data_with_movement.mat');
raw_acc_x = raw_acc(:,1);
raw_acc_y = raw_acc(:,2);
raw_acc_z = raw_acc(:,3);

linear_acc_x = int16(raw_acc_x) - int16(gravity_x .* 8192);
linear_acc_y = int16(raw_acc_y) - int16(gravity_y .* 8192);
linear_acc_z = int16(raw_acc_z) - int16(gravity_z .* 8192);
linear_acc = [linear_acc_x linear_acc_y linear_acc_z];

x=double(linear_acc);
y=x;
% plot(y);
L=length(y);
Fs=60;
t=(0:L-1)*(1/Fs);
figure;
subplot(311);plot(t, y(:,1),'r');
subplot(312);plot(t, y(:,2),'g');
subplot(313);plot(t, y(:,3),'b');
%% Band pass filter LP & HP
[b,a]=butter(5,(8/30));
filt_y= filtfilt(b,a,y);
[B,A]=butter(5,(0.05/30),'high');
filt_y=filtfilt(B,A,filt_y);
figure;
subplot(311);plot(t,filt_y(:,1),'r');
subplot(312);plot(t,filt_y(:,2),'g');
subplot(313);plot(t,filt_y(:,3),'b');
% 
% a = 0;




%% Pre-processing
% t=(0:length(double(linear_acc(:,1)))-1)*(1/Sampling_Freq);
% figure;
% subplot(311);plot(t, double(linear_acc(:,1)),'r');
% subplot(312);plot(t, double(linear_acc(:,2)),'g');
% subplot(313);plot(t, double(linear_acc(:,3)),'b');


% quat_process = pre_processing(quat);
% euler_angles_process = pre_processing(euler_angles);
% ypr_process = pre_processing(ypr);
% linear_acc_process = pre_processing(double(linear_acc));
% world_acc_process = pre_processing(world_acc);

% figure;
% subplot(311);plot(t, double(linear_acc_process(:,1)),'r');
% subplot(312);plot(t, double(linear_acc_process(:,2)),'g');
% subplot(313);plot(t, double(linear_acc_process(:,3)),'b');

linear_acc_process = double(filt_y) ./ 16384;


%% Pre-processing
%%-Time Domain

%%Statistical Metrics: Mean, Variance and Standard Deviation
mean_acc = mean(linear_acc_process);
std_acc = std(linear_acc_process);
variance_acc = var(linear_acc_process);

%Envelope Metrics: Median, Maximum, Minimum and Range
median_acc = median(linear_acc_process);
min_acc = min(linear_acc_process);
max_acc = max(linear_acc_process);

%Root Mean Square (RMS) Metric
rms_acc = rms(linear_acc_process);

%%Position and Velocity using Numeric Integration
% For now ignore

%%Signal Correlation and Correlation-Coefficient
corr_acc = corr(linear_acc_process);


% Signal autocorrelation
auto_corr_x = xcorr(linear_acc_process(:,1));
auto_corr_y = xcorr(linear_acc_process(:,2));
auto_corr_z = xcorr(linear_acc_process(:,3));


wintype = 'rectwin';
winlen = 201;
winamp = [0.5,1]*(1/winlen);

% find the zero-crossing rate
zc = zerocross(linear_acc_process(:,2),wintype,winamp(1),winlen);

% find the energy
E = energy(linear_acc_process(:,2),wintype,winamp(2),winlen);
Sum_of_ZC = sum(zc);
Sum_of_Energy = sum(E);

%Signal Magnitude area
SMA_acc = (sum(abs(linear_acc_process(:,1) .*  timestamp) * 1000) + sum(abs(linear_acc_process(:,2) .*  timestamp) * 1000) +...
    sum(abs(linear_acc_process(:,3) .*  timestamp) * 1000))./sum(timestamp);


%Sample differences Power
pwrTot_acc = [bandpower(linear_acc_process(:,1),Sampling_Freq,[0 Sampling_Freq/4]) bandpower(linear_acc_process(:,2),Sampling_Freq,[0 Sampling_Freq/4])...
    bandpower(linear_acc_process(:,3),Sampling_Freq,[0 Sampling_Freq/4])];
pwrTotdiff_acc = [bandpower(diff(linear_acc_process(:,1)),Sampling_Freq,[0 Sampling_Freq/4]) bandpower(diff(linear_acc_process(:,2)),Sampling_Freq,[0 Sampling_Freq/4]) ...
    bandpower(diff(linear_acc_process(:,3)),Sampling_Freq,[0 Sampling_Freq/4])];

%Signal Vector magnitude
SVM_acc = sum((linear_acc_process(:,1) .^2) + (linear_acc_process(:,2) .^2) + (linear_acc_process(:,3) .^2))./ length(linear_acc_process(:,1));

% Entropy
e = wentropy(double(linear_acc_process),'shannon');


%%-Frequency Domain

%Absolute freq, PSD of signal, Power, Median freq
[linear_acc_freq linear_acc_psd Power_from_PSD Info_Entropy median_acc_freq] = frequency_response(double(linear_acc_process), Sampling_Freq);

% DC component
dc_acc = linear_acc_freq(1);

% Spectral Analysis of Key Coefficients (from 0 to 8 Hz)

max_value1 = max(find(linear_acc_psd(:,1).Frequencies < 8));

key_coeff_acc(1) =  sum(linear_acc_psd(:,1).Data(1:max_value1));
key_coeff_acc(2) =  sum(linear_acc_psd(:,2).Data(1:max_value1));
key_coeff_acc(3) =  sum(linear_acc_psd(:,3).Data(1:max_value1));


figure; subplot(211);plot(zc(1:size(linear_acc_process(:,2),1)));subplot(212);plot(E(1:size(linear_acc_process(:,2),1)));
