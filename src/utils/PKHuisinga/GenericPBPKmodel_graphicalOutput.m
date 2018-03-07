%%%
%%% Version: August 31st, 2012
%%% 
%%% Check at URL: http://www.pharmacometrics.de
%%% for potential updates and comments
%%%
%%% This function plots the diverse graphics to illustrate the simulation
%%% results.
%%% 
%%% Online Supplement to:
%%%
%%%     W. Huisinga, A. Solms, L. Fronton, S. Pilari,
%%%     Modeling Interindividual Variability in Physiologically Based 
%%%     Pharmacokinetics and Its Link to Mechanistic Covariate Modeling,
%%%     Pharmacometrics & Systems Pharmacology (2012) 1, e5; 
%%%     doi:10.1038/psp.2012.3
%%% 
%%%
%%% For details on the INPUT and OUTPUT, please see the
%%% GenericPBPKmodel_README.txt file.
%%%
%%% 
%%% Copyright (C) 2012, Universitaet Potsdam, Germany
%%% Contact: W. Huisinga, huisinga@uni-potsdam.de
%%%
%%% The program is distributed under the terms of the 
%%% Creative Commons License (CC BY-NC-SA 3.0):
%%% Attribution-NonCommercial-ShareAlike 3.0 Unported 
%%%
%%% For a SHORT HUMAN-READABLE SUMMARY OF THE LEGAL CODE, see URL
%%% http://creativecommons.org/licenses/by-nc-sa/3.0/
%%%
%%% For the Legal Code (the full license) see URL
%%% http://creativecommons.org/licenses/by-nc-sa/3.0/legalcode
%%%
%%%

function out = GenericPBPKmodel_graphicalOutput(population,individual,drug,study,ShowGraphics)


OK = 1; % for error reporting. Do not change.

%%% Plot venous blood concentrations for entire population
%%%
if strcmp(ShowGraphics.allindividual.plasma,'yes')
    
    %%% plot venous blood concentrations for entire population
    figNr = ShowGraphics.allindividual.figNr;
    figure(figNr); clf;
    T = individual(1).tissue;
    color = char(individual(1).color);
    t = individual(1).t; C = individual(1).C;
    plot(t/60,C(:,T.ven),color,'LineWidth',1.3);
    xlabel('t[h]'); ylabel('C [mg/l] in ven');
    title(sprintf('Compound = %s',drug.name));
    xlim([0, t(end)/60]); 
    ymin = ShowGraphics.lowerLimitY;
    ymax = max(C(:,T.ven));
    hold on;
    for id=2:length(individual)
        T = individual(id).tissue;
        t = individual(id).t; C = individual(id).C;
        color = char(individual(id).color);
        plot(t/60,C(:,T.ven),color,'LineWidth',1.3);
        ymax = max([ymax, max(C(:,T.ven))]);;
    end;
    hold off; grid on; ylim([ymin ymax]);
    
    set(gca,'YScale',ShowGraphics.allindividual.YScale);

    fprintf('\n');

end;


%%% Give detailed plots for all tissues for specified individuals -----
%%%
if ~isempty(ShowGraphics.allTissues.individual)
        
    if sum(ismember(ShowGraphics.allTissues.individual,1:population.size)==0)
        OK = warning('MATLAB:genericPBPKmodel','\n\n ---> ShowGraphics.allTissues.individual contains invalid ids! <---- \n\n');
        return;
    end;

    colorVec = ['r','b','k','m','g','y','m','b','g','r','k','y','m']; 
    figNr = ShowGraphics.allTissues.figNr; figure(figNr); clf; figure(figNr+1); clf;

    for id = ShowGraphics.allTissues.individual

        figure(figNr); hold on;
        T = individual(id).tissue;
        t = individual(id).t; C = individual(id).C;
        ymax = max(max(C(:,T.allTisExVen)));
        color = colorVec(find(ShowGraphics.allTissues.individual==id)); 
        for n=1:12
            subplot(3,4,n); hold on;
            tis = T.allTisAlphabetic(n);
            plot(t/60,C(:,tis),'Color',color,'LineWidth',1.3);
            xlabel('t[h]'); ylabel(sprintf('C_{%s} [mg/l]',T.name{tis}));
            xlim([0, t(end)/60]); ylim([ymin ymax]); 
            set(gca,'YScale',ShowGraphics.allTissues.YScale);
        end;
        subplot(3,4,2); title(sprintf('drug %s',drug.name),'Fontsize',15);
        set(gca,'YScale',ShowGraphics.allTissues.YScale);

        % vene only  for individual 'id'
        figure(figNr+1); hold on;
        plot(t/60,C(:,T.ven),'Color',color,'LineWidth',1.3);
        xlabel('t[h]'); ylabel('C [mg/l] in ven');
        xlim([0, t(end)/60]); ylim([ymin ymax]); 
        set(gca,'YScale',ShowGraphics.allTissues.YScale);

        if ShowGraphics.allTissues.oneFigOnly
           title(sprintf('drug %s',drug.name),'Fontsize',15);
        else
           title(sprintf('individual %d, drug %s',id,drug.name));
           figNr = ShowGraphics.allTissues.figNr+id+2;
        end;


    end;
    

end;


%%% Plot all tissues for specified individuals in a single plot (not sclaed
%%% and scaled with the partition coefficient
%%%
if ~isempty(ShowGraphics.individual.allTissues)
    
    if sum(ismember(ShowGraphics.individual.allTissues,1:population.size)==0)
        OK = warning('MATLAB:genericPBPKmodel','\n\n ---> ShowGraphics.individual.allTissues contains invalid ids! <---- \n\n');
        return;
    end;
    figNr = ShowGraphics.individual.figNr; 
    style.adi = 'r--'; style.bon = 'b-';
    style.mus = 'b--'; style.ski = 'r-';
    style.ven = 'g-';  style.lun = 'g--';
    style.art = 'g-';  style.bra = 'm-';
    style.liv = 'k-';  style.gut = 'k--';
    style.spl = 'k--'; style.hea = 'm--';
    style.kid = 'm--'; 
    
    
    for id = ShowGraphics.individual.allTissues
        
        %%% plot all concentration-time profiles in a single plot
        figure(figNr); clf; 
        subplot(1,2,1); hold on;
        T = individual(id).tissue;
        t = individual(id).t; C = individual(id).C;
        ymax = max(max(C(:,T.allTis)));
        ymin = ShowGraphics.lowerLimitY;;
        for tis = T.allTis
            plot(t/60,C(:,tis),style.(T.name{tis}),'LineWidth',1.3);
        end;
        xlabel('t[h]'); ylabel(sprintf('C_{tis} [mg/l]'));
        xlim([0, t(end)/60]); ylim([ymin ymax]);
        title(sprintf('individual(%d), drug %s',id,drug.name),'Fontsize',15);
        set(gca,'YScale',ShowGraphics.allTissues.YScale); hold off;
        
        %%% plot all normalized concentration-time profiles in a single plot
        subplot(1,2,2); hold on;
        CdivKtb = C(:,T.allTis)*diag(1./individual(id).nKtb);
        ymax = max(max(CdivKtb));
        ymin = ShowGraphics.lowerLimitY;;
        for tis = T.allTis
            plot(t/60,CdivKtb(:,tis),style.(T.name{tis}),'LineWidth',1.3);
        end;
        xlabel('t[h]'); ylabel(sprintf('C_{tis}/K_{tis} [mg/l]'));
        xlim([0, t(end)/60]); ylim([ymin ymax]);
        set(gca,'YScale',ShowGraphics.allTissues.YScale);
        title(sprintf('Normalized drug concentration'),'Fontsize',15); hold off;
        legend(T.name(T.allTis),'Location','SouthEast');
        figNr = figNr + 1;
        
    end;

end;


%%% Plot percentiles for venous blood concentrations for entire population
%%% Only applicable for population of size>=10 
%%%
population.size = length(individual);
if strcmp(ShowGraphics.percentiles.plasma,'yes') && (population.size>=10)

    figure(ShowGraphics.percentiles.figNr); clf; 
    
    subplot(1,2,1);
    plot(population.BW,population.BH,'o');
    xlabel('BW'); ylabel('BH');
    
    if length(study.observationTime)==2
        fprintf('\n\n --> study.observationTime only specified initial and final time point.');
        fprintf('\n For this option to work you have to specify a vector of observation times.');
        fprintf('\n Suggestion: Change study.observationTime = [0 study.Tend] to ');
        fprintf('\n study.observationTime = [0:dt:study.Tend] where you replace dt with, e.g, 1'),
        fprintf('\n corresponding to observations every minute\n\n'),
        error(' DO IT!');
    end;
    
    subplot(1,2,2);
    tspan = individual(1).t/60;
    if length(tspan)<5
        fprintf(' Only %d time points specified in study.observationTime! Please increase number for more meaningful graphical representation!');
        beep;
    end;
        
    ind = 2:length(tspan);
    Cven_vs_t = NaN*ones(population.size,length(individual(1).t(ind)));
    for id=1:length(individual) 
        Cven_vs_t(id,:) = individual(id).C(ind,T.ven)';  
    end;
    percentile = prctile(Cven_vs_t,[5 25 50 75 95])';
    hold on; 
    h = area(tspan(ind),[percentile(:,1)'; diff(percentile,1,2)']');
    set(get(h(1),'Children'),'FaceColor',[1 1 1],'LineStyle','none');
    set(get(h(2),'Children'),'FaceColor',[.9 .9 .9],'LineStyle','none');
    set(get(h(3),'Children'),'FaceColor',[.8 .8 .8],'LineStyle','none');
    set(get(h(4),'Children'),'FaceColor',[.8 .8 .8],'LineStyle','none');
    set(get(h(5),'Children'),'FaceColor',[.9 .9 .9],'LineStyle','none');
    plot(tspan(ind),percentile(:,[1 5]),':k',tspan(ind),percentile(:,[2 4]),'--k',tspan(ind),percentile(:,3),'-k'); 
    set(gca,'Layer','top');
    
    xlabel('t[h]'); ylabel('C [mg/l] in ven');
    title(sprintf('Compound = %s',drug.name));
    xlim([0, tspan(end)]); 
    ymin = ShowGraphics.lowerLimitY;
    ymax = max(max(Cven_vs_t));
    grid on; ylim([ymin ymax]);
    set(gca,'YScale',ShowGraphics.percentiles.YScale);    


end;

fprintf('\n');


