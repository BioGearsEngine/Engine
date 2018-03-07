%%%
%%% Version: August 31st, 2012
%%% 
%%% Check at URL: http://www.pharmacometrics.de
%%% for potential updates and comments
%%%
%%% This function simulates the generic 13-compartment PBPK model
%%% for some individual, a given drug and study design.
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

function [sim,OK] = GenericPBPKmodel_simulatePBPKmodel(individual,drug,study)


if individual.id==1
   fprintf('\n   Simulating PBPK model ...\n');
   fprintf('\n   Individual(id): age; sex; height; weight; line type in output \n');
end;
   fprintf('\n   Individual(%2.0f):  %2.0f; %s; %1.2f m; %2.1f kg; ''%s''; ',...
   individual.id,ageInYears(individual.age),individual.sex(1),individual.BH,individual.BW,char(individual.color));

%%% check for consistency of all PBPK parameters
OK = GenericPBPKmodel_concistencycheck(individual,drug,study);
if ~OK return; end;
    
%%% Set initial values for ODE
T     = individual.tissue;
C_tis = zeros(1,T.numberOfTis); A_GItract = 0; A_metab = 0;
sim.t = []; sim.C = []; sim.A_GItract = []; sim.A_metab = [];
Tend = max(study.observationTime);

for d=1:study.numberOfDosing
    
    C_tis(end,T.ven) = C_tis(end,T.ven) + individual.bolus.dose/individual.V(T.ven);
    A_GItract(end)   = A_GItract(end) + (1-individual.E.gut)*(1-individual.E.feces)*individual.po.dose;

    X0 = [C_tis(end,:), A_GItract(end), A_metab(end)];

    %%% Simulate system of ODEs
    [t,X] = ode15s(@GenericPBPKmodel_RHS,study.observationTime,X0',[],individual,study);
    
    t = t + (d-1)*Tend; % transformation from relative time to absolute time 
    if d~=study.numberOfDosing
        t(end)  = t(end) - 1e-10; % modify time to account for trough value measurement
    end;
    C_tis     = X(:,1:T.numberOfTis);
    A_GItract = X(:,T.numberOfTis+1);
    A_metab   = X(:,T.numberOfTis+2);
    
    sim.t   = [sim.t;t];
    sim.C   = [sim.C;C_tis];
    sim.A_GItract = [sim.A_GItract;A_GItract];
    sim.A_metab   = [sim.A_metab;A_metab];

end;

sim.A = sim.C*diag(individual.V); % amounts
sim.unit.t = 'min'; sim.unit.C = 'mg/L'; sim.unit.A = 'mg';


end

% ------------------------------------------------------------------------


function out = GenericPBPKmodel_RHS(t,C,individual,study)

T = individual.tissue; % individual related indexing

% tissue volumes, blood flows, extraction ratios, clearance etc.
V     = individual.V;
Q     = individual.Q; 
Ktb   = individual.Ktb;
CLint = individual.CLint; 

% GItract
A_GItract = C(T.numberOfTis+1);
lambda_po = individual.lambda_po; 

% venous concentration leaving tissue
C_vbl = zeros(T.numberOfTis,1); 
C_vbl(T.allTisExBlo) = C(T.allTisExBlo)./Ktb(T.allTisExBlo)';

% dosing
r_iv_infusion = 0;
if (study.infusion.tend > 0) && (t<=study.infusion.tend)
    r_iv_infusion = individual.infusion.dose/study.infusion.tend;
end;

% concentration entering organs/tissues
Cin_ven = (Q(T.intoVen)*C_vbl(T.intoVen))/Q(T.ven);

Qliv_art = Q(T.liv)-Q(T.spl)-Q(T.gut);
Cin_liv = (Qliv_art*C(T.art)+Q(T.gut)*C_vbl(T.gut)+Q(T.spl)*C_vbl(T.spl))/Q(T.liv);

% rates of change for the tissue/organ concentrations
VdC = zeros(T.numberOfTis,1); 

% lung
VdC(T.lun) = Q(T.lun)*(C(T.ven)-C_vbl(T.lun));
% artery
VdC(T.art) = Q(T.art)*(C_vbl(T.lun)-C(T.art));
% brain
VdC(T.bra) = Q(T.bra)*(C(T.art)-C_vbl(T.bra));
% adipose
VdC(T.adi) = Q(T.adi)*(C(T.art)-C_vbl(T.adi));
% heart
VdC(T.hea) = Q(T.hea)*(C(T.art)-C_vbl(T.hea));
% kidneys
VdC(T.kid) = Q(T.kid)*(C(T.art)-C_vbl(T.kid));
% muscle
VdC(T.mus) = Q(T.mus)*(C(T.art)-C_vbl(T.mus));
% bone
VdC(T.bon) = Q(T.bon)*(C(T.art)-C_vbl(T.bon));
% skin
VdC(T.ski) = Q(T.ski)*(C(T.art)-C_vbl(T.ski));
% gut
VdC(T.gut) = Q(T.gut)*(C(T.art)-C_vbl(T.gut));
% spleen
VdC(T.spl) = Q(T.spl)*(C(T.art)-C_vbl(T.spl));
% liver
VdC(T.liv) = Q(T.liv)*(Cin_liv-C_vbl(T.liv)) - CLint.liv*C(T.liv) + lambda_po*A_GItract;
% vein
VdC(T.ven) = Q(T.ven)*(Cin_ven-C(T.ven)) + r_iv_infusion;

% converting amounts to concentrations
dC = VdC./V';

% drug amount in GI tract for absorption
dA_GItract = -lambda_po*A_GItract;

% metabolized and excreted compound (mass)
dA_metab = CLint.liv*C(T.liv);

% output vector
out = [dC;dA_GItract;dA_metab];

end


% ------------------------------------------------------------------------

function OK = GenericPBPKmodel_concistencycheck(individual,drug,study)

T = individual.tissue; OK = 1;

if sum(individual.V<0)
    OK = warning('MATLAB:genericPBPKmodel','\n\n ---> There are negative values in individual.V <---- \n\n');
end;

if sum(individual.Q<0)
    OK = warning('MATLAB:genericPBPKmodel','\n\n ---> There are negative values in individual.Q <---- \n\n');
end;

if sum(isnan(individual.Ktup(T.allTisExBlo)))
    OK = warning('MATLAB:genericPBPKmodel','\n\n ---> There are undefined values in individual.Ktup(T.allTisExBlo) <---- \n\n');
end;

if individual.BP<(1-individual.hct)
    OK = warning('MATLAB:genericPBPKmodel','\n\n ---> Unphysiological condition: BP < (1-hct) <---- \n\n');
end;

if individual.Keup<0
    OK = warning('MATLAB:genericPBPKmodel','\n\n ---> Erythrocyte-to-unbound plasma partition coefficient <=0 <---- \n\n');
end;

if sum(individual.Ktup(T.allTisExBlo)<0)
    OK = warning('MATLAB:genericPBPKmodel','\n\n ---> There are negative values in drug.Ktup.pred <---- \n\n');
end;

if sum(isnan(individual.Ktb(T.allTisExBlo)))>0
    OK = warning('MATLAB:genericPBPKmodel','\n\n ---> There are undefined values in drug.Ktb.pred <---- \n\n');
end;

if sum(individual.Ktb(T.allTisExBlo)<0)
    OK = warning('MATLAB:genericPBPKmodel','\n\n ---> There are negative values in drug.Ktb.pred <---- \n\n');
end;

if isnan(individual.fuP)>0 || (individual.fuP<0)
    OK = warning('MATLAB:genericPBPKmodel','\n\n ---> drug.fuP either NaN or negative <---- \n\n');
end;

if isnan(individual.BP)>0 || (individual.BP<0)
    OK = warning('MATLAB:genericPBPKmodel','\n\n ---> drug.BP either NaN or negative <---- \n\n');
end;

if isnan(drug.logPow)>0 
    OK = warning('MATLAB:genericPBPKmodel','\n\n ---> drug.logPow value is NaN <---- \n\n');
end;

if ~strcmp(drug.mode,'N') && isnan(drug.pKa)
    OK = warning('MATLAB:genericPBPKmodel','\n\n ---> drug.pKa is NaN <---- \n\n');
end;

if (study.po.dose>0) && ( isnan(individual.lambda_po) || (individual.lambda_po<=0) )
    OK = warning('MATLAB:genericPBPKmodel','\n\n ----> drug.po.lambda either NaN or <=0 ! <---- \n\n');
end;

if sum(isnan(individual.V(T.allTis)<0))>0 | individual.V(T.allTis)<0
    OK = warning('MATLAB:genericPBPKmodel','\n\n ---> There are NaN or negative values in individual.V <---- \n\n');
end;

if individual.CLblood.hep>individual.Q(T.liv)
    OK = warning('MATLAB:genericPBPKmodel','\n\n ---> Unphysiological condition: CL_blood > Q_liv <---- \n\n');
end;

if abs(sum(individual.Q(T.intoVen))-individual.Q(T.ven))>1e-10
    OK = warning('MATLAB:genericPBPKmodel','\n\n ---> Sum of blood flows into ven differnt from Qven <---- \n\n');
end;

if (individual.bolus.dose==0) && (individual.infusion.dose==0) &&  (individual.po.dose==0)
    OK = warning('MATLAB:genericPBPKmodel','\n\n ---> No drug administerd (iv and po dose=0)! <---- \n\n');
end;

if (individual.bolus.dose<0) || (individual.infusion.dose<0) || (individual.po.dose<0)
    OK = warning('MATLAB:genericPBPKmodel','\n\n ---> Negative drug dose administerd ! <---- \n\n');
end;

if individual.infusion.tend > max(study.observationTime)
    OK = warning('MATLAB:genericPBPKmodel','\n\n ---> Infusion time larger than observation time ! <---- \n\n');
end;

if (individual.E.feces<0) || (individual.E.feces>1)
    OK = warning('MATLAB:genericPBPKmodel','\n\n ---> Violation of 0<=E.feces<=1 ! <---- \n\n');
end;

if (individual.E.gut<0) || (individual.E.gut>1)
    OK = warning('MATLAB:genericPBPKmodel','\n\n ---> Violation of 0<=E.gut<=1 ! <---- \n\n');
end;

if (individual.E.ren~=0)
    OK = warning('MATLAB:genericPBPKmodel','\n\n ---> Renal clearnance not implemented ! <---- \n\n');
end;


end


% ------------------------------------------------------------------------

function y = ageInYears(ageClass)

a = char(ageClass);

switch a    
    case 'age5'; 
        y = 5;
    case 'age10'; 
        y = 10;
    case 'age15m'; 
        y = 15;
    case 'age15f'; 
        y = 15;
    case 'age35m'; 
        y = 35;
    case 'age35f'; 
        y = 35;
    otherwise
        error('Unknown ageclass in GenericPBPKmodel_simulatePBPKmodel!');
end;

end

