function spline_pl(x,y,k)
% SPLINE_PL
%    x,y - two vectors of the same size, point coordinates
%    k - parametrization parameter: 
%     t-distances are  proportional to (chord length)^k.
%      typical values:
%            k=0 - uniform,   k=1 - linear,    k=0.5 - centripetal 
dx=diff(x);dy=diff(y);
dt=(dx.^2+dy.^2).^(k/2);
[m,n]=size(dt);if m<n, dt=dt';end % let it be a column
t=cumsum([0;dt])/sum(dt); % normalization
NN=10000; % plot
tt=linspace(0,1,NN);
xx=spline(t,x,tt); yy=spline(t,y,tt);
plot(x,y,'*k',xx,yy,'r')
axis equal