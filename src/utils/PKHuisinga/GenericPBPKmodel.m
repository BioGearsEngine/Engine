%%% MAIN SCRIPT TO CONTROL THE SIMULATION
%%%
%%% Version: August 31st, 2012. 
%%% 
%%% Check at URL: http://www.pharmacometrics.de
%%% for potential updates and comments.
%%%
%%% This script specifies all inputs for the simulation of the 
%%% pharmacokinetics of different drugs in a virtual population of 
%%% human adults and children.
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


clear all;

%%% =================================================================
%%% The structure 'species' contains all the reference anatomical and
%%% physiological values that are used to generate individuals and
%%% compute tissue partition coefficients.
%%% species.name = 'human' ('rat' only partially implemented)

species.type = 'human';
[species,T]  = GenericPBPKmodel_physiology(species.type);


%%% =================================================================
%%% The structure 'drug' contains all physico-chemical drug properties
%%% as well as human and rat related PK parameters.
%%% Choose a drug from the list below, classified by its type: 
%%% moderate-to-strong base, weak base, neutral, acid. 
%%% The drug type determines how the tissue partition coefficients
%%% are computed. 
%%%
%%% Moderate-to-strong bases: Amitriptyline, Caffeine, Desipramine, 
%%% Diltiazem, Diphenhydramine, Imipramine, Lidocaine, Metoprolol, 
%%% Pindolol, Sildefanil, Theophylline
%%%
%%% Weak bases: Alprazolam, Diazepam, Flunitrazepam, Midazolam, Triazolam
%%%
%%% Acids: Amobarbital, Diclofenac, Hexobarbital, Ibuprofen, Methohexital, 
%%% Thiopental, Tolbutamide, Valproate, Warfarin

drug.name = 'Midazolam'; 
drug      = GenericPBPKmodel_drugDatabase(drug.name,species);


%%% =================================================================
%%% The structure 'study' contains information about the dose and
%%% the route of application (iv bolus, iv infusion, po), the
%%% simulation time in [min], the output time points, etc.
%%% Dosing can be fixed in unit [mg] or per body size descriptor (BSD)  
%%% in unit [mg/per unit of the BSD], e.g., [mg/kg BW]
%%% Choose dosing per BSD = 'fixed' (fixed dose),'BW','LBW','BH','BMI','BSA', etc

study.bolus.dose       = 5;   
study.bolus.per        = 'fixed'; 

study.infusion.dose    = 0;  
study.infusion.per     = 'fixed'; 
study.infusion.tend    = 60; 

study.po.dose          = 0; 
study.po.per           = 'fixed';  

study.observationTime  = [0 :1: 10*60]; % start and end time in [min]
study.numberOfDosing   = 1;            % for a single dose


%%% =================================================================
%%% Determine all parameters for the reference individuals:
%%% adults 35y (20-50y, m/f), and children 15y (m/f), 10y (u), 5y (u)
%%% with the last two being unisex (u)

[reference] = GenericPBPKmodel_referenceIndividual(species,drug,study);


%%% =================================================================
%%% Generate virtual population based on antropometric data.
%%% Approaches for generating the population:
%%%
%%%   male          = adult male reference individual
%%%   adults        = adult m/f reference individuals
%%%   all           = adult m/f, age15 m/f, age10, age5 
%%%   Grandmaison   = 355 male/329 female adults, with distribtion of BW & BH 
%%%                   as in the autopsy study by de la Grandmaison et al (2001)
%%%   random        = 'size' many individuals of 'agesex' with mean BW & BH
%%%                   identical to reference.agesex and CV of BH & BMI 
%%%                   as specified in de la Grandmaison et al (2001)
%%%   obese         = four individuals with BH identical to 'agesex' and
%%%                   BMI = [0.7 1 1.3 2]*BMI of reference 'agesex'
%%%   identical     = 'size' many individuals of 'agesex', only reasonalbe 
%%%                   if combined with randomEffects

population.how2generate = 'adults';   % 'male','adults','random','identical','all','children_small_adults','delaGrandmaison','obese'
population.agesex       = 'age35m'; % only valid with 'random','identical' and 'obese' option
population.size         = 35;      % only valid with 'random' and 'identical' option
[individual,population] = GenericPBPKmodel_virtualPopulation(reference,study,population);


%%% =================================================================
%%% Include random effects on hepatic (intrinsic) clearance and/or
%%% tissue-to-blood partition coefficients. Parameters are uniformly
%%% perturbed by a randon factor '1+facX', where faxX = 0 corresponds 
%%% to no perturbation and facX>0 corresponds to a perturbation between
%%% 1/(1+facX)-fold and (1+facX)-fold of the original value

randomEffects.facX.CLint.hep = 0;
randomEffects.facX.Ktb       = 0;
[individual,population]      = GenericPBPKmodel_randomEffects(individual,study,randomEffects,population);


%%% =================================================================
%%% Simulate PBPK model for each individual
%%%

for id=1:population.size
   [sim(id),OK] = GenericPBPKmodel_simulatePBPKmodel(individual(id),drug,study);
   if OK~=1 return; end;
   individual(id).t = sim(id).t; individual(id).unit.t = sim(id).unit.t;
   individual(id).C = sim(id).C; individual(id).unit.C = sim(id).unit.C; 
end;
fprintf('\n\n')


%%% =================================================================
%%% Graphical output. Specify below, which plots to be shown
%%% 
ShowGraphics.allindividual.plasma  = 'yes'; % 'yes' or 'no'
ShowGraphics.allindividual.YScale  = 'log'; % 'log' or 'lin'
ShowGraphics.allindividual.figNr   = 13;

ShowGraphics.allTissues.individual = [];    % specify id of individuals, e.g. [1 2 7]; [] = 'No plot' 
ShowGraphics.allTissues.YScale     = 'log'; % 'log' or 'lin'
ShowGraphics.allTissues.figNr      = 10;
ShowGraphics.allTissues.oneFigOnly = 1;     % 1 = all individual in one figure, 0 = use different figures for different individual 

ShowGraphics.individual.allTissues = [];   % specify id of individual, e.g. [1]; [] = 'No plot' 
ShowGraphics.individual.figNr      = 21;

ShowGraphics.percentiles.plasma    = 'yes'; % applies only to a virtual population of size 10 or larger 
ShowGraphics.percentiles.YScale    = 'log';
ShowGraphics.percentiles.figNr     = 24;

ShowGraphics.lowerLimitY           = 0.01;  % sets the lower value of the Y axis
GenericPBPKmodel_graphicalOutput(population,individual,drug,study,ShowGraphics); 


%%% =================================================================
%%% Lumping of PBPK models
%%%
%%% (i) mechanistic lumping
%%%
lumping = {[T.mus,T.adi],[T.bon],[T.ski],[T.bra,T.hea,T.kid],[T.gut,T.spl,T.liv],[T.lun,T.art,T.ven]};

ShowGraphics.lumping.individual = []; % specify id of individual, e.g. [1]; [] = 'No plot' 
ShowGraphics.lumping.YScale     = 'log'; % 'log' or 'lin'
ShowGraphics.lumping.figNr      = 50;
ShowGraphics.lumping.type       = 'mechanistically lumped model';

for id=ShowGraphics.lumping.individual
   mecL(id) = GenericPBPKmodel_mechanisticLumping(individual(id),drug,study,lumping); 
end;
if ~exist('mecL') mecL = []; end;
GenericPBPKmodel_graphicalOutputLumping(individual,mecL,drug,ShowGraphics)


%%%
%%% (ii) minimal lumping
%%%
lumping = {[T.mus,T.adi,T.bon,T.ski],[T.bra,T.hea,T.kid,T.gut,T.spl,T.liv,T.lun,T.art,T.ven]};

ShowGraphics.lumping.individual = []; % specify id of individual, e.g. [1]; [] = 'No plot' 
ShowGraphics.lumping.YScale     = 'log'; % 'log' or 'lin'
ShowGraphics.lumping.figNr      = 60;
ShowGraphics.lumping.type       = 'minimal lumped model';

for id=ShowGraphics.lumping.individual % 1:population.size %
   minL(id) = GenericPBPKmodel_mechanisticLumping(individual(id),drug,study,lumping); 
end;
if ~exist('minL') minL = []; end;
GenericPBPKmodel_graphicalOutputLumping(individual,minL,drug,ShowGraphics)

fprintf('\n\n')

