clc
clear all 
close all
britty = tdfread('E:\Official\IITD\computer vision\Assignments\Programs\Matlab\sleight\accData\Britty-2.txt');
ramandeep = tdfread('E:\Official\IITD\computer vision\Assignments\Programs\Matlab\sleight\accData\Ramandeep-0deg.txt');
vinkle = tdfread('E:\Official\IITD\computer vision\Assignments\Programs\Matlab\sleight\accData\vinkle-0deg.txt');

%%
field_b = fieldnames(britty);
field_b = field_b{1};
britty = britty.(field_b);
avgtimestamp = sum(britty (:, 4))./size(britty (:, 4), 1);
Sampling_Freq = 1 / avgtimestamp;
britty = britty(:, 1:3);
britty = britty(3250:6750,:);
britty = britty(:, 2);



% Preprocessing

%preprocessing
m_b = mean(britty);
Xm_b = britty - m_b;
Xn_b = Xm_b./(max(abs(Xm_b)));

%FFT
L_b = length(Xn_b);
NFFT_b = 2^nextpow2(L_b);
Y_b = fft(Xn_b,NFFT_b);
Ymag_b = abs(Y_b);
Ymag_b = Ymag_b(1:NFFT_b/2);
f_b = (0:(NFFT_b/2)-1)*(Sampling_Freq/NFFT_b);

% Moving Average Filter
% N=16;
% w=rectwin(N)/N;
% y1=filter(w,1,britty);

field_r = fieldnames(ramandeep);
field_r = field_r{1};
ramandeep = ramandeep.(field_r);
ramandeep = ramandeep(:, 1:3);
ramandeep = ramandeep(5250:8750,:);
ramandeep = ramandeep(:,2);


%preprocessing
m_r = mean(ramandeep);
Xm_r = ramandeep - m_r;
Xn_r = Xm_r./(max(abs(Xm_r)));

%FFT
L_r = length(Xn_r);
NFFT_r = 2^nextpow2(L_r);
Y_r = fft(Xn_b,NFFT_r);
Ymag_r = abs(Y_r);
Ymag_r = Ymag_r(1:NFFT_r/2);
f_r = (0:(NFFT_r/2)-1)*(Sampling_Freq/NFFT_r);

field_v = fieldnames(vinkle);
field_v = field_v{1};
vinkle = vinkle.(field_v);
vinkle = vinkle(:, 1:3);
vinkle = vinkle(1450:11350, :);
vinkle = vinkle(:,2);

%preprocessing
m_v = mean(vinkle);
Xm_v = vinkle-m_v;
Xn_v = Xm_v./(max(abs(Xm_v)));

%FFT
L_v = length(Xn_v);
NFFT_v = 2^nextpow2(L_v);
Y_v = fft(Xn_v,NFFT_v);
Ymag_v = abs(Y_v);
Ymag_v = Ymag_v(1:NFFT_v/2);
f_v = (0:(NFFT_v/2)-1)*(Sampling_Freq/NFFT_v);

psdestb = psd(spectrum.periodogram,britty,'Fs',Sampling_Freq,'NFFT',NFFT_b);
psdestr = psd(spectrum.periodogram,ramandeep,'Fs',Sampling_Freq,'NFFT',NFFT_r);
psdestv = psd(spectrum.periodogram,vinkle,'Fs',Sampling_Freq,'NFFT',NFFT_v);

plot(psdestb.Frequencies,10*log10(psdestb.Data), 'r');
title('PSD of B'); xlabel('Hz'); ylabel('dB/Hz');
hold on
plot(psdestr.Frequencies,10*log10(psdestr.Data), 'b');
title('PSD of R'); xlabel('Hz'); ylabel('dB/Hz');

hold on
plot(psdestv.Frequencies,10*log10(psdestv.Data), 'g');
title('PSD of V'); xlabel('Hz'); ylabel('dB/Hz');
clear field_b field_r field_v


figure;subplot(311);plot(britty);axis tight;grid on;
hold on
xlabel('y'), ylabel('Acceleration in Y')
title('Britty')

subplot(312);plot(Xn_b);axis tight;grid on;
subplot(313);plot(f_b,Ymag_b);axis tight;grid on;


figure;subplot(311);plot(ramandeep);axis tight;grid on;
hold on
xlabel('y'), ylabel('Acceleration in Y')
title('Ramandeep')

subplot(312);plot(Xn_r);axis tight;grid on;
subplot(313);plot(f_r,Ymag_r);axis tight;grid on;


figure;subplot(311);plot(vinkle);axis tight;grid on;
hold on
xlabel('y'), ylabel('Acceleration in Y')
title('Vinkle')

subplot(312);plot(Xn_v);axis tight;grid on;
subplot(313);plot(f_v,Ymag_v);axis tight;grid on;
% plot((1:size(ramandeep(:,2))), ramandeep(:,2), 1:size(britty(:,2)), britty(:,2), 1:size(vinkle(:,2)), vinkle(:,2));
% plot(ramandeep);
% hold on
% grid on
% xlabel('y'), ylabel('Acceleration in Y')
% title('Ramandeep')
% % figure
% % scatter3(ramandeep(:, 1), ramandeep(:, 2), ramandeep(:, 3));
% % 
% figure
% plot(vinkle);
% hold on
% grid on
% xlabel('y'), ylabel('Acceleration in Y')
% title('Vinkle')
