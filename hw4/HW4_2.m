clear all;
data = xlsread('HW4-2.xls');
time = 1:size(data,1);

quaternion = zeros(size(data,1), 4);
quaternion(1,:) = [1 0 0 0];
beta = 0.01;

for i = 1:length(data)-1
    q = quaternion(i,:);
    acc = data(i,:) / norm(data(i,:));

    F = [2*(q(2)*q(4) - q(1)*q(3)) - acc(1)
         2*(q(1)*q(2) + q(3)*q(4)) - acc(2)
         2*(0.5 - q(2)^2 - q(3)^2) - acc(3)];
    J = [-2*q(3), 2*q(4), -2*q(1),	2*q(2)
         2*q(2),  2*q(1),  2*q(4),	2*q(3)
         0,      -4*q(2), -4*q(3),	0    ];
    step = (J'*F);
    step = step / norm(step);

    q = q - beta * step';
    quaternion(i+1,:) = q / norm(q); 
end

% convert quaternion to angle
angle = quatern2euler(quaternConj(quaternion)) * (180/pi);	

subplot(3,1,1);
plot(time,angle(:,1))
title('Attitude Trajectory')

ylabel('\phi (deg)')

subplot(3,1,2);
plot(time,angle(:,2))
ylabel('\theta (deg)')

subplot(3,1,3);
plot(time,angle(:,1))
ylabel('\psi (deg)')
xlabel('Time (s)');