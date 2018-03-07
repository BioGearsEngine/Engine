function [Pointwise_Residuals,Pointwise_PercentError,MSE_D,MSE_C,MSE_T,LSE_D,LSE_C,LSE_T] = stats(i,UNC,BG,first_transition,second_transition)
%%    
    %%%%%%%%%% POINTWISE ANALYSIS %%%%%%%%%%
%{
    !!!!! THIS ASSUMES THAT THE UNC DATA WILL ALWAYS HAVE !!!!!
    !!!!! FEWER DATAPOINTS THAN THE BIOGEARS OUTPUT FILES !!!!!
%}

[Lunc,Wunc] = size(UNC);

%% find the points that are viable to be compared 
for j = 1:Lunc
    An = abs(BG(:,1) - UNC(j,1));
    Amin = min(An);
    Ai = find(Amin == An);
    
    if size(Ai) == [1,1]
       A(j,1) = BG(Ai,31);
    else
        A(j,1) = BG(Ai(1:1),31);
    end
end

%% Calculate the Pointwise Residuals and Percent Error
Pointwise_Residuals(:,1) = abs(UNC(:,2)-A(:,1)); 

Pointwise_PercentError(:,1) = 100*abs(UNC(:,2)-A(:,1))./UNC(:,2);

%% Calculate the Least Squares Error and Mean Square Error for each comparable point

% This is done for the diffusion, transitional, and clearance regions of
% the concentration-time curve.

LSE_D = 0;
LSE_C = 0;
LSE_T = 0;
D_counter = 0;
C_counter = 0;
T_counter = 0;
for j = 1:Lunc
    if UNC(j,1) <= first_transition
        D_counter = D_counter + 1;
        LSE_D = (A(j,1) - UNC(j,2))^2 + LSE_D;
    elseif UNC(j,1) >= second_transition
        C_counter = C_counter + 1;
        LSE_C = (A(j,1) - UNC(j,2))^2 + LSE_C;
    elseif UNC(j,1) < second_transition && UNC(j,1) > first_transition
        T_counter = T_counter + 1;
        LSE_T = (A(j,1) - UNC(j,2))^2 + LSE_T;
    end    
end


MSE_D = LSE_D / D_counter;
MSE_C = LSE_C / C_counter;
MSE_T = LSE_T / T_counter;

end










