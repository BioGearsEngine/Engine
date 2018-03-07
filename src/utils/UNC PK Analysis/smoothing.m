function [smooth__second_derivative] = smooth_BG(second_derivative)


% this function reads in second derivative data for every point and
% averages each value over a 5-point interval (two points above and two
% points below the current point being averaged)

% this allows for more accurate prediction of transitional regions in the
% data by filtering out fluctuations (noise)


L = length(second_derivative);

count = 0;
for i = 5:L-4
    count = count + 1;
    smooth__second_derivative(count,1) = second_derivative(i,1);
    smooth__second_derivative(count,2) = mean(second_derivative(i-4:i+4,2));
end


end