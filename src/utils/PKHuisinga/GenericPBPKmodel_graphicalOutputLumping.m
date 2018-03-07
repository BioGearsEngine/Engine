%%%
%%% Version: August 31st, 2012
%%% 
%%% Check at URL: http://www.pharmacometrics.de
%%% for potential updates and comments
%%%
%%% This function plots the diverse graphics to illustrate the simulation
%%% results for the lumped model.
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

function out = GenericPBPKmodel_graphicalOutputLumping(individual,L,drug,ShowGraphics)

if isempty(L) return; end;


%%% --- graphical output -------------------------------------------------

figNr = ShowGraphics.lumping.figNr;
for id=ShowGraphics.lumping.individual
    
    
    % all tissues except for vein
    figure(figNr); clf; hold on;
    T = individual(id).tissue;
    ymax = max(max(individual(id).C(:,T.allTisExVen)));
    ymin = ShowGraphics.lowerLimitY;
    
    for n=1:12
        subplot(3,4,n); hold on;
        tis = T.allTisAlphabetic(n);
        plot(individual(id).t/60,individual(id).C(:,tis),'g',L(id).t/60,L(id).C_fullmodel(:,tis),'k--');
        xlabel('t[h]'); ylabel(sprintf('C_{%s} [mg/l]',T.name{tis}));
        xlim([0, L(id).t(end)/60]); ylim([ymin ymax]);
        set(gca,'YScale',ShowGraphics.allTissues.YScale);
    end;
    subplot(3,4,2); title(sprintf('individual %d, drug %s, %s',id,drug.name,ShowGraphics.lumping.type));
    set(gca,'YScale',ShowGraphics.lumping.YScale);
    
    % vein only
    figure(figNr+1); clf; hold on;
    plot(individual(id).t/60,individual(id).C(:,T.ven),'g',L(id).t/60,L(id).C_fullmodel(:,T.ven),'k--');
    xlabel('t[h]'); ylabel('C [mg/l] in ven');
    title(sprintf('individual %d, drug %s, %s',id,drug.name,ShowGraphics.lumping.type));
    xlim([0, L(id).t(end)/60]); ylim([ymin ymax]);
    set(gca,'YScale',ShowGraphics.lumping.YScale);
    figNr = figNr + 2;

end; 

