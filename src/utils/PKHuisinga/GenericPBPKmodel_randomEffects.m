%%%
%%% Version: August 31st, 2012
%%% 
%%% Check at URL: http://www.pharmacometrics.de
%%% for potential updates and comments
%%%
%%% This function introduces random perturbations to the hepatic blood 
%%% clearance (via CLint) and the tissue-to-blood partition coefficients. 
%%% The size of the perturbation is specified by facX, so that the ratio
%%% of the perturbed-to-unperturbed value lies between 1/(1+facX) and
%%% (1+facX). 
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

function [individual,population] = GenericPBPKmodel_randomEffects(individual,study,randomEffects,population)


facX.CLint.hep = randomEffects.facX.CLint.hep;
facX.Ktb = randomEffects.facX.Ktb;

if facX.CLint.hep>0
    fprintf('\n   --> Note: Random effect for CLint.hep considered!\n');
elseif facX.CLint.hep<0
    error('Random effect CLint.hep is negative!');
end;
if facX.Ktb>0
    fprintf('\n   --> Note: Random effect for Ktb considered! \n');
elseif facX.Ktb<0
    error('Random effect Ktb is negative!');
end;

%%% scale all tissue of individual 'id' with same random factor (=1) or each tissue
%%% of individual 'id' with different randomly chosen factor (=0)
randomEffects.facX.Ktb_allTisIdentical = 0;


if facX.CLint.hep>0 || facX.Ktb>0
    for id=1:population.size

        age   = individual(id).age;
        T     = individual(id).tissue;
        R_hep = individual(id).E.hep/(1-individual(id).E.hep);
        Q_liv = individual(id).Q(T.liv);
        K_liv = individual(id).Ktb(T.liv);
        CLint.hep = R_hep*Q_liv/K_liv;
        

        if rand<0.5
            CLint.hep = CLint.hep*(1+facX.CLint.hep*rand); % maximum of factor 'facX' increase
        else
            CLint.hep = CLint.hep/(1+facX.CLint.hep*rand); % maximum of factor 'facX' decrease
        end;

        for m=T.allTisExBlo

            if randomEffects.facX.Ktb_allTisIdentical
                rand1 = rand(1)*ones(1,T.numberOfTis);
                rand2 = rand(1)*ones(1,T.numberOfTis);
            else
                rand1 = rand(1,T.numberOfTis);
                rand2 = rand(1,T.numberOfTis);

            end;

            Ktb = individual(id).Ktb(m);
            Ktb = [rand1<0.5].*Ktb.*(1+facX.Ktb*rand2) + [rand1>=0.5].*Ktb./(1+facX.Ktb*rand2);
            
            individual(id).Ktb(m) = Ktb(m);

        end;

        individual(id).CLint.liv = CLint.hep;
        individual(id).CLint_perKg.liv = CLint.hep/individual(id).V(T.liv);
        individual(id).E.hep = CLint.hep*individual(id).Ktb(T.liv)/(Q_liv+CLint.hep*individual(id).Ktb(T.liv));
        individual(id).CLblood.hep = individual(id).E.hep*Q_liv;

        individual(id).Ktup(T.allTisExBlo) = individual(id).Ktb(T.allTisExBlo)*individual(id).fuP/individual(id).BP;
        individual(id).nKtb = individual(id).Ktb;
        individual(id).nKtb(T.liv) = (1-individual(id).E.hep)*individual(id).Ktb(T.liv);
        individual(id).nKtb(T.kid) = (1-individual(id).E.ren)*individual(id).Ktb(T.kid);
        individual(id).nKtb(T.art) = 1; individual(id).nKtb(T.ven) = 1;

        % some PK parameters and characteristics
        individual(id).Vss    = sum(individual(id).V.*individual(id).nKtb);
        individual(id).VssAdi = individual(id).V(T.adi)*individual(id).nKtb(T.adi);
        individual(id).VssLBW = individual(id).Vss-individual(id).VssAdi;
        individual(id).V1     = sum(individual(id).V(T.V1).*individual(id).nKtb(T.V1));
        individual(id).V2     = sum(individual(id).V(T.V2).*individual(id).nKtb(T.V2));
    
        % dosing
        for d = {'po','bolus','infusion'}
            
            route = char(d);
            %individual(id).(route) = study.(route);
            if ~strcmp(study.(route).per,'fixed')
                BSD  = study.(route).per;
                individual(id).(route).dose = study.(route).dose*individual(id).(BSD);
            end;
            
        end;

    end;
    

end;

population = populationStatistics(population,individual);

% ========================================================================

function population = populationStatistics(population,individual)


for id=1:population.size
    population.age(id)     = individual(id).age;
    population.BH(id)      = individual(id).BH;
    population.BW(id)      = individual(id).BW;
    population.LBW(id)     = individual(id).LBW; 
    population.BMI(id)     = individual(id).BMI; 
    population.hct(id)     = individual(id).hct; 
    population.BP(id)      = individual(id).BP; 
    population.SVadi(id)   = individual(id).SV.adi; 
    population.SVski(id)   = individual(id).SV.ski; 
    population.SVtis(id)   = individual(id).SV.tis; 
    population.BSA(id)     = individual(id).BSA; 
    population.co(id)      = individual(id).co; 
    population.E.hep(id)   = individual(id).E.hep; 
    population.Qliv(id)    = individual(id).Q(individual(id).tissue.liv); 
    population.CLblood.hep(id) = individual(id).CLblood.hep; 
    population.CLblood.ren(id) = individual(id).CLblood.ren; 
    population.Vss(id)     = individual(id).Vss; 
    population.R(id)       = individual(id).VssAdi/individual(id).Vss;
    population.VssAdi(id)  = individual(id).VssAdi; 
    population.VssLBW(id)  = individual(id).VssLBW; 
    population.V1(id)      = individual(id).V1; 
    population.V2(id)      = individual(id).V2; 
    population.Q12(id)     = individual(id).Q12; 
    population.po(id)      = individual(id).po;
    population.bolus(id)   = individual(id).bolus;
    population.infusion(id)= individual(id).infusion;
end;
