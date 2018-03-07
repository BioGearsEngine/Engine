function [bar1,bar2,bar3,bar4] = bars(BG,lower_bound,first_transition,second_transition,check)

% This function creates the horizontal and vertical lines used to delimit
% the diffustion, transitional, and clearance regions of the plasma
% concentration vs. time curve



%{
%these are referenced as distance from the y-axis
bar1_lower = 1;
bar1_upper = lower_bound + 250;
bar2_lower = 1;
bar2_upper = first_transition + 250;

%these are now y-idecies--> distance from the x-axis
bar3_lower = 1;
bar3_upper = ceil(BG(second_transition,check) + .2*(max(BG(:,check))));
bar4_lower = 1;
bar4_upper = ceil(BG(second_transition,check) + .2*(max(BG(:,end))));


%% need to initialize the arrays with zeros

bar1 = zeros(bar1_upper,2);
bar2 = zeros(bar2_upper,2);
bar3 = zeros(bar3_upper,2);
bar4 = zeros(bar4_upper,2);


%% x-values
% horizontal lines
bar1(:,1) = BG(bar1_lower:bar1_upper,1);
bar2(:,1) = BG(bar2_lower:bar2_upper,1);


% vertical lines
bar3(:,1) = BG(second_transition,1);
bar4(:,1) = BG(end,1);


%% y-values
% horizontal lines
bar1(:,2) = BG(lower_bound,check);
bar2(:,2) = BG(first_transition,check);

% vertical lines
bar3(:,2) = BG(bar3_lower:bar3_upper,check);
bar4(:,2) = BG(bar4_lower:bar4_upper,check);

%}


end











