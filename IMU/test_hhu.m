
fs1 = 10;             % Original sampling frequency in Hz
t1 = 0:1/fs1:1;       % Time vector
x = t1;               % Define a linear sequence
y = resample(x,3,2);  % Now resample it
t2 = (0:(length(y)-1))*2/(3*fs1);  % New time vector
plot(t1,x,'*',t2,y,'o',-0.5:0.01:1.5,-0.5:0.01:1.5,':')
legend('original','resampled'); xlabel('Time')