clc;
clear all;
close all;


%%   Data Acquisition

% Read the raw data file
filename = 'G:\IMU\Data_Set_IMU\dataAGC\IMU\exp_PickNPlace\Dr Ari Chaco';
str = 'Dr Ari Chaco';


rawdata = csvread(strcat(filename,'.csv'));
%rawdata_ = csvread('C:\IMU\new data\test_data_with_movement.csv');

Sampling_Freq = 100;

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
load(strcat(filename,'-acc','.mat'));

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


saveas(figure(2), strcat(filename,'-filtered'),'fig');
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
%corr_acc = corr(linear_acc_process);


% Signal autocorrelation
auto_corr_x = sum(xcorr(linear_acc_process(:,1)));
auto_corr_y = sum(xcorr(linear_acc_process(:,2)));
auto_corr_z = sum(xcorr(linear_acc_process(:,3)));
auto_corr = [auto_corr_x auto_corr_y auto_corr_z];

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


% %Sample differences Power
pwrTot_acc = [bandpower(linear_acc_process(:,1),Sampling_Freq,[0 Sampling_Freq/4]) bandpower(linear_acc_process(:,2),Sampling_Freq,[0 Sampling_Freq/4])...
    bandpower(linear_acc_process(:,3),Sampling_Freq,[0 Sampling_Freq/4])];
pwrTotdiff_acc = [bandpower(diff(linear_acc_process(:,1)),Sampling_Freq,[0 Sampling_Freq/4]) bandpower(diff(linear_acc_process(:,2)),Sampling_Freq,[0 Sampling_Freq/4]) ...
    bandpower(diff(linear_acc_process(:,3)),Sampling_Freq,[0 Sampling_Freq/4])];
% pwrTot_acc =[0 0 0];
% pwrTotdiff_acc = [0 0 0];
%Signal Vector magnitude
SVM_acc = sum((linear_acc_process(:,1) .^2) + (linear_acc_process(:,2) .^2) + (linear_acc_process(:,3) .^2))./ length(linear_acc_process(:,1));

% Entropy
e = wentropy(double(linear_acc_process),'shannon');


%%-Frequency Domain

%Absolute freq, PSD of signal, Power, Median freq
[linear_acc_freq, linear_acc_psd, Power_from_PSD, Info_Entropy, median_acc_freq] = frequency_response(double(linear_acc_process), Sampling_Freq);

% DC component
dc_acc = linear_acc_freq(1);

% Spectral Analysis of Key Coefficients (from 0 to 8 Hz)

max_value1 = find(linear_acc_psd(:,1).Frequencies < 8, 1, 'last' );

key_coeff_acc(1) =  sum(linear_acc_psd(:,1).Data(1:max_value1));
key_coeff_acc(2) =  sum(linear_acc_psd(:,2).Data(1:max_value1));
key_coeff_acc(3) =  sum(linear_acc_psd(:,3).Data(1:max_value1));


Parameters = {'Meanx';'Meany';'Meanz';'Variancex'; 'Variancey'; 'Variancez';'Std-Deviationx';'Std-Deviationy'; 'Std-Deviationz';...
    'Medianx';'Mediany';'Medianz';'Minimumx';'Minimumy';'Minimumz';'Maximumx';'Maximumy';'Maximumz';'RMSx';'RMSy';'RMSz';'AutoCorrcoeffx';...
    'AutoCorrcoeffy';'AutoCorrcoeffz';'Signal Magnitude area';'Power of signalx';'Power of signaly';'Power of signalz';'Sample Differences Powerx';...
    'Sample Differences Powery';'Sample Differences Powerz';'Sum of zero-crossing';'Sum of Energy';'Signal Vector Magnitude';'Entropy';'median_acc_freqx';...
    'median_acc_freqy';'median_acc_freqz';'Info_Entropyx';'Info_Entropyy';'Info_Entropyz';'Power_from_PSDx';'Power_from_PSDy';'Power_from_PSDz';'dc_Component';...
    'key_coefficientx';'key_coefficienty';'key_coefficientz';};
Headers = {'Parameters','Value'};
Dat = [mean_acc variance_acc std_acc median_acc min_acc max_acc rms_acc auto_corr SMA_acc pwrTot_acc pwrTotdiff_acc Sum_of_ZC Sum_of_Energy SVM_acc e...
    median_acc_freq Info_Entropy Power_from_PSD dc_acc key_coeff_acc];

Data = [Parameters,cellfun(@num2str, num2cell(Dat'), 'UniformOutput' , false )]; 


Final = [Headers;Data];

fid = fopen(strcat(filename,'-new','.txt'),'wt');
if fid ~= -1
  fprintf(fid,'%s\r\n',str);       %# rint the string
 % // this iterates fprintf on the cell rows, giving you the output
cellfun(@(x,y) fprintf(fid,'%s\t%s\n',x,y),Final(:,1),Final(:,2));       
fclose(fid);
end


figure; subplot(211);plot(zc(1:size(linear_acc_process(:,2),1)));subplot(212);plot(E(1:size(linear_acc_process(:,2),1)));
saveas(figure(3), strcat(filename,'-zerocrossing'),'fig');
