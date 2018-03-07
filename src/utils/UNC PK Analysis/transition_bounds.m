function [first_transition,second_transition] = transition_bounds(second_derivative,lower_bound,Current_NAME)

%this function takes the second derivative information and finds transition
%points that denote the diffusion dominated and clearance dominated 
%portions of the plasma concentration vs. time curve  


%% find the first transition point --> point that begins transition region

L = length(second_derivative);
epsT = 0.001;
tf = 1;
first_transition = [];
index = floor((1 / 10) * second_derivative(end,1));
counter = 0;



% each drug has slightly different PK respsonses, thus each drug has
% different stopping criteria (epsT) that denotes transitions between
% characteristic portions of the plasma concentration vs. time curve


derp = strcmp(Current_NAME,'Ketamine');
if derp == 1
    epsT = 0.00009;
end

derp = strcmp(Current_NAME,'Propofol');
if derp == 1
    epsT = 0.008;
end
    
derp = strcmp(Current_NAME,'Prednisone');
if derp ==1
    epsT = 0.00005;
end 

derp = strcmp(Current_NAME,'Morphine');
if derp ==1
    epsT = 0.0001;
end 

derp = strcmp(Current_NAME,'Midazolam');
if derp ==1
    epsT = 0.000008;
end 

derp = strcmp(Current_NAME,'Naloxone');
if derp ==1
    epsT = 0.0000003;
end

while tf == 1
    epsT = epsT * .9999;
    for i = index:-1:lower_bound+1
        counter = counter + 1;
        holder(i) = second_derivative(i,2);
        if i <= index - 3
            if mean(holder(i:i + 2)) >= epsT
                first_transition = i;
                tf = 0;
                break
            end
        end
    end
end
    

%% find the second transition point --> where clearance is dominant

iteration_number = 0;
epsT = 0.00003;
tf = 1;
second_transition = [];

derp = strcmp(Current_NAME,'Ketamine');
if derp == 1
    epsT = 0.0000045;
end

derp = strcmp(Current_NAME,'Propofol');
if derp == 1
    epsT = 0.001;
end

derp = strcmp(Current_NAME,'Prednisone');
if derp ==1
    epsT = 0.0000015;
end

derp = strcmp(Current_NAME,'Morphine');
if derp ==1
    epsT = 0.0000001;
end 

derp = strcmp(Current_NAME,'Midazolam');
if derp ==1
    epsT = 0.0000003;
end 

derp = strcmp(Current_NAME,'Naloxone');
if derp ==1
    epsT = 0.0000004;
end


% general criteria
g = 25

% special cases for a larger averaging samples
if strcmp(Current_NAME,'Midazolam') ==1
    g = 5000
end



'second_transition'
while tf == 1
    counter = 0;
    for j = first_transition:L
        counter = counter + 1;
        holder(j) = second_derivative(j,2);
        if counter >= g + 1
            if mean(holder((j-g):j)) <= epsT
                second_transition = j;
                tf = isempty(second_transition);
                if tf == 0
                    break
                end
            end
        end
    end
    epsT = epsT + .0000001
end

end











