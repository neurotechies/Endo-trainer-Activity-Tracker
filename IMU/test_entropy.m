
clc;
clear all;
close all;
Fs = 1000;                    % Sampling frequency
T = 1/Fs;                     % Sample time
L = 1000;                     % Length of signal
t = (0:L-1)*T;                % Time vector
% Sum of a 50 Hz sinusoid and a 120 Hz sinusoid
x = 0.7*sin(2*pi*50*t); 
y = x + 0.05*randn(size(t));     % Sinusoids plus noise
e1 = wentropy(x,'shannon');
e2 = wentropy(y,'shannon');
e = [e1;e2];
figure; plot(Fs*t(1:50),x(1:50))
title('Signal Corrupted with Zero-Mean Random Noise')
xlabel('time (milliseconds)');
figure; plot(Fs*t(1:50),y(1:50))
title('Signal Corrupted with Zero-Mean Random Noise')
xlabel('time (milliseconds)');
NFFT = 2^nextpow2(L); % Next power of 2 from length of y
Y1 = fft(x,NFFT)/L;
Y2 = fft(y,NFFT)/L;
f = Fs/2*linspace(0,1,NFFT/2+1);
normalized_Y1 = Y1(2:end)/max(abs(Y1(:)));
ef1 = wentropy(normalized_Y1,'shannon');


normalized_Y2 = Y2(2:end)/max(abs(Y2(:)));
ef2 = wentropy(normalized_Y2,'shannon');
ef = [ef1; ef2];
% Plot single-sided amplitude spectrum.
figure; plot(f,2*abs(Y1(1:NFFT/2+1))) 
title('Single-Sided Amplitude Spectrum of y(t)')
xlabel('Frequency (Hz)')
ylabel('|Y(f)|')

% Plot single-sided amplitude spectrum.
figure; plot(f,2*abs(Y2(1:NFFT/2+1))); 
title('Single-Sided Amplitude Spectrum of y(t)')
xlabel('Frequency (Hz)')
ylabel('|Y(f)|')

