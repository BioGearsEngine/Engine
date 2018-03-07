%%%
%%% Version: August 31st, 2012
%%% 
%%% Check at URL: http://www.pharmacometrics.de
%%% for potential updates and comments
%%%
%%% This function generates a virtual population of individuals based on 
%%% antropometric data and the LBW-based scaling approach.
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

function [individual,population] = GenericPBPKmodel_virtualPopulation(reference,study,population)

ref = 0;  % do not change

%%% use of 'ref' below: 
%%% If chosen either for BH or for BW, then the corresponding
%%% reference values will be chosen. If chosen for BH and BW, then the
%%% complete parameterization of the reference individual will be chosen.
%%% The difference is mainly due to different LBW and skin. 
%%%

switch population.how2generate
    case 'male'
        population.age = {'age35m'};
        population.BH  =  [ref];
        population.BW  =  [ref];
        population.color = {'r-'};
    case 'adults'
        population.age = {'age35m','age35f'};
        population.BH  =  [ref ref];
        population.BW  =  [ref ref];
        population.color = {'r-' 'b--'};
    case 'all'
        population.age = {'age5','age10','age15m','age15f','age35m','age35f'};
        population.BH  = [ref ref ref ref ref ref];
        population.BW  = [ref ref ref ref ref ref];
        population.color = {'k--' 'k-' 'b--' 'r--' 'b-' 'r-'};
    case 'Grandmaison'
        population.size = 355+329;
        % male
        idVec = 1:355;
        for id=idVec 
               population.age(id)    = {'age35m'};
               population.color(id)  = {'k'};
        end;        
        muBH            = 1.72; 
        sdBH            = 0.075;
        population.BH(idVec) = muBH + sdBH * randn(1,355);        
        muBMI           = 22.8;
        sdBMI           = 3.3;
        % transforming mu and sd to the scale of the underlying normal distribution
        log_muBMI       = log(muBMI^2/sqrt(muBMI^2+sdBMI^2));
        log_sdBMI       = sqrt(log(sdBMI^2/muBMI^2+1)); 
        % BMI lognormal with mean=muBMI and sd=s
        population.BMI(idVec) = exp(log_muBMI + log_sdBMI * randn(1,355));   
        population.BW(idVec)  = population.BMI(idVec) .* (population.BH(idVec).^2); 
        
        % female
        idVec = [1:329]+355;
        for id=idVec 
               population.age(id)    = {'age35f'};
               population.color(id)  = {'b'};
        end;        
        muBH            = 1.61; 
        sdBH            = 0.075;
        population.BH(idVec) = muBH + sdBH * randn(1,329);        
        muBMI           = 22.5;
        sdBMI           = 4.5;
        % transforming mu and sd to the scale of the underlying normal distribution
        log_muBMI       = log(muBMI^2/sqrt(muBMI^2+sdBMI^2));
        log_sdBMI       = sqrt(log(sdBMI^2/muBMI^2+1)); 
        % BMI lognormal with mean=muBMI and sd=s
        population.BMI(idVec) = exp(log_muBMI + log_sdBMI * randn(1,329));   
        population.BW(idVec)  = population.BMI(idVec) .* (population.BH(idVec).^2); 
        population.agesex = [];
    case 'random'
        for id=1:population.size
            population.age(id)    = {population.agesex};
            population.color(id)  = {'k'};
        end;
        muBH            = reference.(population.agesex).BH; 
        sdBH            = 0.075;
        population.BH   = muBH + sdBH * randn(1,population.size);        
        muBMI           = reference.(population.agesex).BMI;
        sdBMI           = 3.3;
        % transforming mu and sd to the scale of the underlying normal distribution
        log_muBMI       = log(muBMI^2/sqrt(muBMI^2+sdBMI^2));
        log_sdBMI       = sqrt(log(sdBMI^2/muBMI^2+1)); 
        % BMI lognormal with mean=muBMI and sd=s
        population.BMI  = exp(log_muBMI + log_sdBMI * randn(1,population.size));   
        population.BW   = population.BMI .* (population.BH.^2);    
    case 'obese'
        age = population.agesex;
        population.age = {age,age,age,age};
        BH = reference.(age).BH;
        population.BH = [BH BH BH BH];
        BMI = reference.(age).BMI;
        population.BW = [0.7*BMI BMI 1.3*BMI 2*BMI].*BH^2;
        population.color = {'m' 'r-' 'b--' 'k--'};
    case 'identical' 
        for id=1:population.size 
               population.age(id)    = {population.agesex};
               population.BH(id)     = reference.(population.agesex).BH;
               population.BW(id)     = reference.(population.agesex).BW;
               population.color(id)  = {'k'};
        end;
    case 'children_small_adults' 
        population.age = {'age5','age35m','age10','age35m','age15m','age35m'};
        population.BH  =  [ref 1.09 ref 1.38 ref 1.67];
        population.BW  =  [ref 19 ref 32 ref 56];
        population.color = {'b' 'b--' 'k' 'k--' 'r' 'r--'};
    otherwise
        error('   Unknow approach of generating a virtual population!');
end;
population.size = length(population.BW);

for id=1:population.size
    
    age = char(population.age(id));
    
    BH = population.BH(id); BW = population.BW(id); 
    
    if BH == ref && BW == ref
        individual(id)        = reference.(age);
        individual(id).id     = id;
        individual(id).color  = population.color(id);
    else
        T = reference.(age).tissue; adi = T.adi; bra = T.bra; ski = T.ski; liv = T.liv; kid = T.kid;
        if BH == ref  BH = reference.(age).BH; end;   % set reference BH, if 'ref' is chosen
        if BW == ref  BW = reference.(age).BW; end;   % set reference BW, if 'ref' is chosen
        
        % set tissue indexing and printing color
        individual(id).id     = id;
        individual(id).type   = reference.(age).type;
        individual(id).tissue = reference.(age).tissue;
        individual(id).color  = population.color(id);

        % age, sex, body height, weight and BMI
        individual(id).age    = reference.(age).age;        
        individual(id).sex    = reference.(age).sex;
        individual(id).BH     = BH; 
        individual(id).BW     = BW;
        individual(id).BMI    = BW/BH^2;
        
        % hematocrit values and volume fractions etc
        individual(id).hct    = reference.(age).hct;
        individual(id).fVtis  = reference.(age).fVtis;
        individual(id).r      = reference.(age).r;

        % scaling of adipose tissue
        individual(id).LBW    = FFM(BW,BH,individual(id).sex);  % Note: LBW approximated by FFM!
        individual(id).SV.adi =(BW - individual(id).LBW) / (reference.(age).BW - reference.(age).LBW);
        individual(id).V(adi) = individual(id).SV.adi * reference.(age).V(adi);

        % scaling of skin
        individual(id).BSA    = BSA(BW,BH);
        individual(id).SV.ski = individual(id).BSA / reference.(age).BSA;
        individual(id).V(ski) = individual(id).SV.ski * reference.(age).V(ski);

        % scaling of brain
        individual(id).SV.bra = 1;
        individual(id).V(bra) = individual(id).SV.bra * reference.(age).V(bra);

        % scaling of all remaining tissues
        m = T.allTisSVtisScaled;
        individual(id).Vres   = individual(id).BW - sum(individual(id).V([adi bra ski]));
        individual(id).SV.tis = individual(id).Vres/reference.(age).Vres;
        individual(id).V(m)   = individual(id).SV.tis * reference.(age).V(m);

        % scaling of blood flows
        individual(id).SQ     = individual(id).SV.tis; 
        individual(id).co     = individual(id).SQ * reference.(age).co;
        individual(id).Q      = individual(id).SQ * reference.(age).Q;

        % parameters regarding distribution and protein binding
        individual(id).Ktup   = reference.(age).Ktup;
        individual(id).Ktb    = reference.(age).Ktb;
        individual(id).nKtb   = reference.(age).nKtb;
        individual(id).Keup   = reference.(age).Keup;
        individual(id).fuP    = reference.(age).fuP;
        individual(id).BP     = reference.(age).BP;
        
        % scaling the clearances. The below procedure results in constant E.hep
        % and CLblood.hep scaled with liver blood flow Q(liv)
        individual(id).E           = reference.(age).E;  % for E.feces and E.gut
        individual(id).CLint_perKg = reference.(age).CLint_perKg;
        individual(id).CLint.liv   = individual(id).CLint_perKg.liv*individual(id).V(T.liv);
        individual(id).E.hep       = individual(id).CLint.liv*individual(id).Ktb(T.liv)/...
                                     (individual(id).Q(T.liv) + individual(id).CLint.liv*individual(id).Ktb(liv));
        individual(id).CLblood.hep = individual(id).Q(T.liv) * individual(id).E.hep;
        
        individual(id).CLint.kid   = individual(id).CLint_perKg.kid*individual(id).V(T.kid);
        individual(id).E.ren       = individual(id).CLint.kid*individual(id).Ktb(T.kid)/...
                                     (individual(id).Q(T.kid) + individual(id).CLint.kid*individual(id).Ktb(kid));

        individual(id).CLblood.ren = individual(id).Q(T.kid) * individual(id).E.ren;

        % some PK parameters and characteristics
        individual(id).Vss    = sum(individual(id).V.*individual(id).nKtb);
        individual(id).VssAdi = individual(id).V(T.adi)*individual(id).nKtb(T.adi);
        individual(id).VssLBW = individual(id).Vss-individual(id).VssAdi;
        individual(id).V1     = sum(individual(id).V(T.V1).*individual(id).nKtb(T.V1));
        individual(id).V2     = sum(individual(id).V(T.V2).*individual(id).nKtb(T.V2));
        individual(id).Q12    = sum(individual(id).Q(T.V2));
       
        
        % dosing
        individual(id).lambda_po = reference.(age).lambda_po;
        for d = {'po','bolus','infusion'}
            
            route = char(d);
            individual(id).(route) = reference.(age).(route);
            if ~strcmp(study.(route).per,'fixed')
                BSD  = study.(route).per;
                individual(id).(route).dose = study.(route).dose*individual(id).(BSD);
            end;
            
        end;
        
                
    end; % if BH == ref && BW == ref
    
end; % for id=1:population.size


%%% ===================================================================

function out = LBW(BW,BH,sex)

% determine lean body weight (LBW) according to
% Green B, Duffull SB. Clin Pharmacol Ther Vol 72, 2002

BMI = BW/BH^2;
if strcmp(sex,'male')
    out = 1.10 * BW - 0.012  * BMI * BW;
else
    out = 1.07 * BW - 0.0148 * BMI * BW;
end;



function out = FFM(BW,BH,sex)

% determine fat free mass (FFM) according to
% Green B, Duffull SB. Clin Pharmacol Ther Vol 72, 2002

BMI = BW/BH^2;
if strcmp(sex,'male')
    out = 9.27e3*BW/(6.68e3+216*BMI);
else
    out = 9.27e3*BW/(8.78e3+244*BMI);
end;



function out = BSA(BW,BH)

% Determine body surface area (BSA) in [m^2]
% Source: Mosteller, New Engl J Med, Vol 317, 1987

out = sqrt(BH*BW/36);


