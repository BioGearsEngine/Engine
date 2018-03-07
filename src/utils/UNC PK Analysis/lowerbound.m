function [lower_bound] = lowerbound(second_derivative,Current_NAME)

% This function uses the second derivative to determine at what point the
% plasma concentration curve of a given drug becomes linear with respect to
% time. When the second derivative at a given time is below a tuned cutoff
% parameter, the concentration vs. time curve is linear. This search only
% needs the current time step to be less than the cutoff parameter due to
% the coarse (with respect to time) nature of the data

% The lower bound found below also signifies the beginning of the
% diffusion-dominant portion of the concentration-time curve

L = length(second_derivative);
epsT = 0.002;

derp = strcmp(Current_NAME,'Pralidoxime');
if derp ==1
    epsT = 0.1;
end



tf = strcmp(Current_NAME,'Propofol');
if tf == 1
    epsT = 0.1;
end


for i = 1:L
    holder(i) = second_derivative(i,2);
    if holder(i)<epsT
        lower_bound = i;
        break
    end
end

end
