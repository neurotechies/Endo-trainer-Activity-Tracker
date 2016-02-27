clc;
clear all
close all

%%   Data Acquisition

% Read the raw data file
rawdata = csvread('C:\IMU\IMU_data\sample_mpu\Dr_Natesan_07_07_2014_1830_proper.csv');
%rawdata_ = csvread('C:\IMU\new data\test_data_with_movement.csv');


%Get the quaternion values (in float)
timestamp = diff(rawdata(:,1));
%as = timestamp > 0;
%as = timestamp;
%rawdata = rawdata_(as, :);
%timestamp = timestamp(as);
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

%% Load acceleration data from file.
%load('raw_acc_drnatesan.mat');
raw_acc_x = raw_acc(:,1);
raw_acc_y = raw_acc(:,2);
raw_acc_z = raw_acc(:,3);

%%  Derive the Euler angles from Quaternion
%
%data[0] = atan2(2*q -> x*q -> y - 2*q -> w*q -> z, 2*q -> w*q -> w + 2*q -> x*q -> x - 1);   // psi
%data[1] = -asin(2*q -> x*q -> z + 2*q -> w*q -> y);                              // theta
%data[2] = atan2(2*q -> y*q -> z - 2*q -> w*q -> x, 2*q -> w*q -> w + 2*q -> z*q -> z - 1);   // phi

euler_psi = (atan2((2 .* quat(:,2) .* quat(:,3)) - (2.* quat(:,1) .* quat(:,4)), ...
            (2 .* quat(:,1) .* quat(:,1)) + (2 .* quat(:,2) .* quat(:,2)) - 1)) .* 180/pi;

euler_theta = -(asin((2 .* quat(:,2) .* quat(:,4)) + (2.* quat(:,1) .* quat(:,3)))) .* 180/pi;

euler_phi = (atan2((2 .* quat(:,3) .* quat(:,4)) - (2.* quat(:,1) .* quat(:,2)), ...
            (2 .* quat(:,1) .* quat(:,1)) + (2 .* quat(:,4) .* quat(:,4)) - 1)) .* 180/pi;

euler_angles = [euler_psi euler_theta euler_phi];

%% %Derive yaw, pitch, roll from quaternion and gravity
%
%     // yaw: (about Z axis)
%     data[0] = atan2(2*q -> x*q -> y - 2*q -> w*q -> z, 2*q -> w*q -> w + 2*q -> x*q -> x - 1);
%     // pitch: (nose up/down, about Y axis)
%     data[1] = atan(gravity -> x / sqrt(gravity -> y*gravity -> y + gravity -> z*gravity -> z));
%     // roll: (tilt left/right, about X axis)
%     data[2] = atan(gravity -> y / sqrt(gravity -> x*gravity -> x + gravity -> z*gravity -> z));

% about z axis
yaw = (atan2((2 .* quat(:,2) .* quat(:,3)) - (2.* quat(:,1) .* quat(:,4)), ...
      (2 .* quat(:,1) .* quat(:,1)) + (2 .* quat(:,2) .* quat(:,2)) - 1)) .* 180/pi;

% about y axis
pitch = (atan(gravity_x ./ sqrt(gravity_y .* gravity_y + gravity_z .* gravity_z))) .* 180/pi;

% about x axis
roll = (atan(gravity_y ./ sqrt(gravity_x .* gravity_x + gravity_z .* gravity_z))) .*180/pi;

ypr = [yaw pitch roll];
[pitch1, roll1, yaw1] = quat2angle(quat, 'YXZ');
ypr1 = [yaw1 pitch1 roll1];

ypr1 = ypr1 .* 180/pi;

%% %Derive linear acceleration
%
%
%     // get rid of the gravity component (+1g = +8192 in standard DMP FIFO packet, sensitivity is 2g)
%     v -> x = vRaw -> x - gravity -> x*8192;
%     v -> y = vRaw -> y - gravity -> y*8192;
%     v -> z = vRaw -> z - gravity -> z*8192;
% 

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
%% Derive world acceleration
%
%
%     // rotate measured 3D acceleration vector into original state
%     // frame of reference based on orientation quaternion
%     memcpy(v, vReal, sizeof(VectorInt16));
% 	
% 	// P_out = q * P_in * conj(q)
% 	// - P_out is the output vector
% 	// - q is the orientation quaternion
% 	// - P_in is the input vector (a*aReal)
% 	// - conj(q) is the conjugate of the orientation quaternion (q=[w,x,y,z], q*=[w,-x,-y,-z])
% 
% 	Quaternion p(0, v->x, v->y, v->z);
% 
% 	// quaternion multiplication: q * p, stored back in p
% 	p = q -> getProduct(p);
% 
% 	// quaternion multiplication: p * conj(q), stored back in p
% 	p = p.getProduct(q -> getConjugate());
% 
% 	// p quaternion is now [0, x', y', z']
% 	v->x = p.x;
% 	v->y = p.y;
% 	v->z = p.z;

lin_acc_quat = [zeros(size(linear_acc_x,1), 1) linear_acc_x linear_acc_y linear_acc_z];
%lin_acc_quat = [zeros(size(gravity_x,1), 1) gravity_x gravity_y gravity_z];
%lin_acc_quat = double(lin_acc_quat);
lin_acc_quat = quat_get_product(quat, double(lin_acc_quat));
% Quaternion(w, -x, -y, -z);
quat_conj = [quat(:,1) (-1 .* quat(:,2)) (-1 .* quat(:,3)) (-1 .* quat(:,4))]; 
lin_acc_quat = quat_get_product(lin_acc_quat, quat_conj);

world_acc_x = lin_acc_quat(:, 2);
world_acc_y = lin_acc_quat(:, 3);
world_acc_z = lin_acc_quat(:, 4);
world_acc = [world_acc_x world_acc_y world_acc_z];


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
quat_process = double(quat);
euler_angles_process = double(euler_angles);
ypr_process = double(ypr);
linear_acc_process = double(filt_y) ./ 16384;
world_acc_process = double(world_acc);

a = 0;


%% Processing

% % plot3(linear_acc_process(:,1), linear_acc_process(:,2), linear_acc_process(:,3));
% % xlabel('x')
% % ylabel('y')
% % zlabel('z')
% % grid on
% % axis square

% u = gradient(linear_acc_process(:,1));
% v = gradient(linear_acc_process(:,2));
% w = gradient(linear_acc_process(:,3));
% scale = 0;
% quiver3(linear_acc_process(:,1),linear_acc_process(:,2),linear_acc_process(:,3),u,v,w,scale);
% xlabel('x')
% ylabel('y')
% zlabel('z')
% grid on
% axis square

%%% Frequency response
L = length(rawdata);
NFFT = 2^nextpow2(L);
f_b = (0:(NFFT/2)-1)*(Sampling_Freq/NFFT);
[linear_acc_freq linear_acc_psd Power_from_PSD Info_Entropy median_acc_freq] = frequency_response(double(linear_acc_process), Sampling_Freq);
% [quat_freq, quat_psd] = frequency_response(quat_process,Sampling_Freq);
% [euler_angles_freq euler_angles_psd] = frequency_response(euler_angles_process, Sampling_Freq);
% [ypr_freq ypr_psd]= frequency_response(ypr_process, Sampling_Freq);
% [linear_acc_freq linear_acc_psd] = frequency_response(double(linear_acc_process), Sampling_Freq);
% [world_acc_freq world_acc_psd]= frequency_response(world_acc_process, Sampling_Freq);

%%% 
% a = 0; 
% euler_angles_process
% ypr_process
% linear_acc_process
% world_acc_process

% Jerk = sqrt((dXacc/dt)^2 +  (dYacc/dt)^2 + (dZacc/dt)^2)
% Motion smoothness = J = sqrt((1/2) * integral(j^2. dt))

%%Curvature = ||v X a|| ./||v||.^3

velocity(:,1) = (linear_acc_process(:,1) .*  timestamp(1:end)) * 1000;
velocity(:,2) = (linear_acc_process(:,2) .*  timestamp(1:end)) * 1000;
velocity(:,3) = (linear_acc_process(:,3) .*  timestamp(1:end)) * 1000;

curvature = cross(velocity ./ norm(velocity), linear_acc_process ./ linear_acc_process);
curvature = ((curvature ./ norm(curvature)) ./ ((velocity ./ norm(velocity)) .^ 3));





Jerk_lin_acc = (((diff(linear_acc_process(:,1))./ timestamp(1:end-1)) .^ 2) ...
               + ((diff(linear_acc_process(:,2))./ timestamp(1:end-1)) .^ 2)...
               + ((diff(linear_acc_process(:,3))./ timestamp(1:end-1)) .^ 2)) ;
           
Jerk_acc = sqrt(((diff(linear_acc_process(:,1))./ timestamp(1:end-1)) .^ 2) ...
               + ((diff(linear_acc_process(:,2))./ timestamp(1:end-1)) .^ 2)...
               + ((diff(linear_acc_process(:,3))./ timestamp(1:end-1)) .^ 2)) ;

smooth_val = sqrt(0.5 * (sum(Jerk_lin_acc .* timestamp(1:end-1))));
      


[aa, bb] = frequency_response(Jerk_lin_acc, Sampling_Freq);
% figure;plot(bb.Frequencies,10*log10(bb.Data), 'r');title('PSD of B'); xlabel('Hz'); ylabel('dB/Hz');
% 
% % 
% % fun = @(x) exp(-x.^2).*log(x).^2;
% % 
% % Evaluate the integral from x=0 to x=Inf.
% % 
% % q = integral(fun,0,Inf)
% 
% 
% 
% 
% figure;plot(quat_psd(1).Frequencies,10*log10(quat_psd(1).Data), 'r');
% title('PSD of B'); xlabel('Hz'); ylabel('dB/Hz');
% hold on
% 
% figure;subplot(311);plot(quat);axis tight;grid on;
% hold on
% xlabel('y'), ylabel('Acceleration in Y')
% title('Britty')
% 
% subplot(312);plot(quat_process);axis tight;grid on;
% subplot(313);plot(f_b,quat_freq(:,1));axis tight;grid on;





