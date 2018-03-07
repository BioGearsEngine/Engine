
close all;
clc;

%%
%{   

    This MATLAB script reads in PK data provided by UNC and from the 
    BioGears scenario runs. This script then performs statistical analysis 
    of each dataset and makes comparisons. Residuals and percent error 
    are plotted in addition to the raw data and corresponding fits. Linear
    regressions are found for the diffusion dominated and clearance
    dominated portions of the BG concentration vs. time curves. These
    linear fits are used to determine the viability of BG to represent the
    scenarios presented in the UNC PK data.

%}



% UNC PK data is in a text file and consists of one header line
% and two columns of data --> (time, concentration)


%%

% creates a path to the BG trunk

pathbase = [pwd,'\..\..\..'];

%%
%%%%%% create array of character strings for the drug names %%%%%%
% This character string array is used to cycle through each drug

NAMES = {'Fentanyl';'Furosemide';'Ketamine';'Midazolam';'Morphine';'Naloxone';'Pralidoxime';...
    'Prednisone';'Propofol';'Rocuronium';'Succinylcholine';};

NAMES = {'Albuterol'};

%%%%%% create array of character strings for the BG and UNC data %%%%%%

%%% Num_Files provides the upper limit for the file iteration loop
Num_Files = length(NAMES);

EC50=0;


%% File iteration loop begins here. The for loop where the bulk of the script 
% is contained simply calls the next drug files as i is incremented. This 
% process is continued until all drug files have been analyzed. 

for i = 1:Num_Files
   
   
    %% Current_NAME is saved to easily reference the current drug name
    % It is used to create titles, column names, etc.
    %%% save the name of the current drug being analyzed %%%
    Current_NAME = char(NAMES(i));
    
    %%% save the name of the current drug being analyzed %%%
    
    %% Create BG and UNC filenames
    
    Experimental = [Current_NAME, '.txt'];
    
    BG = [Current_NAME, 'ValidationResults.txt'];
    
    %% Create path to filenames in the bin directory
    
    BGdatapath = [pathbase, '\bin\Scenarios\Validation\',BG];
    Experimentaldatapath = [pathbase, '\docs\Validation\Pharmacokinetic\',Experimental];
    imagebase = [pathbase, '\docs\Doxygen\html\images\Drugs\'];
    
    
    %% Check to see if file exists before opening
    
    if exist(BGdatapath,'file') == 0 || exist(Experimentaldatapath,'file') == 0
        continue
    end
    
    %% Read in all the data for the current drug
    %%%%%%%% Import Data from current directory %%%%%%%%
    
    Experimental = importdata(Experimentaldatapath,'\t',1);
    Experimental = Experimental.data;
    
    BG = importdata(BGdatapath);
    %%BG = readtable(BGdatapath,'Delimiter','\t');
    %%% use string search to find correct column of data
    % the variable 'check' is saved as the column number that contains the
    % plasma concentration data --> 'check' is now the reference column for
    % the BG plasma concentration data
    %%check = BG.Properties.VariableNames;
    %%check = strsplit(char(BG.textdata));
    %%column_title = [Current_NAME,'_PlasmaConcentration_ug_mL_'];
    %%check = find(strcmp(check,column_title) == 1); %finds correct column for plasma concentration data
    check = 2;
    %%% use string search to find correct column of data                                                   
    
    
    %% Below is a logic check to ensure that the variable 'check' contains
    %%% a number and is not empty. If 'check' is empty, there is not a
    %%% column with the current drug's plasma concentration data and a
    %%% warning is presented. The loop restarts with i incremented by 1,
    %%% moving on to the next drug.
    
    tf = isempty(check);
    if tf == 1
        msg1 = 'WARNING!!!';
        msg2 = ['THERE IS NO PLASMA CONCENTRATION DATA FOR: ', Current_NAME];
        disp(msg1)
        disp(msg2)
        
        Avg_PercentError(i,1) = 1/0;
        Max_PercentError(i,1) = 1/0;
        Min_PercentError(i,1) = 1/0;

        Avg_Residuals(i,1) = 1/0;
        Max_Residuals(i,1) = 1/0;
        Min_Residuals(i,1) = 1/0;
        
        continue
    end
    
    % convert the BG table into a cell array
    %%BG = table2cell(BG);
    [L,W] = size(BG.data);
    BG = BG.data;
    %%%%%%%% Import Data from current directory %%%%%%%%
    
    
    %% clear the QNANs out of the data
    % The BG output files contains cells populated by '-1.#QNAN' that need
    % to be reassigned as zeros. This portion of the code replaces the
    % '-1.#QNAN's with zeros and converts the remaining rows to numbers
    % rather than characters
    for j = 1:L
        for k = 1:W
            
            %%% Reassign '-1.#QNAN' as zeros %%%
            tf = strcmp(BG(j,k),'-1.#QNAN');
            if tf == 1
                BG{j,k} = 0;
            end
            
            % Convert numbers from charater strings to reals in the case
            % that there were QNANs present at the begining of the column
            tf = iscellstr(BG(j,k));
            if tf == 1
                BG{j,k} = str2num(BG{j,k});
            end
            
        end
    end
    
    % convert the BG cell array to a double array
    %%BG = cell2mat(BG);
    
    
    %% Apply a fit to the BG data to find the transition points
    
   %%[fitresult, gof] = createFit(BG(:,1),BG(:,check));
    
    
    
    
    
    
%     %% implement smooth function for BG here and use the result to find transition locations
%     % this will take the BG data and smooth out each time step using a
%     % running average, making the transition point determination more
%     % accurate
    
%     [smooth_BG] = smoothBG(BG,check);
%     
%     
%     %% Use the discrete_differentiation function to get a set of discrete
%     % second derivatives in the bounds of BG in order to determine 
%     % transitional regions in the data
%     [second_derivative] = discrete_differentiation(smooth_BG);


%%smooth_BG(:,1) = BG(:,1);
%smooth_BG(:,2) = BG(:,check);

%[second_derivative] = discrete_differentiation(smooth_BG);
%    
%     % take the absolute value of the second derivative data
%     % only need to look at the magnitude of transitions rather than the
%     % direction
%     second_derivative(:,:) = abs(second_derivative(:,:));
%     
%     second_derivative(:,:) = BG(:,:);
    
%     % use lower bound function to find the lower bound of the diffusion
%     % dominant region in the plasma concentration vs. time curve
%     [lower_bound] = lowerbound(second_derivative,Current_NAME);


    %% Find the maximum of the UNC data and set that as the first data point 
        %lower_bound = find(BG(:,check)==max(BG(:,check)));

    
%     %% try smoothing out the second derivative results 
    
%     [smooth_second_derivative] = smoothing(second_derivative);
%     [smoother_second_derivative] = smoothing(smooth_second_derivative);
%     [smootherer_second_derivative] = smoothing(smoother_second_derivative);
%     
%     clear second_derivative;
%     second_derivative(:,:) = smootherer_second_derivative(:,:);
%     clear smoother_second_derivative;
%     clear smootherer_second_derivative;
    
   
%     %% use the transition bounds function to find the first and second transition points
%     [first_transition,second_transition] = transition_bounds(second_derivative,lower_bound,Current_NAME);
    
    
    
%     %% Use stats function to get Pointwise Residuals, Percent Error, and MSE
%     [Pointwise_Residuals,Pointwise_PercentError,MSE_D,MSE_C,MSE_T,LSE_D,LSE_C,LSE_T] = stats(i,UNC,BG,first_transition,second_transition);
%     
%     % save the MSE and LSE for each drug for use in the doxy table
%     MSE_diffusion(i,1) = MSE_D;
%     MSE_clearance(i,1) = MSE_C;
%     MSE_transition(i,1) = MSE_T;
%     LSE_diffusion(i,1) = LSE_D;
%     LSE_clearance(i,1) = LSE_C;
%     LSE_transition(i,1) = LSE_T;
%     
%     
%     
%     %%  creating table
% 
%     Avg_PercentError(i,1) = mean(Pointwise_PercentError);
%     Max_PercentError(i,1) = max(Pointwise_PercentError);
%     Min_PercentError(i,1) = min(Pointwise_PercentError);
% 
%     Avg_Residuals(i,1) = mean(Pointwise_Residuals);
%     Max_Residuals(i,1) = max(Pointwise_Residuals);
%     Min_Residuals(i,1) = min(Pointwise_Residuals);
    
    
    
    
%     %% create text and line objects to put in each region on the plots
%     
%     %these are referenced as distance from the y-axis
%     %bar1_upper = lower_bound + .05*(BG(end,1));
%     %bar2_upper = first_transition + .05*(BG(end,1));
%     
%     bar1_upper = 0.024 * BG(end,1);
%     bar2_upper = 0.024 * BG(end,1);
%     
% 
%     %these are now y-idecies--> distance from the x-axis
%     bar3_lower = BG(second_transition,check);
%     bar3_upper = BG(second_transition,check) + .01*(max(BG(:,check)));
%     bar4_lower = BG(end,check);
%     bar4_upper = BG(end,check) + .01*(max(BG(:,check)));
%     
%     % special cases where the general size doesn't work
%     
%     if strcmp(Current_NAME,'Pralidoxime') == 1
%         bar4_upper = BG(end,check) + .00002*(max(BG(:,check)));
%     end
    
    
    
    
    
%     % text box positions
%     box1_xposition = lower_bound + .05*(BG(end,1));
%     box1_yposition = (BG(lower_bound,check) + BG(first_transition,check)) / 2.4;
%     
%     box2_xposition = (second_transition + BG(end,1)) / 2.4;
%     box2_yposition = BG(end,check) + .03*(max(BG(:,check)));
%     
%     box3_xposition = (first_transition + second_transition) / 2.4;
%     box3_yposition = (BG(first_transition,check) + BG(second_transition,check)) / 1.7;
    
    
    
    %% Plot literature data and BG data as well as transition points 

    fig = figure('units','normalized','outerposition',[0 0 1 1],'Color', 'white');
    scatter(Experimental(:,1)/60,Experimental(:,2),'b', 'filled');
    set(gca,'yscale','log');
    hold on
    semilogy(BG(:,1)/60,BG(:,check), 'LineWidth', 1);
%     hold on
%     bar1 = plot([0 bar1_upper],[BG(lower_bound,check) BG(lower_bound,check)], 'k', 'LineWidth', 2);
%     hold on
%     bar2 = plot([0 bar2_upper],[BG(first_transition,check) BG(first_transition,check)], 'k', 'LineWidth', 2);
%     hold on
%     bar3 = plot([second_transition second_transition],[bar3_lower bar3_upper], 'k', 'LineWidth', 2);
%     hold on
%     bar4 = plot([BG(end,1) BG(end,1)],[bar4_lower bar4_upper], 'k', 'LineWidth', 2);
%     hold on
%     box1 = text(box1_xposition, box1_yposition,'Diffusion Region');
%     box1.FontSize = 10;
%     hold on
%     box2 = text(box2_xposition, box2_yposition,'Clearance Region');
%     box2.FontSize = 10;
%     hold on
%     box3 = text(box3_xposition, box3_yposition, 'Transitional Region');
%     box3.FontSize = 10;
%     hold off
    
    y_name = [Current_NAME,' Plasma Concentration (ug/mL)'];
    if BG(end,1) > Experimental(end,1)
        xlim([0 BG(end,1)/60]);
    else
        xlim([0 Experimental(end,1)/60]);
    end
    xlabel('Time (minutes)', 'FontSize', 10);
    ylabel(y_name, 'FontSize', 10);
    legend('Experimental','BioGears');
    
    Image_NAME = [imagebase,Current_NAME,'PKValidation.png'];
    img = getframe(fig);
    imwrite(img.cdata, [Image_NAME]);
    
    % Find the cmax and EC50 for the drugs
    Cmax = max(BG(:,check));
    EC50(length(EC50)+1)=Cmax/32.0;
    
end
    
    
    
% %% compile table
% inputTable = table(MSE_diffusion,MSE_clearance,MSE_transition,LSE_diffusion,LSE_clearance,LSE_transition,Avg_PercentError,Max_PercentError,Min_PercentError,Avg_Residuals,Max_Residuals,Min_Residuals,'RowNames',NAMES);
% tableName = 'BioGears PK Verification and Validation ';
    
    
    
%% Overwrite NANs in the table as N/A   ----> broken!!!!!

%{
[L,W] = size(inputTable);

for j = 1:L
    for k = 1:W
        tf = isnan(inputTable{j,k});
        if tf == 1
            inputTable{j,k} = 'N/A';
        end
    end
end   
    
%}    
%% put talble into doxyTable function to get table ready for doxygen software

% [doxyFormattedTable] = doxyTable(inputTable,tableName);  






    
    
    