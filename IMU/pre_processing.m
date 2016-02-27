function Xn_b = pre_processing(x)

% %% Pre-Processing
% x = double(x);
% m_b = mean(x);
% 
% for i = 1: length(m_b)
%     Xm_b(:,i) = x(:,i) - m_b(i);
%     Xn_b(:,i) = Xm_b(:,i)./max(abs(Xm_b(:,i)));
% end

%Moving average filter
B = 1/10*ones(10,1);
Xn_b = filtfilt(B,1,x);
