%%%
%%% Version: August 31st, 2012
%%% 
%%% Check at URL: http://www.pharmacometrics.de
%%% for potential updates and comments
%%%
%%% This function determines all parameters for a lumped compartment model
%%% obtained from the 13-compartment PBPK model and simulates it.
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

function L = GenericPBPKmodel_mechanisticLumping(individual,drug,study,lumping)


fprintf('\n   Compute lumping for id = %d and simulate it. ',individual.id)

T     = individual.tissue;

L.nKtb = individual.nKtb;
t      = individual.t; %Tend = t(end);
nC     = individual.C(:,T.allTis)*diag(1./L.nKtb);

L.lumping  = lumping;
L.noOrgans = length(L.lumping);


%%% --- determine lumped compartments containing ven, liv and kid 
for n=1:L.noOrgans
    if ismember(T.ven,L.lumping{n}) L.cen = n; end;
    if ismember(T.liv,L.lumping{n}) L.Liv = n; end;
    if ismember(T.kid,L.lumping{n}) L.Kid = n; end;
end;
L.not_cen = 1:L.noOrgans; L.not_cen(L.cen) = [];

%%% --- determine lumped parameters
L.V   = zeros(L.noOrgans,1); 
L.Q   = zeros(L.noOrgans,1); 
L.Ktb = zeros(L.noOrgans,1); 
for n=1:length(L.lumping)
    ind      = L.lumping{n};
    L.V(n)   = sum(individual.V(ind)); 
    L.Q(n)   = sum(individual.Q(ind));
    L.Ktb(n) = sum(individual.V(ind).*L.nKtb(ind))/L.V(n);
end;
L.Q(L.cen) = sum(L.Q(L.not_cen));

L.CL.hep = individual.CLblood.hep;
L.CL.ren = individual.CLblood.ren;

L.bolus    = individual.bolus;
L.infusion = individual.infusion;
L.po       = individual.po;

%%% --- simulate lumped model
C0 = zeros(L.noOrgans+1,1); 
C0(L.cen) = L.bolus.dose/L.V(L.cen);
[L.t,L.C] = ode15s(@ODE_RHS,t,C0,[],individual,study,L);


C_tis = zeros(1,L.noOrgans); A_metab = 0;
L.t = []; L.C = []; L.A_metab = [];

Tend = max(study.observationTime);
for d=1:study.numberOfDosing
    
    C_tis(end,L.cen) = C_tis(end,L.cen)+individual.bolus.dose/L.V(L.cen);
    X0 = [C_tis(end,:), A_metab(end)];

    %%% Simulate system of ODEs
    [t,X] = ode15s(@ODE_RHS,study.observationTime,X0',[],individual,study,L);
    
    t = t + (d-1)*Tend; % transformation from relative time to absolute time 
    if d~=study.numberOfDosing
        t(end)  = t(end) - 1e-10; % modify time to account for trough value measurement
    end;
    C_tis   = X(:,1:L.noOrgans);
    A_metab = X(:,L.noOrgans+1);
    
    L.t   = [L.t;t];
    L.C   = [L.C;C_tis];
    L.A_metab = [L.A_metab;A_metab];

end;


%%% "unlump" concentrations to original set of tissues
[L.C_fullmodel,L.Atot_fullmodel] = map2detailedModel(L,individual);


%%% --- output tissues that are lumped together
fprintf('\n    -> %s: lumped %d-cmp model: ',drug.name,L.noOrgans);
for k=1:L.noOrgans
    fprintf('{');
    for m=1:length(L.lumping{k})
        fprintf('%s,',char(T.name(L.lumping{k}(m))));
    end;
    fprintf('\b},');
end;
fprintf('\b\n');



% ==================================================================
function [C_fullmodel,Atot_fullmodel] = map2detailedModel(L,individual)

% compute predicted original tissue and organ concentrations
T = individual.tissue;
C_fullmodel = zeros(size(L.C(:,1:L.noOrgans),1),T.numberOfTis+1);

for m = L.noOrgans:-1:1
    for tis = L.lumping{m}
        C_fullmodel(:,tis) = L.C(:,m)/L.Ktb(m)*L.nKtb(tis);
    end;
end;

Atot_fullmodel = sum(C_fullmodel(:,T.allTis)*diag(individual.V(T.allTis)),2);



% ==================================================================
function out = ODE_RHS(t,C,individual,study,L)

T      = individual.tissue;
Lmetab = L.noOrgans+1;
cen = L.cen; not_cen = L.not_cen; Liv = L.Liv; Kid = L.Kid;
CL = L.CL;
E  = individual.E;
lambda_po = individual.lambda_po;

% oral dosing
r_po = (1-E.gut)*(1-E.feces)*lambda_po*exp(-t*lambda_po)*(individual.po.dose);
if cen==Liv
    r_po = (1-E.hep)*r_po;
end;

% i.v. infusion
r_iv_infusion = 0;
if (study.infusion.tend > 0) && (t<=study.infusion.tend)
    r_iv_infusion = (individual.infusion.dose)/study.infusion.tend;
end;

% ODEs 
VdC = zeros(L.noOrgans,1);
C_blood = C(cen)/L.Ktb(cen);
if L.noOrgans>1
    Cin_cen      = sum(L.Q(not_cen).*C(not_cen)./L.Ktb(not_cen)) / L.Q(cen);
    
    VdC(not_cen) = L.Q(not_cen).*(C_blood - C(not_cen)./L.Ktb(not_cen));
    VdC(cen)     = L.Q(cen)*(Cin_cen - C_blood)  - CL.hep*C(Liv)/L.Ktb(Liv) -CL.ren*C(Kid)/L.Ktb(Kid) + r_iv_infusion; 
else
    VdC(cen)     = -CL.hep*C(Liv)/L.Ktb(Liv) -CL.ren*C(Kid)/L.Ktb(Kid) + r_iv_infusion; 
end;
VdC(Liv) = VdC(Liv) + r_po;

% converting amounts to concentrations
dC = VdC./L.V;

% metabolized and excreted compound (mass)
dA_metab = CL.hep*C(Liv)/L.Ktb(Liv) + CL.ren*C(Kid)/L.Ktb(Kid);

% output vector
out = [dC;dA_metab];



