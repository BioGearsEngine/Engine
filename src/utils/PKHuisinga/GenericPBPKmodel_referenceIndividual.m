%%%
%%% Version: August 31st, 2012
%%% 
%%% Check at URL: http://www.pharmacometrics.de
%%% for potential updates and comments
%%%
%%% This function assigns all parameter values to all reference individuals
%%% 'age5','age10','age15m','age15f','age35m','age35f'.
%%% 
%%% Called from the main script GenericPBPKmodel.m
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

function [reference] = GenericPBPKmodel_referenceIndividual(species,drug,study)

fprintf('Parameterizing reference individuals. ')

%%% determine erythrocyte-to-unbound plasma partition coefficient Keup and
%%% hepatic intrinsic clearance per kg liver volume (CLint_perKgLiver)
%%%
[CLint_perKg,Keup] = GenericPBPKmodel_CLintKeup(species,drug);

for a = species.AgeSexClasses
    
    age = char(a);
    T   = species.(age).tissue;
    reference.(age).unit        = species.(age).unit;
    reference.(age).type        = species.(age).type;
    reference.(age).tissue      = T;
    reference.(age).age         = a;
    reference.(age).sex         = species.(age).sex;
    reference.(age).BH          = species.(age).BH;
    reference.(age).BW          = species.(age).BW;
    reference.(age).BMI         = species.(age).BW/species.(age).BH^2;
    reference.(age).V           = species.(age).V(T.allTis);
    reference.(age).hct         = species.(age).hct;
    reference.(age).fVtis       = species.(age).fVtis;
    reference.(age).r           = species.(age).r;
    reference.(age).SV.adi      = 1;
    reference.(age).SV.ski      = 1;
    reference.(age).SV.tis      = 1;
    reference.(age).BSA         = species.(age).BSA;
    reference.(age).LBW         = species.(age).LBW;
    reference.(age).co          = species.(age).co;
    reference.(age).Q           = species.(age).Q(T.allTis);
    reference.(age).SQ          = 1;
    reference.(age).CLint_perKg.liv = CLint_perKg.liv; reference.(age).unit.CLint_perKg = 'L/min/kg OW';
    reference.(age).CLint.liv       = CLint_perKg.liv*reference.(age).V(T.liv); reference.(age).unit.CLint = 'L/min';
    reference.(age).CLint_perKg.kid = CLint_perKg.kid; 
    reference.(age).CLint.kid       = CLint_perKg.kid*reference.(age).V(T.kid);
    reference.(age).Keup        = Keup;  reference.(age).unit.Keup = 'unitless';
    reference.(age).fuP         = drug.(species.type).fuP; reference.(age).unit.fuP = drug.unit.fuP;
    reference.(age).BP          = reference.(age).hct*Keup*reference.(age).fuP + (1-reference.(age).hct); reference.(age).unit.BP = drug.unit.BP;
    
    [Ktup,Ktb]                  = GenericPBPKmodel_Ktb(reference.(age),drug);
    reference.(age).Ktup        = Ktup(T.allTis); reference.(age).unit.Ktup = 'unitless';
    reference.(age).Ktb         = Ktb(T.allTis); reference.(age).unit.Ktb = 'unitless';
    
    reference.(age).lambda_po   = drug.(species.type).lambda_po;
    [CLblood,E,nKtb,Vss,t_half]=  GenericPBPKmodel_CLblood_etc(reference.(age),drug.(species.type).E);
    
    reference.(age).CLblood     = CLblood; reference.(age).unit.CLblood = 'L/min';
    reference.(age).E           = E; reference.(age).unit.E = 'fraction'; 
    reference.(age).nKtb        = nKtb(T.allTis); reference.(age).unit.nKtb = 'unitless';
    reference.(age).t_half      = t_half(T.allTis); reference.(age).unit.t_half = 'min';
    reference.(age).Vss         = Vss; reference.(age).unit.Vss = 'L';
    reference.(age).VssAdi      = reference.(age).nKtb(T.adi)*reference.(age).V(T.adi);
    reference.(age).VssLBW      = Vss-reference.(age).VssAdi;
    reference.(age).Vres        = species.(age).BW - sum(species.(age).V([T.adi T.bra T.ski]));
    reference.(age).V1          = sum(reference.(age).V(T.V1).*reference.(age).nKtb((T.V1)));
    reference.(age).V2          = sum(reference.(age).V(T.V2).*reference.(age).nKtb((T.V2)));
    reference.(age).Q12         = sum(reference.(age).Q(T.V2));

    reference.(age).id          = a;
    reference.(age).color       = 'b';
    
    for d = {'po','bolus','infusion'}
        
        route = char(d);
        reference.(age).(route) = study.(route);
        if ~strcmp(study.(route).per,'fixed')
            BSD  = study.(route).per;
            reference.(age).(route).dose = study.(route).dose*reference.(age).(BSD);
        end;
        
    end;
    reference.(age).lambda_po = drug.(reference.(age).type).lambda_po;
    
end;



% ========================================================================
function out = BSA(BW,BH)

% Determine body surface area (BSA) in [m^2]
% Source: Mosteller, New Engl J Med, Vol 317, 1987

out = sqrt(BH*BW/36);


% ========================================================================
function [CLint_perKg,Keup] = GenericPBPKmodel_CLintKeup(species,drug)

%%% Experimental values of hepatic and renal blood clearance are assumed 
%%% to correspond to the age = drug.(species.type).expData.age.
%%% Here, these values are converted to age-independent data
%%% CLint_kgLiv and CLren_kgKid
%%%

% age to which experimental values of hct and CLblood correspond
age  = drug.(species.type).expData.age;

% determine erythrocyte-to-unbound plasma partition coefficient Keup
hct  = species.(age).hct;
BP   = drug.(species.type).BP;
fuP  = drug.(species.type).fuP;

Keup = (BP-(1-hct))/(hct*fuP);

T    = species.(age).tissue;
species.(age).fuP = drug.(species.type).fuP;
species.(age).BP  = drug.(species.type).BP;
species.(age).Keup = Keup;
[Ktup,Ktb]  = GenericPBPKmodel_Ktb(species.(age),drug);

% determine hepatic intrinsic clearance per kg liver volume
% based on well-stirred liver model
CLblood.hep  = drug.(species.type).CLblood_kgBW.hep * species.(age).BW;
Q_liv        = species.(age).Q(T.liv);
K_liv        = Ktb(T.liv);

if CLblood.hep>Q_liv error(' CLblood.hep larger than Q_liv!'); end;

CLint.liv       = Q_liv*CLblood.hep/(K_liv*(Q_liv-CLblood.hep));
CLint_perKg.liv = CLint.liv/species.(age).V(T.liv);

% determine renal clearance per kg kidney volume (based on well-stirred
% tissue model and QSSA)
if drug.(species.type).CLblood_kgBW.ren > 0
    error('Renal clearance not implemented !');
end;
% Below is just a DUMMY scaling that is NOT used and needs to be 
% verified before eventually using it. Most likely, it is not good,
% so don't use it!
CLblood.ren  = drug.(species.type).CLblood_kgBW.ren * species.(age).BW;
Q_kid        = species.(age).Q(T.kid);
K_kid        = Ktb(T.kid);

if CLblood.ren>Q_kid error(' CLblood.kid larger than Q_kid!'); end;

CLint.kid       = Q_kid*CLblood.ren/(K_kid*(Q_kid-CLblood.ren));
CLint_perKg.kid = CLint.kid/species.(age).V(T.kid);


% ========================================================================
function [Ktup,Ktb] = GenericPBPKmodel_Ktb(individual,drug)

%%% Uses method published by Rodgers and Rowland to determine 
%%% tissue-to-unbound plasma partition coefficients. 


fVtis = individual.fVtis; % volume fractions of interstitial and cellular space
Vr    = individual.r;     % tissue-to-plasma ratios of binding proteins
hct   = individual.hct;   % hematocrite
fuP   = individual.fuP;    % fraction unbound in plasma
BP    = individual.BP;    % blood-to-plasma ratio

T = individual.tissue;

if (isnan(drug.logPow))
    error('Octanol-to-water partition coefficient undefined!');
end
% estimate logPvow based on logPow according to Poulin & Theil
if (isnan(drug.logPvow))
    drug.logPvow =1.115*drug.logPow-1.35;
end
 
KA_TPR  = zeros(size(T.initialize));

% ionization effects
pH_p = 7.4; pH_iw = 7.0; pH_e  = 7.22;
fnC  = GenericPBPKmodel_ionization(drug,pH_iw);
fnE  = GenericPBPKmodel_ionization(drug,pH_e);
fnP  = GenericPBPKmodel_ionization(drug,pH_p);

% erythrocytes-to-unbound plasma partition coefficient
Keup = (BP-(1-hct))/(hct*fuP);

% neutral lipids-to-water partition coefficient
Pow  = 10^drug.logPow;
Pvow = 10^drug.logPvow;
Knlw = Pow*ones(size(T.initialize));
Knlw(T.adi) = Pvow;

% neutral phospholipids-to-water partition coefficient
Knpw = 0.3*Knlw+0.7;

switch drug.mode
    case {'B','AB'} % binding to acidic phospholipids (APmt)
        KA_TPR = zeros(size(T.initialize));
        KA_AP  = (Keup - fnP/fnE*fVtis.wic(T.ery) - fnP*Pow*fVtis.nlt(T.ery)...
            - fnP*(0.3*Pow+0.7)*fVtis.npt(T.ery)) * fnE/((1-fnE)*fnP)/fVtis.APmt(T.ery);
    case {'N'}      % binding to lipoproteins (Ltp)
        KA_TPR = (1/fuP -1 -fnP*Pow*fVtis.nlt(T.pla) ...
            -fnP*(0.3*Pow+0.7)*fVtis.npt(T.pla))*Vr.Ltp;
        KA_AP  = 0;
    case {'A','wB'} % binding to albumin (Atp)
        KA_TPR = (1/fuP -1 -fnP*Pow*fVtis.nlt(T.pla) ...
            -fnP*(0.3*Pow+0.7)*fVtis.npt(T.pla))*Vr.Atp;
        KA_AP  = 0;
    otherwise
        error(' Unknown type of compound to predict Ktup')
end

Ktup = T.initialize;
m    = T.allTisExBlo;
Ktup(m) = fVtis.wex(m) +fnP/fnC*fVtis.wic(m) + fnP*Knlw(m).*fVtis.nlt(m) ...
          + fnP*Knpw(m).*fVtis.npt(m) + fnP*(1-fnC)/fnC*KA_AP*fVtis.APmt(m) + KA_TPR(m);
               
Ktb = Ktup*fuP/BP;



% ========================================================================
function [CLblood,E,nKtb,Vss,t_half] = GenericPBPKmodel_CLblood_etc(individual,E)

%%% Determines blood clearance values, extraction ratios, normalized partition
%%% coefficients, volume of distribution and half life. 
%%%

hct   = individual.hct;               % hematocrite
fuP   = individual.fuP;               % fraction unbound in plasma
BP    = individual.BP;                % blood-to-plasma ratio
Ktb   = individual.Ktb;               % tissue-to-blood partition coefficients
T     = individual.tissue;

% hepatic clearance
Q_liv = individual.Q(T.liv);
K_liv = individual.Ktb(T.liv);
CLint.liv   = individual.CLint_perKg.liv * individual.V(T.liv);
E.hep       = CLint.liv*K_liv/(Q_liv + CLint.liv*K_liv);
CLblood.hep = Q_liv * E.hep;

% renal clearance
Q_kid = individual.Q(T.kid);
K_kid = individual.Ktb(T.kid);
CLint.kid   = individual.CLint_perKg.kid * individual.V(T.kid);
E.ren       = CLint.kid*K_kid/(Q_kid + CLint.kid*K_kid);
CLblood.ren = Q_kid * E.ren;


% extraction ratio normalized tissue-to-blood partition coefficients (nKtb)
nKtb        = Ktb;
nKtb(T.liv) = (1-E.hep)*Ktb(T.liv);
nKtb(T.kid) = (1-E.ren)*Ktb(T.kid);
nKtb(T.art) = 1; 
nKtb(T.ven) = 1;

% predict volume of distribution at steady state
m  = T.allTis;
Vss = sum(nKtb(m).*individual.V(m));

% predicted tissue distribution half-life
t_half    = T.initialize;
m         = T.allTis;
t_half(m) = log(2)*individual.V(m).*nKtb(m)./individual.Q(m);



% ========================================================================

function fn = GenericPBPKmodel_ionization(drug,pH)

% pka  = pka values (vector) in decreasing order
% pH   = pH value of the environment
% mode = 'A' for acid; 'B' for base; 'N' for neutral;
%        'AA' for diprotonic acid; 
%        'BB' for diprotonic base; 'AB' for zwitterions

switch drug.mode
    case 'A' % acid
        fn = 1/(1+10^(pH-drug.pKa(1)));
    case {'B','wB'} % base
        fn = 1/(1+10^-(pH-drug.pKa(1)));
    case 'N' % neutral
        fn = 1;
    case 'AA'% di-acid
        fn = 1/(1+10^(pH-drug.pKa(1))+10^(2*pH-drug.pKa(1)-drug.pKa(2)));   
    case 'BB'% di-base
        fn = 1/(1+10^-(pH-drug.pKa(2))+ 10^-(2*pH-drug.pKa(1)-drug.pKa(2)));
    case 'AB'% zwitter
        fn = 1/(1+10^(pH-drug.pKa(2))+ 10^-(pH-drug.pKa(1)));
end




