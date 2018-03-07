clear;
clc;
close all;
%%Calculate the LSQ Error of the plasma concentration curves for the
%%BioGears PK model and experimental human data

%% Base Path Assignment
pathbase = [pwd,'\..\..'];

%FileStoragePath = [pathbase, '\..\Publications\IEEEConference2016\'];
FileStoragePath = [pathbase, '\..\Publications\MMVR2016\'];

%% List the drugs being compared and analyzed in an array - Can be used to loop over the drugs
% This character string array is used to cycle through each drug
NAMES = {'Fentanyl';'Ketamine';'Midazolam';'Morphine';'Naloxone';'Pralidoxime';...
    'Prednisone';'Propofol';'Rocuronium';'Succinylcholine';};
% NAMES = {'Morphine'};

%%Single Drug Override of the array
%NAMES = {'Fentanyl';'Ketamine'};

%% Number of drugs being analyzed
Num_Files = length(NAMES);

for i = 1:Num_Files
   
    %% Drug is the name of the drug as pulled from the looped array
    Drug = char(NAMES(i));
    
    %%Experimental Data file
    Experimental = [pathbase,'\docs\Validation\Pharmacokinetic\', Drug, '.txt'];
    %%BioGears Data File
    BG = [pathbase,'\bin\Scenarios\Validation\', Drug, 'ValidationResults.txt'];
    
    %%Import Data
    ExperimentalData = importdata(Experimental,'\t',1);
    ExperimentalData = ExperimentalData.data;
    
    BGData = readtable(BG,'Delimiter',',');
    
    %%Get the table column headers and search for plasma concentration
    ColumnHeaders = BGData.Properties.VariableNames;
    match = 0;
    for j=1:length(ColumnHeaders)
        test = char(ColumnHeaders(j));
        match = ~isempty(strfind(test,[Drug,'_PlasmaConcentration_ug_mL_']));
        if match == 1
            break;
        end
    end
    
    %%Calculate Error between the experimental and computed data
    for k=1:length(ExperimentalData)
        time = ExperimentalData(k,1);
        %Find the corresponding time in the BioGears Data
        if(time == 0)
            RowNumber = 1;
        else
            sampleTime = BGData{2,1} - BGData{1,1};
            if time < sampleTime
                RowNumber = 1;
            else
                RowNumber = round(time/sampleTime);
            end
        end
    
        if RowNumber > length(BGData{:,1})
            break;
        end
        
        if strcmp(BGData{RowNumber,j},'-1.#QNAN')
            error(k) = (ExperimentalData(k,2) - 0)^2;
        else
            error(k) = (ExperimentalData(k,2) - BGData{RowNumber,j})^2;
        end
    end
    RMSError(1,i) = sum(error)^0.5;
    RMSError(2,i) = RMSError(1,i)/(max(ExperimentalData(:,2))-min(ExperimentalData(:,2)));
    
    
    %%Plot experimental vs computed data and save the image in the correct
    %%size and format (initially defined for IEEE standards)
    %%FS = 8; %IEEE
    FS = 16; %Posters
    FN = 'Times New Roman';
    fig = figure;
    semilogy(BGData{:,1},BGData{:,j},'LineStyle','-','LineWidth',2.0,'Color','k')
    hold on
    semilogy(ExperimentalData(:,1),ExperimentalData(:,2),'LineStyle','none','Marker','*','MarkerSize',8,'Color','b')
    xlabel('Time (s)','FontName',FN, 'FontSize',FS)
    ylabel('Plasma Concentration (ug/mL)','FontName',FN, 'FontSize',FS)
    title(Drug,'FontName',FN, 'FontSize',FS)
    %%legend('BioGears', 'Experimental Data')
    ax=gca;
    ax.FontSize = FS;
    ax.FontName = FN;
    ax.XGrid = 'on';
    ax.YGrid = 'on';
    ax.XLim = [0 max(BGData{:,1})];
    
    %%Results Storage
    fig.PaperUnits = 'inches';
    %fig.PaperPosition = [0 0 1.5 1.5]; %IEEE Size
    fig.PaperPosition = [0 0 4 4];
    fig.PaperPositionMode = 'manual';
    FileStorage = [FileStoragePath,Drug,'Plot'];
    if exist(FileStoragePath, 'dir')
    print(FileStorage, '-dtiff');
    else
    mkdir(FileStoragePath)
    print(FileStorage, '-dtiff');
    end     
end

%%Save RMS Error Table
%%Create a table for the RMS Error Results
casenames = {'RMSError','NormalizedRMSError'};
variablenames = NAMES';
FileStorage = [FileStoragePath,'RMSErrorTable.txt'];
T = array2table(RMSError,'RowNames',casenames,'VariableNames',NAMES');
if exist(FileStoragePath, 'dir')
    writetable(T,FileStorage,'WriteRowNames',true);
else
    mkdir(FileStoragePath)
    writetable(T,FileStorage,'WriteRowNames',true);
end

close all;