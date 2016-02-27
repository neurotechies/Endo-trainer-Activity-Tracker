function [Ymag, psd_des,Power_from_PSD,e, med_freq] = frequency_response(x,Sampling_Freq)
%FFT

T = length(x)/Sampling_Freq;
L = length(x);
NFFT = 2^nextpow2(L);
Y = fft(x,NFFT);
Ymag = abs(Y)./L;
Ymag = Ymag(1:NFFT/2, :);
f_b = (0:(NFFT/2)-1)*(Sampling_Freq/NFFT);

figure;
plot(f_b,Ymag)

Exf = Ymag.^2;
% Note: Exf can also be computed as abs(Xf).^2
%
% Pxf is an approximation to the continuous time PSD
%
Pxf = Exf/T;
%
% Assigning frequencies to the samples of the PSD
%
df = Sampling_Freq/L; % Df=freq separation between two consecutive fft points

%Calculating power from the PSD using Parseval's relation:
% i.e., power = integral of (Pxf) over all frequencies.
% This is a Riemann sum approximation to the actual power

Power_from_PSD = sum(Pxf)* df;

% Normalized coefficients 
% Entropy



for i = 1:size(x,2)
psd_des(:,i) = psd(spectrum.periodogram,x(:,i),'Fs',Sampling_Freq,'NFFT',NFFT);
normcumsumpsd = cumsum(psd_des(:,i).Data)./sum(psd_des(:,i).Data);
Ind(:,i) = find(normcumsumpsd <=0.5,1,'last');
normalized_Y(:,i) = Y(:,i)/max(abs(Y(:,i)));
med_freq(:,i) = psd_des(:,i).Frequencies(Ind(:,i));
e(:,i) = wentropy(normalized_Y(:,i),'shannon');
end

end

  