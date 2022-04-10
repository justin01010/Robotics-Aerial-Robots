data = xlsread('HW4-1.xls');
A = data(:,1:2);
Y = data(:,3);
x = inv(A'*A)*A'*Y;

fprintf('The ideal linear regression model is Y = A * x, where\n    Y: input\n    A: output\n    x =\n')
disp(x)

% compare 
    %disp([A*x,Y])


