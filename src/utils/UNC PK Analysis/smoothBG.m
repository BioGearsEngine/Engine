function [smooth_BG] = smoothBG(BG,check)

% the BioGears outputs have fluctuations over each time step that yield
% very large variances in the second derivatives at each point

% to prevent large fluctuations in the second derivative, the BG data is
% smoothed using a running average to preserve the overall trend while
% allowing for more accurate prediction of transition points in the data

L = length(BG);

count = 0;
for i = 5:L-4
    count = count + 1;
    smooth_BG(count,1) = BG(i,1);
    smooth_BG(count,2) = mean(BG(i-4:i+4,check));
end


% only sample every fifth point to make data set more coarse
%{
L = length(smooth_BG);
counter = 0;
for i = 1:L
    if mod(smooth_BG(i,1),5) == 0
        counter = counter + 1;
        holder(counter,2) = smooth_BG(i,2);
        holder(counter,1) = smooth_BG(i,1);
    end
end


clear smooth_BG

smooth_BG(:,:) = holder(:,:);
%}

end