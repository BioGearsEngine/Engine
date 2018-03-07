function [second_derivative] = discrete_differentiation(smooth_BG)

%this function takes the data from BG results and finds the 
%second derivative of the data, allowing for the determination of
%transitional points in the concentration curves

%since this function takes the second derivative at discrete points, two
%data points are lost as the derivative between data points can only be
%found --> the number of data points is so large that this is not a problem

A(:,1) = smooth_BG(:,1);
A(:,2) = smooth_BG(:,2);

[L,~] = size(A);

%% first derivative
for i = 1:L-1
    B(i,1) = (A(i,1) + A(i+1,1)) / 2;
    B(i,2) = (A(i,2) - A(i+1,2)) / (A(i,1) - A(i+1,1));
end


[L,~] = size(B);

%% second derivative
for i = 1:L-1
    C(i,1) = (B(i,1) + B(i+1,1)) / 2;
    C(i,2) = (B(i,2) - B(i+1,2)) / (B(i,1) - B(i+1,1)); 
end

second_derivative = C(:,:);

end
