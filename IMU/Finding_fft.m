clc;
clear all;
close all;


%%   Data Acquisition

% Read the raw data file
filename = 'C:\IMU\new data\Recording-1-dataDG10';

Sampling_Freq = 100;
%% Load acceleration data from file.
load(strcat(filename,'.mat'));

linear_acc = linear_acc(10*Sampling_Freq:end,:);
timeo = 1:length(linear_acc);
timestamp = [1 diff(timeo)];
timestamp = timestamp';

%linear_acc_process = linear_acc;
y = linear_acc;
% plot(y);
L = length(y);  
t = (0:L-1)*(1/Sampling_Freq);
figure;
subplot(311);plot(t, y(:,1),'r');
subplot(312);plot(t, y(:,2),'g');
subplot(313);plot(t, y(:,3),'b');

saveas(figure(1), strcat(filename,'-input'),'fig');
%% Band pass filter LP & HP
[b,a]=butter(5,(8/30));
filt_y= filtfilt(b,a,y);
[B,A]=butter(5,(0.05/30),'high');
linear_acc_process=filtfilt(B,A,filt_y);
figure;
subplot(311);plot(t,linear_acc_process(:,1),'r');
subplot(312);plot(t,linear_acc_process(:,2),'g');
subplot(313);plot(t,linear_acc_process(:,3),'b');


[linear_acc_freq, linear_acc_psd, Power_from_PSD, Info_Entropy, median_acc_freq] = frequency_response(double(linear_acc_process(:,1)), Sampling_Freq);
