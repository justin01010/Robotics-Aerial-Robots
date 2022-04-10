clear all;
data = xlsread('HW4-2.xls');
earth = [0;0; -1]; % normalized
data_size = size(data,1);
angle = zeros(data_size,3);

for i=1:size(data,1)
    % find rotation matrix from earth frame to sensor frame S = R * q
    sensor = data(i,:);
    sensor = sensor/norm(sensor);
    R =  sensor'*earth';
    disp(R)
    %disp(R*earth.*9.81)
    % convert rotation matrix to Euler angle
    angle(i,1) = atan2d(R(3,2), R(3,3));                   % roll
    angle(i,2) = atan2d(-R(3,1),(R(3,1)^2+R(3,3)^2)^0.5);  % pitch
    angle(1,3) = atan2d(R(2,1),R(1,1));                    % yaw
end
