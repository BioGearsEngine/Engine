%%%
%%% Version: August 31st, 2012
%%% 
%%% Check at URL: http://www.pharmacometrics.de
%%% for potential updates and comments
%%%
%%% This function specifies the physico-chemical parameter values
%%% of various drugs in addition to pharmacokinetic values for human
%%% and rat.
%%%  
%%% NOTE: Not all data sets are complete, might contain generic default 
%%% assumptions or rat values as surrogate for unknown human values. 
%%% PLease check before using the corresponding drug. 
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

function drug = GenericPBPKmodel_drugDatabase(compound,species)

fprintf(' & drug data (%s). ',compound)

% drug data base (db)
% units: weight in [kg], volume in [l], time in [min]
% potential fields (unspecified NaN or with default values) are

% physico-chemical drug properties --------------------------------------
db.(compound).name    = NaN; % name of drug
db.(compound).mode    = NaN; % moderate to strong base 'B', weak base 'wB', neutral 'N', acid 'A'
db.(compound).MW      = NaN; % molecular weight in [g/mol] (not required if unit is mg etc.)
db.(compound).pKa     = NaN; % pKa value
db.(compound).logPow  = NaN; % log octanol-water partition coefficient
db.(compound).logPvow = NaN; % log vegatible oil-water partition coefficient 
                             % (will be estimated from logPov if not speciefied)

% rat related PK parameters ---------------------------------------------
db.(compound).rat.fuP              = NaN; % fraction unbound in plasma
db.(compound).rat.BP               = NaN; % blood-to-plasma ratio 
db.(compound).rat.CLblood_kgBW.hep = 0;   % hepatic blood clearance in [L/min/kg BW]
db.(compound).rat.CLblood_kgBW.ren = 0;   % renal blood clearance in [L/min/kg BW]
db.(compound).rat.lambda_po        = 0;   % first order oral absorption rate constant in [1/min]
db.(compound).rat.E.feces          = 0;   % fraction excreted unchanged in the feces
db.(compound).rat.E.gut            = 0;   % fraction metabolized in the gut

% human related PK parameters -------------------------------------------
db.(compound).human.fuP              = NaN; % fraction unbound in plasma
db.(compound).human.BP               = NaN; % blood-to-plasma ratio 
db.(compound).human.CLblood_kgBW.hep = 0;   % hepatic blood clearance in [L/min/kg BW]
db.(compound).human.CLblood_kgBW.ren = 0;   % renal blood clearance in [L/min/kg BW]
db.(compound).human.lambda_po        = 0;   % first order oral absorption rate constant in [1/min]
db.(compound).human.E.feces          = 0;   % fraction excreted unchanged in the feces
db.(compound).human.E.gut            = 0;   % fraction metabolized in the gut
db.(compound).human.expData.age      = 'age35m'; % age&sex that has been used to determine BP, fuP, CL.hep

% units -----------------------------------------------------------------
db.unit.MW = 'g/mol';
db.unit.pKa = 'unitless';
db.unit.logPow = 'unitless';
db.unit.logPvow = 'unitless';
db.unit.fuP = 'fraction';
db.unit.BP = 'unitless';
db.unit.CLblood_kgBW = 'L/min/kg BW';
db.unit.po_lambda = '1/min';
db.unit.po_Efeces = 'fraction';
db.unit.po_Egut   = 'fraction';



% --- Bases ------------------------------------------------------------

db.Acebutolol.name = 'Acebutolol'; 
db.Acebutolol.mode = 'B'; 
db.Acebutolol.MW = 336.43;
db.Acebutolol.pKa = 9.67;
db.Acebutolol.logPow = 1.87;
db.Acebutolol.logPvow = 0.74;
db.Acebutolol.human.fuP = 0.87;
db.Acebutolol.human.BP = 1.39;
db.Acebutolol.human.CLblood_kgBW.hep = 8.8/1000;

db.AcebutololR.name = 'AcebutololR'; 
db.AcebutololR.mode = 'B'; 
db.AcebutololR.pKa = 9.7;
db.AcebutololR.logPow = 1.87;
db.AcebutololR.logPvow = 0.74;
db.AcebutololR.rat.fuP = 0.79;
db.AcebutololR.rat.BP = 1.09;
db.AcebutololR.rat.E.hep = NaN;

db.AcebutololS.name = 'AcebutololS'; 
db.AcebutololS.mode = 'B'; 
db.AcebutololS.pKa = 9.7;
db.AcebutololS.logPow = 1.87;
db.AcebutololS.logPvow = 0.74;
db.AcebutololS.rat.fuP = 0.73;
db.AcebutololS.rat.BP = 1.01;
db.AcebutololS.rat.E.hep = NaN;

db.Alprenolol.name = 'Alprenolol';
db.Alprenolol.mode = 'B';
db.Alprenolol.MW = 249.35;
db.Alprenolol.pKa = 9.53;
db.Alprenolol.logPow = 3.16;
db.Alprenolol.human.fuP = 0.2;
db.Alprenolol.human.BP = 0.76;

db.Amitriptyline.name = 'Amitriptyline'; 
db.Amitriptyline.mode = 'B'; 
db.Amitriptyline.MW = 277.40;
db.Amitriptyline.pKa = 9.40;
db.Amitriptyline.logPow = 4.90;
db.Amitriptyline.rat.fuP = 0.056;
db.Amitriptyline.rat.BP = 0.86;
db.Amitriptyline.rat.E.hep = NaN; 
db.Amitriptyline.human.fuP = 0.056;
db.Amitriptyline.human.BP = 0.86;
db.Amitriptyline.human.CLblood_kgBW.hep = 12/1000; % Obach99

db.Atenolol.name = 'Atenolol';
db.Atenolol.mode = 'B';
db.Atenolol.MW = 266.94;
db.Atenolol.pKa = 9.55;
db.Atenolol.logPow = 0.16;
db.Atenolol.human.fuP = 0.96;
db.Atenolol.human.BP = 1.11;
db.Atenolol.human.CLblood_kgBW.hep = 1.7/1000; % 1.3-2.1

db.BetaxololR.name = 'BetaxololR'; 
db.BetaxololR.mode = 'B'; 
db.BetaxololR.pKa = 9.4;
db.BetaxololR.logPow = 2.59;
db.BetaxololR.logPvow = 1.54;
db.BetaxololR.rat.fuP = 0.53;
db.BetaxololR.rat.BP = 2.06;

db.BetaxololS.name = 'BetaxololS'; 
db.BetaxololS.mode = 'B'; 
db.BetaxololS.pKa = 9.4;
db.BetaxololS.logPow = 2.59;
db.BetaxololS.logPvow = 1.54;
db.BetaxololS.rat.fuP = 0.54;
db.BetaxololS.rat.BP = 1.91;

db.Biperiden.name = 'Biperiden'; 
db.Biperiden.mode = 'B'; 
db.Biperiden.pKa = 8.8;
db.Biperiden.logPow = 4.25;
db.Biperiden.logPvow = 3.39;
db.Biperiden.rat.fuP = 0.17;
db.Biperiden.rat.BP = 1.19;
db.Biperiden.rat.E.hep = NaN; % to be transformed: 12/1000; % human value
db.Biperiden.human.fuP = 0.1; % RR07
db.Biperiden.human.BP = 0.95; % RR07
db.Biperiden.human.CLblood_kgBW.hep = 12/1000; % ml/min/kg -> l/min/kg
db.Biperiden.human.E.feces   = 0.29;

db.BisoprololR.name = 'BisoprololR'; 
db.BisoprololR.mode = 'B'; 
db.BisoprololR.pKa = 9.4;
db.BisoprololR.logPow = 1.87;
db.BisoprololR.logPvow = 0.74;
db.BisoprololR.rat.fuP = 0.85;
db.BisoprololR.rat.BP = 1.36;
db.BisoprololR.rat.E.hep = NaN; % to be transformed: 2.7/1000; % human value
db.BisoprololR.human.fuP = 0.7;
db.BisoprololR.human.BP = 1.36; % rat value
db.BisoprololR.human.CLblood_kgBW.hep = 2.7/1000; % 1.8-3.6 ml/min/kg -> l/min/kg
db.BisoprololR.human.lambda_po   = 0.01; % 1/min
db.BisoprololR.human.E.feces   = 0.85; % 80-90 %

db.BisoprololS.name = 'BisoprololS'; 
db.BisoprololS.mode = 'B'; 
db.BisoprololS.pKa = 9.4;
db.BisoprololS.logPow = 1.87;
db.BisoprololS.logPvow = 0.74;
db.BisoprololS.rat.fuP = 0.85;
db.BisoprololS.rat.BP = 1.36;
db.BisoprololS.rat.E.hep = NaN; % to be transformed: 2.7/1000; % human value

db.Bupivacaine.name = 'Bupivacaine';
db.Bupivacaine.mode = 'B';
db.Bupivacaine.MW = 288.43;
db.Bupivacaine.pKa = 8.13;
db.Bupivacaine.logPow = 3.41;
db.Bupivacaine.human.fuP = 0.045;
db.Bupivacaine.human.BP = 0.64;
db.Bupivacaine.human.CLblood_kgBW.hep = 7.5/1000;

db.Caffeine.name = 'Caffeine';
db.Caffeine.mode = 'B';
db.Caffeine.MW = 194.19;
db.Caffeine.pKa = 10.4;
db.Caffeine.logPow = -0.091;
db.Caffeine.human.fuP = 0.7;
db.Caffeine.human.BP = 1.04;
db.Caffeine.human.CLblood_kgBW.hep = 1.4/1000;

db.Carvedilol.name = 'Carvedilol'; 
db.Carvedilol.mode = 'B'; 
db.Carvedilol.MW = 406.47;
db.Carvedilol.pKa = 8.1;
db.Carvedilol.logPow = 4.19;
db.Carvedilol.logPvow = 3.32;
db.Carvedilol.human.fuP = 0.025; % RR07
db.Carvedilol.human.BP = 0.71; % RR07
db.Carvedilol.human.CLblood_kgBW.hep = 8.7/1000; % ml/min/kg -> l/min/kg

db.CarvedilolR.name = 'CarvedilolR'; 
db.CarvedilolR.mode = 'B'; 
db.CarvedilolR.pKa = 8.1;
db.CarvedilolR.logPow = 4.19;
db.CarvedilolR.logPvow = 3.32;
db.CarvedilolR.rat.fuP = 0.019;
db.CarvedilolR.rat.BP = 0.81;
db.CarvedilolR.rat.E.hep = NaN; % to be transformed: 8.7/1000; % human value
db.CarvedilolR.human.fuP = 0.02;
db.CarvedilolR.human.BP = 0.71; % RR07
db.CarvedilolR.human.CLblood_kgBW.hep = 8.7/1000; % ml/min/kg -> l/min/kg
db.CarvedilolR.human.E.feces   = 0.31;

db.Chlorpheniramine.name = 'Chlorpheniramine';
db.Chlorpheniramine.mode = 'B';
db.Chlorpheniramine.MW = 274.79;
db.Chlorpheniramine.pKa = 9.13;
db.Chlorpheniramine.logPow = 3.07;
db.Chlorpheniramine.human.fuP = 0.28;
db.Chlorpheniramine.human.BP = 1.34;
db.Chlorpheniramine.human.CLblood_kgBW.hep = 3/1000; % 1.4-4.7

db.Clomipramine.name = 'Clomipramine';
db.Clomipramine.mode = 'B';
db.Clomipramine.MW = 314.85;
db.Clomipramine.pKa = 9.38;
db.Clomipramine.logPow = 5.22;
db.Clomipramine.human.fuP = 0.068;
db.Clomipramine.human.BP = 1.05;
db.Clomipramine.human.CLblood_kgBW.hep = 8/1000; % 5.5-11

db.Cocaine.name = 'Cocaine';
db.Cocaine.mode = 'B';
db.Cocaine.MW = 303.35;
db.Cocaine.pKa = 8.66;
db.Cocaine.logPow = 2.3;
db.Cocaine.human.fuP = 0.09;
db.Cocaine.human.BP = 1;
db.Cocaine.human.CLblood_kgBW.hep = 29/1000; % 23-35

db.Desipramine.name = 'Desipramine';
db.Desipramine.mode = 'B';
db.Desipramine.MW = 266.38;
db.Desipramine.pKa = 10.32;
db.Desipramine.logPow = 4.9;
db.Desipramine.human.fuP = 0.19;
db.Desipramine.human.BP = 0.96;
db.Desipramine.human.CLblood_kgBW.hep = 12/1000; % Obach99

db.Diltiazem.name = 'Diltiazem';
db.Diltiazem.mode = 'B';
db.Diltiazem.MW = 414.52;
db.Diltiazem.pKa = 7.7;
db.Diltiazem.logPow = 2.67;
db.Diltiazem.human.fuP = 0.2;
db.Diltiazem.human.BP = 1.03;
db.Diltiazem.human.CLblood_kgBW.hep = 12/1000; % Obach99

db.Diphenhydramine.name = 'Diphenhydramine';
db.Diphenhydramine.mode = 'B';
db.Diphenhydramine.MW = 255.35;
db.Diphenhydramine.pKa = 8.98;
db.Diphenhydramine.logPow = 3.31;
db.Diphenhydramine.human.fuP = 0.089;
db.Diphenhydramine.human.BP = 0.8;
db.Diphenhydramine.human.CLblood_kgBW.hep = 9.5/1000; % Obach99

db.Doxepine.name = 'Doxepine';
db.Doxepine.mode = 'B';
db.Doxepine.MW = 279.38;
db.Doxepine.pKa = 9;
db.Doxepine.logPow = 4.01;
db.Doxepine.human.fuP = 0.19;
db.Doxepine.human.BP = 1.25;
db.Doxepine.human.CLblood_kgBW.hep = 14/1000;

db.Etidocaine.name = 'Etidocaine';
db.Etidocaine.mode = 'B';
db.Etidocaine.pKa = 7.8;
db.Etidocaine.logPow = 3.45;
db.Etidocaine.human.fuP = 0.06;
db.Etidocaine.human.BP = 0.59;
db.Etidocaine.human.CLblood_kgBW.hep = 15.8/1000;

db.Fentanyl.name = 'Fentanyl'; 
db.Fentanyl.mode = 'B'; 
db.Fentanyl.pKa = 9.0;
db.Fentanyl.logPow = 4.05;
db.Fentanyl.logPvow = 3.17;
db.Fentanyl.rat.fuP = 0.15;
db.Fentanyl.rat.BP = 0.89;
db.Fentanyl.rat.E.hep = NaN; % to be transformed: 2.12/60*species.BW; % human value
db.Fentanyl.human.fuP = 0.16;  % RR07
db.Fentanyl.human.BP = 1; % RR07
db.Fentanyl.human.CLblood_kgBW.hep = 2.12/60; % l/h/kg -> l/min/kg

db.Haloperidol.name = 'Haloperidol';
db.Haloperidol.mode = 'B';
db.Haloperidol.MW = 375.86;
db.Haloperidol.pKa = 8.48;
db.Haloperidol.logPow = 4.3;
db.Haloperidol.human.fuP = 0.1;
db.Haloperidol.human.BP = 1.08;
db.Haloperidol.human.CLblood_kgBW.hep = 12/1000;

db.Imipramine.name = 'Imipramine'; 
db.Imipramine.mode = 'B'; 
db.Imipramine.MW = 280.41;
db.Imipramine.pKa = 9.5;
db.Imipramine.logPow = 4.80;
db.Imipramine.logPvow = 4.00;
db.Imipramine.rat.fuP = 0.24;
db.Imipramine.rat.BP = 1.67;
db.Imipramine.rat.CLblood_kgBW.hep = 12/1000; % human, ml/min/kg -> l/min/kg
db.Imipramine.rat.E.feces   = 1;
db.Imipramine.human.fuP = 0.13; % RR07
db.Imipramine.human.BP = 1.07; % RR07
db.Imipramine.human.CLblood_kgBW.hep = 12/1000; % Obach99, ml/min/kg -> l/min/kg

db.Inaperisone.name = 'Inaperisone'; 
db.Inaperisone.mode = 'B'; 
db.Inaperisone.pKa = 9.1;
db.Inaperisone.logPow = 3.72;
db.Inaperisone.logPvow = 2.79;
db.Inaperisone.rat.fuP = 0.24;
db.Inaperisone.rat.BP = 1.88;

db.Ketamine.name = 'Ketamine';
db.Ketamine.mode = 'B';
db.Ketamine.MW = 237.73;
db.Ketamine.pKa = 7.5;
db.Ketamine.logPow = 2.18;
db.Ketamine.human.fuP = 0.77;
db.Ketamine.human.BP = 0.82;
db.Ketamine.human.CLblood_kgBW.hep = 20/1000; % Obach99

db.Ketanserine.name = 'Ketanserine';
db.Ketanserine.mode = 'B';
db.Ketanserine.pKa = 7.31;
db.Ketanserine.logPow = 3.29;
db.Ketanserine.human.fuP = 0.049;
db.Ketanserine.human.BP = 0.7;
db.Ketanserine.human.CLblood_kgBW.hep = 8/1000; % 6-10

db.Lidocaine.name = 'Lidocaine'; 
db.Lidocaine.mode = 'B'; 
db.Lidocaine.MW = 234.34;
db.Lidocaine.pKa = 8.01;
db.Lidocaine.logPow = 2.26;
db.Lidocaine.logPvow = 1.27;
db.Lidocaine.rat.fuP = 0.38;
db.Lidocaine.rat.BP = 1.27;
db.Lidocaine.rat.E.hep = NaN; % to be transformed: 15/1000; % human value
db.Lidocaine.rat.E.feces   = 1; % default assumption
db.Lidocaine.human.fuP = 0.296;
db.Lidocaine.human.BP = 0.84;
db.Lidocaine.human.CLblood_kgBW.hep = 15/1000; % ml/min/kg -> l/min/kg
db.Lidocaine.human.E.feces   = 0; % default assumption
db.Lidocaine.human.lambda_po   = 0.018; % 1/min Boyes etal (1970), J Pharmacol Exp Ther 174:1?8 (dogs)

db.Lorcainide.name = 'Lorcainide';
db.Lorcainide.mode = 'B';
db.Lorcainide.pKa = 9.5;
db.Lorcainide.logPow = 4.85;
db.Lorcainide.human.fuP = 0.16;
db.Lorcainide.human.BP = 0.77;
db.Lorcainide.human.CLblood_kgBW.hep = 18/1000; % Obach99

db.Maprotiline.name = 'Maprotiline';
db.Maprotiline.mode = 'B';
db.Maprotiline.MW = 277.4;
db.Maprotiline.pKa = 9.9;
db.Maprotiline.logPow = 4.75;
db.Maprotiline.human.fuP = 0.12;
db.Maprotiline.human.BP = 2.13;

db.Meperidine.name = 'Meperidine';
db.Meperidine.mode = 'B';
db.Meperidine.MW = 247.33;
db.Meperidine.pKa = 8.64;
db.Meperidine.logPow = 2.72;
db.Meperidine.human.fuP = 0.38;
db.Meperidine.human.BP = 1.11;

db.Mepivacaine.name = 'Mepivacaine';
db.Mepivacaine.mode = 'B';
db.Mepivacaine.MW = 246.35;
db.Mepivacaine.pKa = 7.76;
db.Mepivacaine.logPow = 1.88;
db.Mepivacaine.human.fuP = 0.23;
db.Mepivacaine.human.BP = 0.91;
db.Mepivacaine.human.CLblood_kgBW.hep = 11.1/1000;

db.Metclopramide.name = 'Metclopramide';
db.Metclopramide.mode = 'B';
db.Metclopramide.MW = 299.8;
db.Metclopramide.pKa = 9.3;
db.Metclopramide.logPow = 2.6;
db.Metclopramide.human.fuP = 0.65;
db.Metclopramide.human.BP = 1.08;
db.Metclopramide.human.CLblood_kgBW.hep = 9.5/1000; % 8-11

db.Methadone.name = 'Methadone';
db.Methadone.mode = 'B';
db.Methadone.MW = 309.45;
db.Methadone.pKa = 8.94;
db.Methadone.logPow = 3.93;
db.Methadone.human.fuP = 0.13;
db.Methadone.human.BP = 0.77;
db.Methadone.human.CLblood_kgBW.hep = 15/1000; % 0.33-30

db.Metolazone.name = 'Metolazone';
db.Metolazone.mode = 'B';
db.Metolazone.MW = 365.84;
db.Metolazone.pKa = 9.7;
db.Metolazone.logPow = 1.8;
db.Metolazone.human.fuP = 0.057;
db.Metolazone.human.BP = 1.13;
db.Metolazone.human.CLblood_kgBW.hep = NaN;

db.Metoprolol.name = 'Metoprolol';
db.Metoprolol.mode = 'B';
db.Metoprolol.MW = 267.36;
db.Metoprolol.pKa = 9.7;
db.Metoprolol.logPow = 2.15;
db.Metoprolol.human.fuP = 0.9;
db.Metoprolol.human.BP = 1.14;
db.Metoprolol.human.CLblood_kgBW.hep = 12.15/1000;

db.Mexiletine.name = 'Mexiletine';
db.Mexiletine.mode = 'B';
db.Mexiletine.MW = 179.26;
db.Mexiletine.pKa = 9;
db.Mexiletine.logPow = 2.15;
db.Mexiletine.human.fuP = 0.34;
db.Mexiletine.human.BP = 1.12;
db.Mexiletine.human.CLblood_kgBW.hep = 7.3/1000; % 6.3-8.3

db.Morphine.name = 'Morphine';
db.Morphine.mode = 'B';
db.Morphine.MW = 285.34;
db.Morphine.pKa = 8.35;
db.Morphine.logPow = 0.89;
db.Morphine.human.fuP = 0.71;
db.Morphine.human.BP = 1.02;
db.Morphine.human.CLblood_kgBW.hep = 18/1000; % 8-27

db.Naloxone.name = 'Naloxone';
db.Naloxone.mode = 'B';
db.Naloxone.MW = 327.37;
db.Naloxone.pKa = 7.9;
db.Naloxone.logPow = 2.1;
db.Naloxone.human.fuP = 0.59;
db.Naloxone.human.BP = 1.22;
db.Naloxone.human.CLblood_kgBW.hep = 22/1000;

db.Nicardipine.name = 'Nicardipine';
db.Nicardipine.mode = 'B';
db.Nicardipine.MW = 479.53;
db.Nicardipine.pKa = 8.6;
db.Nicardipine.logPow = 3.82;
db.Nicardipine.human.fuP = 0.04;
db.Nicardipine.human.BP = 0.71;
db.Nicardipine.human.CLblood_kgBW.hep = 12/1000; % 7-17

db.Nitrofurantoin.name = 'Nitrofurantoin';
db.Nitrofurantoin.mode = 'B';
db.Nitrofurantoin.MW = 238.16;
db.Nitrofurantoin.pKa = 7.2;
db.Nitrofurantoin.logPow = -0.47;
db.Nitrofurantoin.human.fuP = 0.39;
db.Nitrofurantoin.human.BP = 0.76;

db.Nortriptyline.name = 'Nortriptyline';
db.Nortriptyline.mode = 'B';
db.Nortriptyline.MW = 263.38;
db.Nortriptyline.pKa = 9.7;
db.Nortriptyline.logPow = 4.28;
db.Nortriptyline.human.fuP = 0.05;
db.Nortriptyline.human.BP = 1.58;

db.Ondansetron.name = 'Ondansetron';
db.Ondansetron.mode = 'B';
db.Ondansetron.MW = 293.36;
db.Ondansetron.pKa = 7.4;
db.Ondansetron.logPow = 1.92;
db.Ondansetron.logPvow = NaN;
db.Ondansetron.human.fuP = 028;
db.Ondansetron.human.BP = 0.83;
db.Ondansetron.human.CLblood_kgBW.hep = 5.9/1000; % 4.3-10

db.Oxprenolol.name = 'Oxprenolol';
db.Oxprenolol.mode = 'B';
db.Oxprenolol.MW = 265.35;
db.Oxprenolol.pKa = 9.5;
db.Oxprenolol.logPow = 2.18;
db.Oxprenolol.human.fuP = 0.14;
db.Oxprenolol.human.BP = 0.64;

db.Pentazocine.name = 'Pentazocine'; 
db.Pentazocine.mode = 'B'; 
db.Pentazocine.pKa = 8.5;
db.Pentazocine.logPow = 3.31;
db.Pentazocine.logPvow = 2.34;
db.Pentazocine.rat.fuP = 0.46;
db.Pentazocine.rat.BP = 1.55;
db.Pentazocine.rat.E.hep = NaN;
db.Pentazocine.human.fuP = 0.37; % RR07
db.Pentazocine.human.BP = 1.07; % RR07
db.Pentazocine.human.CLblood_kgBW.hep = 18/1000; % 17-20 ml/min/kg -> l/min
db.Pentazocine.human.E.feces   = 0.21; % 11-32 %

db.Phencyclidine.name = 'Phencyclidine'; 
db.Phencyclidine.mode = 'B'; 
db.Phencyclidine.MW = 285.42;
db.Phencyclidine.pKa = 9.4;
db.Phencyclidine.logPow = 4.96;
db.Phencyclidine.logPvow = 4.10;
db.Phencyclidine.rat.fuP = 0.47;
db.Phencyclidine.rat.BP = 1.12;
db.Phencyclidine.rat.E.hep = 5.4/1000; % human value
db.Phencyclidine.human.fuP = 0.35; % 30-40 %
db.Phencyclidine.human.CLblood_kgBW.hep = 5.4/1000; % ml/min/kg -> l/min/kg
db.Phencyclidine.human.E.feces   = 0.7; % 50-90 %

db.Pindolol.name = 'Pindolol';
db.Pindolol.mode = 'B';
db.Pindolol.MW = 248.32;
db.Pindolol.pKa = 8.8;
db.Pindolol.logPow = 1.75;
db.Pindolol.human.fuP = 0.41;
db.Pindolol.human.BP = 0.81;
db.Pindolol.human.CLblood_kgBW.hep = 4.2/1000;

db.Prilocaine.name = 'Prilocaine';
db.Prilocaine.mode = 'B';
db.Prilocaine.MW = 220.31;
db.Prilocaine.pKa = 7.9;
db.Prilocaine.logPow = 2.11;
db.Prilocaine.human.fuP = 0.61;
db.Prilocaine.human.BP = 1.18;
db.Prilocaine.human.CLblood_kgBW.hep = 34/1000;

db.Prilocaine.name = 'Prilocaine';
db.Prilocaine.mode = 'B';
db.Prilocaine.MW = 220.31;
db.Prilocaine.pKa = 7.9;
db.Prilocaine.logPow = 2.11;
db.Prilocaine.human.fuP = 0.61;
db.Prilocaine.human.BP = 1.18;
db.Prilocaine.human.CLblood_kgBW.hep = 34/1000;

db.Procainamide.name = 'Procainamide'; 
db.Procainamide.mode = 'B'; 
db.Procainamide.MW = 235.33;
db.Procainamide.pKa = 9.2;
db.Procainamide.logPow = 0.88;
db.Procainamide.logPvow = -0.37;
db.Procainamide.rat.fuP = 0.92;
db.Procainamide.rat.BP = 1.00;
db.Procainamide.rat.E.hep = 9.2/1000; % human value
db.Procainamide.human.fuP = 0.84; % RR07
db.Procainamide.human.BP = 0.98; % RR07
db.Procainamide.human.CLblood_kgBW.hep = 9.2/1000; % 8.6-9.8 ml/min/kg -> l/min/kg
db.Procainamide.human.E.feces   = 0.8; % 75-85 %

db.Propafenone.name = 'Propafenone';
db.Propafenone.mode = 'B';
db.Propafenone.MW = 341.44;
db.Propafenone.pKa = 9.74;
db.Propafenone.logPow = 4.24;
db.Propafenone.human.fuP = 0.05;
db.Propafenone.human.BP = 0.7;
db.Propafenone.human.CLblood_kgBW.hep = 19/1000; % Obach99

db.Propranolol.name = 'Propranolol';
db.Propranolol.mode = 'B';
db.Propranolol.MW = 259.34;
db.Propranolol.pKa = 9.45;
db.Propranolol.logPow = 3.65;
db.Propranolol.human.fuP = 0.11;
db.Propranolol.human.BP = 0.8;
db.Propranolol.human.CLblood_kgBW.hep = 13/1000;

db.Remoxipride.name = 'Remoxipride';
db.Remoxipride.mode = 'B';
db.Remoxipride.MW = 371.27;
db.Remoxipride.pKa = 8.9;
db.Remoxipride.logPow = 2.1;
db.Remoxipride.human.fuP = 0.2;
db.Remoxipride.human.BP = 0.7;
db.Remoxipride.human.CLblood_kgBW.hep = 1.5/1000;

db.Ropivacaine.name = 'Ropivacaine';
db.Ropivacaine.mode = 'B';
db.Ropivacaine.MW = 274.4;
db.Ropivacaine.pKa = 8.2;
db.Ropivacaine.logPow = 2.09;
db.Ropivacaine.human.fuP = 0.07;
db.Ropivacaine.human.BP = 0.69;
db.Ropivacaine.human.CLblood_kgBW.hep = 0.12/1000;

db.Sildefanil.name = 'Sildefanil';
db.Sildefanil.mode = 'B';
db.Sildefanil.MW = 474.58;
db.Sildefanil.pKa = 7.6;
db.Sildefanil.logPow = 2.75;
db.Sildefanil.human.fuP = 0.04;
db.Sildefanil.human.BP = 0.62;
db.Sildefanil.human.CLblood_kgBW.hep = 6/1000;

db.Theophylline.name = 'Theophylline';
db.Theophylline.mode = 'B';
db.Theophylline.MW = 180.16;
db.Theophylline.pKa = 8.71;
db.Theophylline.logPow = 0.26;
db.Theophylline.human.fuP = 0.6;
db.Theophylline.human.BP = 0.83;
db.Theophylline.human.CLblood_kgBW.hep = 0.65/1000;

db.Tolamolol.name = 'Tolamolol';
db.Tolamolol.mode = 'B';
db.Tolamolol.MW = NaN;
db.Tolamolol.pKa = 7.94;
db.Tolamolol.logPow = 2.19;
db.Tolamolol.human.fuP = 0.09;
db.Tolamolol.human.BP = 0.76;

db.Verapamil.name = 'Verapamil'; 
db.Verapamil.mode = 'B'; 
db.Verapamil.MW = 454.60;
db.Verapamil.pKa = 8.5;
db.Verapamil.logPow = 3.79;
db.Verapamil.logPvow = 2.88;
db.Verapamil.rat.fuP = 0.05;
db.Verapamil.rat.BP = 0.85;
db.Verapamil.rat.E.hep = NaN; 
db.Verapamil.human.fuP = 0.082; % RR07
db.Verapamil.human.BP = 0.84; % RR07
db.Verapamil.human.CLblood_kgBW.hep = 19/1000; % Obach99
db.Verapamil.human.E.feces   = 0.22; % 12-33 %

% --- Weak Bases --------------------------------------------------------

db.Alfentanil.name = 'Alfentanil'; 
db.Alfentanil.mode = 'wB'; 
db.Alfentanil.MW = 416.5172;
db.Alfentanil.pKa = 6.5;
db.Alfentanil.logPow = 2.2; 
db.Alfentanil.rat.fuP = 0.11;
db.Alfentanil.rat.E.hep = NaN;
db.Alfentanil.human.fuP = 0.08; % 5-12 %
db.Alfentanil.human.BP = 1;
db.Alfentanil.human.CLblood_kgBW.hep = 4/1000; % 2.3-6.5 ml/min/kg -> l/min/kg

db.Alprazolam.name = 'Alprazolam'; 
db.Alprazolam.mode = 'wB'; 
db.Alprazolam.MW = 416.5172;
db.Alprazolam.pKa = 2.4;
db.Alprazolam.logPow = 2.3; 
db.Alprazolam.rat.fuP = 0.35;
db.Alprazolam.rat.E.hep = NaN; 
db.Alprazolam.human.fuP = 0.32; % Obach99
db.Alprazolam.human.BP = 0.78; % Obach99
db.Alprazolam.human.CLblood_kgBW.hep = 0.76/1000; % Obach99
db.Alprazolam.human.E.feces   = 0.85; % 80-90 %

db.Chlordiazepoxide.name = 'Chlordiazepoxide'; 
db.Chlordiazepoxide.mode = 'wB'; 
db.Chlordiazepoxide.MW = 299.755;
db.Chlordiazepoxide.pKa = 4.7;
db.Chlordiazepoxide.logPow = 2.4; 
db.Chlordiazepoxide.rat.fuP = 0.15;
db.Chlordiazepoxide.rat.E.hep = NaN; 
db.Chlordiazepoxide.human.fuP = 0.15;
db.Chlordiazepoxide.human.BP = 1.4;
db.Chlordiazepoxide.human.CLblood_kgBW.hep = 1.8/1000/73; % ml/min/(73 kg body weight)

db.Diazepam.name = 'Diazepam'; 
db.Diazepam.mode = 'wB'; 
db.Diazepam.MW = 284.74;
db.Diazepam.pKa = 3.4;
db.Diazepam.logPow = 2.84; 
db.Diazepam.rat.fuP = 0.14;
db.Diazepam.rat.E.hep = NaN; 
db.Diazepam.human.fuP = 0.013; % Obach99
db.Diazepam.human.BP = 0.71; % Obach99
db.Diazepam.human.CLblood_kgBW.hep = 0.6/1000; % Obach99
db.Diazepam.human.E.feces   = 0.97; % 94-100 %

db.Flunitrazepam.name = 'Flunitrazepam'; 
db.Flunitrazepam.mode = 'wB'; 
db.Flunitrazepam.MW = 313.2832;
db.Flunitrazepam.pKa = 1.8;
db.Flunitrazepam.logPow = 2.1; 
db.Flunitrazepam.rat.fuP = 0.25;
db.Flunitrazepam.rat.E.hep = NaN;
db.Flunitrazepam.human.fuP = 0.25;
db.Flunitrazepam.human.BP = 1.2;
db.Flunitrazepam.human.CLblood_kgBW.hep = 9.8/1000/73; % ml/min/(73kg BW) -> l/min/kg; 
db.Flunitrazepam.human.E.feces   = 0.85;

db.Midazolam.name = 'Midazolam'; 
db.Midazolam.mode = 'wB'; 
db.Midazolam.MW = 325.767;
db.Midazolam.pKa = 6.0;
db.Midazolam.logPow = 3.15; 
db.Midazolam.rat.fuP = 0.059;
db.Midazolam.rat.E.hep = NaN; 
db.Midazolam.human.fuP = 0.05; % Obach99
db.Midazolam.human.BP = 0.567; % Obach99 (changed from 0.53 to 0.567)
db.Midazolam.human.CLblood_kgBW.hep = 8.7/1000; % Obach99
db.Midazolam.human.lambda_po   = 0.16; % 1/min
db.Midazolam.human.E.gut = 0.444;
db.Midazolam.human.E.feces   = 0.4; % 28.2; 40-50

db.Triazolam.name = 'Triazolam'; 
db.Triazolam.mode = 'wB'; 
db.Triazolam.MW = 343.21;
db.Triazolam.pKa = 2.0;
db.Triazolam.logPow = 2.4; 
db.Triazolam.rat.fuP = 0.28;
db.Triazolam.rat.E.hep = NaN; 
db.Triazolam.human.fuP = 0.1; % Obach99
db.Triazolam.human.BP = 0.62; % Obach99
db.Triazolam.human.CLblood_kgBW.hep = 4.7/1000; % human value
db.Triazolam.human.E.feces   = 1;


% --- Neutral ----------------------------------------------------------

db.Digoxin.name = 'Digoxin'; 
db.Digoxin.mode = 'N'; 
db.Digoxin.MW = 780.9385;
db.Digoxin.logPow = 1.23;
db.Digoxin.rat.fuP = 0.61;
db.Digoxin.rat.E.hep = NaN; % to be transformed: 1.8/1000; % human
db.Digoxin.human.fuP = 0.74; % RR07
db.Digoxin.human.BP = 1;
db.Digoxin.human.CLblood_kgBW.hep = 12.2/60/73; % l/hr/(73 kg BW) -> l/min/kg
db.Digoxin.human.E.feces   = 1;

db.Ethoxybenzamide.name = 'Ethoxybenzamide'; 
db.Ethoxybenzamide.mode = 'N'; 
db.Ethoxybenzamide.logPow = 0.8;
db.Ethoxybenzamide.rat.fuP = 0.59;

db.Ftorafur.name = 'Ftorafur'; 
db.Ftorafur.mode = 'N'; 
db.Ftorafur.logPow = -0.3;
db.Ftorafur.rat.fuP = 0.78;


% --- Acids ------------------------------------------------------------

db.Amobarbital.name = 'Amobarbital';
db.Amobarbital.mode = 'A';
db.Amobarbital.MW = 226.27;
db.Amobarbital.pKa = 7.9;
db.Amobarbital.logPow = 1.89;
db.Amobarbital.human.fuP = 0.39;
db.Amobarbital.human.BP = 1.5;
db.Amobarbital.human.CLblood_kgBW.hep = 0.35/1000;

db.Cefazolin.name = 'Cefazolin'; 
db.Cefazolin.mode = 'A'; 
db.Cefazolin.MW = 454.507;
db.Cefazolin.pKa = 2.3;
db.Cefazolin.logPow = 0.3;
db.Cefazolin.rat.fuP = 0.16;

db.Diclofenac.name = 'Diclofenac';
db.Diclofenac.mode = 'A';
db.Diclofenac.MW = 296.15;
db.Diclofenac.pKa = 4.15;
db.Diclofenac.logPow = 3.9;
db.Diclofenac.human.fuP = 0.005;
db.Diclofenac.human.BP = 0.567;
db.Diclofenac.human.CLblood_kgBW.hep = 7.6/1000;

db.Dideoxyinosine.name = 'Dideoxyinosine'; 
db.Dideoxyinosine.mode = 'A'; 
db.Dideoxyinosine.MW = 236.2273;
db.Dideoxyinosine.pKa = 9.1;
db.Dideoxyinosine.logPow = -1.2;
db.Dideoxyinosine.rat.fuP = 0.97;
db.Dideoxyinosine.human.fuP = 0.96; % RR07

db.Etodolac.name = 'Etodolac'; 
db.Etodolac.mode = 'A'; 
db.Etodolac.MW = 287.3535;
db.Etodolac.pKa = 4.7;
db.Etodolac.logPow = 3.6;
db.Etodolac.human.fuP = 0.01;
db.Etodolac.human.CLblood_kgBW.hep = 0.68/1000; % ml/min/kg

db.Hexobarbital.name = 'Hexobarbital';
db.Hexobarbital.mode = 'A';
db.Hexobarbital.MW = 236.27;
db.Hexobarbital.pKa = 8.29;
db.Hexobarbital.logPow = 1.74;
db.Hexobarbital.human.fuP = 0.53;
db.Hexobarbital.human.BP = 1;
db.Hexobarbital.human.CLblood_kgBW.hep = 3.6/1000;

db.Ibuprofen.name = 'Ibuprofen';
db.Ibuprofen.mode = 'A';
db.Ibuprofen.MW = 206.28;
db.Ibuprofen.pKa = 4.7;
db.Ibuprofen.logPow = 4.06;
db.Ibuprofen.human.fuP = 0.01;
db.Ibuprofen.human.BP = 0.567;
db.Ibuprofen.human.CLblood_kgBW.hep = 1.5/1000;

db.Methohexital.name = 'Methohexital';
db.Methohexital.mode = 'A';
db.Methohexital.MW = 262.3;
db.Methohexital.pKa = 8.51;
db.Methohexital.logPow = 1.72;
db.Methohexital.human.fuP = 0.27;
db.Methohexital.human.BP = 0.7;
db.Methohexital.human.CLblood_kgBW.hep = 16/1000;

db.Penicillin.name = 'Penicillin'; 
db.Penicillin.mode = 'A'; 
db.Penicillin.MW = 350.39;
db.Penicillin.pKa = 2.7;
db.Penicillin.logPow = 1.6;
db.Penicillin.rat.fuP = 0.15;
db.Penicillin.human.fuP = 0.2;

db.Phenobarbital.name = 'Phenobarbital'; 
db.Phenobarbital.mode = 'A'; 
db.Phenobarbital.MW = 232.2353;
db.Phenobarbital.pKa = 7.4;
db.Phenobarbital.logPow = 1.6;
db.Phenobarbital.rat.fuP = 0.64;
db.Phenobarbital.rat.E.hep = NaN; 
db.Phenobarbital.human.fuP = 0.52; % RR07
db.Phenobarbital.human.BP = 0.86;
db.Phenobarbital.human.CLblood_kgBW.hep = 3.8/60/1000; % ml/h/kg -> l/min/kg
db.Phenobarbital.human.lambda_po   = 0.03; % 1/min
db.Phenobarbital.human.E.feces   = 0.8; % 80,95-100 %

db.Phenytoin.name = 'Phenytoin'; 
db.Phenytoin.mode = 'A'; 
db.Phenytoin.MW = 252.268;
db.Phenytoin.pKa = 8.3;
db.Phenytoin.logPow = 2.5;
db.Phenytoin.rat.fuP = 0.19;
db.Phenytoin.human.fuP = 0.12; % RR07
db.Phenytoin.human.E.feces   = 0.9; % 85-95 %

db.SalicyclicAcid.name = 'SalicyclicAcid'; 
db.SalicyclicAcid.mode = 'A'; 
db.SalicyclicAcid.MW = 138.1207;
db.SalicyclicAcid.pKa = 3.0;
db.SalicyclicAcid.logPow = 2.0;
db.SalicyclicAcid.rat.fuP = 0.137;

db.Thiopental.name = 'Thiopental'; 
db.Thiopental.mode = 'A'; 
db.Thiopental.MW = 242.338;
db.Thiopental.pKa = 7.5;
db.Thiopental.logPow = 2.9;
db.Thiopental.rat.fuP = 0.13;
db.Thiopental.rat.E.hep = NaN; 
db.Thiopental.human.fuP = 0.18; % RR07
db.Thiopental.human.BP = 0.88; % unknown source
db.Thiopental.human.CLblood_kgBW.hep = 2.02/1000; % 1.9-5.4 ml/min/kg -> l/min/kg

db.Tolbutamide.name = 'Tolbutamide'; 
db.Tolbutamide.mode = 'A'; 
db.Tolbutamide.MW = 270.35;
db.Tolbutamide.pKa = 5.3;
db.Tolbutamide.logPow = 2.39;
db.Tolbutamide.rat.fuP = 0.27;
db.Tolbutamide.rat.E.hep = NaN; 
db.Tolbutamide.human.fuP = 0.04; % Obach99
db.Tolbutamide.human.BP = 0.567; % Obach99
db.Tolbutamide.human.CLblood_kgBW.hep = 0.36/1000; % Obach99
db.Tolbutamide.human.E.feces   = 0.95;

db.Valproate.name = 'Valproate'; 
db.Valproate.mode = 'A'; 
db.Valproate.MW = 144.2114;
db.Valproate.pKa = 4.6;
db.Valproate.logPow = 2.76;
db.Valproate.rat.fuP = 0.37;
db.Valproate.human.fuP = 0.099; % RR07
db.Valproate.human.BP = 0.567; % E:P -> B:P
db.Valproate.human.CLblood_kgBW.hep = 0.11/1000; % 0.09-0.46 ml/min/kg -> l/min/kg

db.Warfarin.name = 'Warfarin';
db.Warfarin.mode = 'A';
db.Warfarin.MW = 308.33;
db.Warfarin.pKa = 5.08;
db.Warfarin.logPow = 3;
db.Warfarin.human.fuP = 0.01;
db.Warfarin.human.BP = 0.567;
db.Warfarin.human.CLblood_kgBW.hep = 0.081/1000;

% --- Zwitterions ---------------------------------------------------------

db.Ofloxacin.name = 'Ofloxacin'; 
db.Ofloxacin.mode = 'AB'; 
db.Ofloxacin.pKa = [6.1 8.2];
db.Ofloxacin.logPow = -0.4;
db.Ofloxacin.rat.fuP = 0.77;
db.Ofloxacin.rat.BP = 0.92; 

db.Enoxacin.name = 'Enoxacin'; 
db.Enoxacin.mode = 'AB'; 
db.Enoxacin.pKa = [6.1 8.2];
db.Enoxacin.logPow = -0.4;
db.Enoxacin.rat.fuP = 0.77;
db.Enoxacin.rat.BP = 0.92; 

db.Lomefloxacin.name = 'Lomefloxacin'; 
db.Lomefloxacin.mode = 'AB'; 
db.Lomefloxacin.pKa = [6.1 8.2];
db.Lomefloxacin.logPow = -0.4;
db.Lomefloxacin.rat.fuP = 0.77;
db.Lomefloxacin.rat.BP = 0.92; 

db.Pefloxacin.name = 'Pefloxacin'; 
db.Pefloxacin.mode = 'AB'; 
db.Pefloxacin.pKa = [6.1 8.2];
db.Pefloxacin.logPow = -0.4;
db.Pefloxacin.rat.fuP = 0.77;
db.Pefloxacin.rat.BP = 0.92; 

db.PipemidicAcid.name = 'PipemidicAcid'; 
db.PipemidicAcid.mode = 'AB'; 
db.PipemidicAcid.pKa = [6.1 8.2];
db.PipemidicAcid.logPow = -0.4;
db.PipemidicAcid.rat.fuP = 0.77;
db.PipemidicAcid.rat.BP = 0.92; 

db.Tetracycline.name = 'Tetracycline'; 
db.Tetracycline.mode = 'AB'; 
db.Tetracycline.pKa = [6.1 8.2];
db.Tetracycline.logPow = -0.4;
db.Tetracycline.rat.fuP = 0.77;
db.Tetracycline.rat.BP = 0.92; 

db.Ceftazidime.name = 'Ceftazidime'; 
db.Ceftazidime.mode = 'AB'; 
db.Ceftazidime.pKa = [6.1 8.2];
db.Ceftazidime.logPow = -0.4;
db.Ceftazidime.rat.fuP = 0.77;
db.Ceftazidime.rat.BP = 0.92; 

db.NalidixicAcid.name = 'NalidixicAcid'; 
db.NalidixicAcid.mode = 'AwB'; 
db.NalidixicAcid.pKa = [6.1 8.2];
db.NalidixicAcid.logPow = -0.4;
db.NalidixicAcid.rat.fuP = 0.77;
db.NalidixicAcid.rat.BP = 0.92; 

db.Clonazepam.name = 'Clonazepam';
db.Clonazepam.mode = 'AwB';
db.Clonazepam.MW = 315.17;
db.Clonazepam.pKa = [1.9 2.53 3.75];
db.Clonazepam.logPow = 2.41;
db.Clonazepam.human.fuP = 0.21;
db.Clonazepam.human.BP = 1.4;
db.Clonazepam.human.CLblood_kgBW.hep = NaN; % to be transformed: 3.4; % ml/min !

db.Lorazepam.name = 'Lorazepam';
db.Lorazepam.mode = 'AwB';
db.Lorazepam.MW = 321.16;
db.Lorazepam.pKa = [1.3 12.25];
db.Lorazepam.logPow = 2.45;
db.Lorazepam.human.fuP = 0.09;
db.Lorazepam.human.BP = 1;
db.Lorazepam.human.CLblood_kgBW.hep = NaN; % to be transformed: 1.1./1000;

db.Oxazepam.name = 'Oxazepam';
db.Oxazepam.mode = 'AwB';
db.Oxazepam.MW = 286.71;
db.Oxazepam.pKa = [2.03 11.6];
db.Oxazepam.logPow = 2.24;
db.Oxazepam.human.fuP = 0.05;
db.Oxazepam.human.BP = 1.11;
db.Oxazepam.human.CLblood_kgBW.hep = NaN; % to be transformed: 1.1./1000;

db.Tenoxicam.name = 'Tenoxicam';
db.Tenoxicam.mode = 'AwB';
db.Tenoxicam.MW = 337.37;
db.Tenoxicam.pKa = [1.01 5.3];
db.Tenoxicam.logPow = 1.9;
db.Tenoxicam.human.fuP = 0.009;
db.Tenoxicam.human.BP = 0.67;
db.Tenoxicam.human.CLblood_kgBW.hep = NaN; % to be transformed: 0.03./1000;

% ----- di-bases ------

db.Chlorpromazine.name = 'Chlorpromazine';
db.Chlorpromazine.mode = 'BB';
db.Chlorpromazine.MW = 318.86;
db.Chlorpromazine.pKa = [6.4 9.7];
db.Chlorpromazine.logPow = .42;
db.Chlorpromazine.human.fuP = 0.036;
db.Chlorpromazine.human.BP = 1.17;
db.Chlorpromazine.human.CLblood_kgBW.hep = NaN; % to be transformed: 11/1000; % Obach99

db.Clozapine.name = 'Clozapine';
db.Clozapine.mode = 'BB';
db.Clozapine.MW = 326.82;
db.Clozapine.pKa = [3.7 7.7];
db.Clozapine.logPow = 3.42;
db.Clozapine.human.fuP = 0.05;
db.Clozapine.human.BP = 0.86;
db.Clozapine.human.CLblood_kgBW.hep = NaN; % to be transformed: 2.9/1000; % Obach99

db.Ethambutol.name = 'Ethambutol';
db.Ethambutol.mode = 'BB';
db.Ethambutol.MW = 204.31;
db.Ethambutol.pKa = [6.3 9.5];
db.Ethambutol.logPow = -0.4;
db.Ethambutol.human.fuP = 0.84;
db.Ethambutol.human.BP = 1.33;

db.Promethazine.name = 'Promethazine';
db.Promethazine.mode = 'BB';
db.Promethazine.MW = 284.42;
db.Promethazine.pKa = [6.5 9.6];
db.Promethazine.logPow = 4.96;
db.Promethazine.human.fuP = 0.16;
db.Promethazine.human.BP = 1.5;
db.Promethazine.human.CLblood_kgBW.hep = NaN; % to be transformed: 16/1000;

db.Quinidine.name = 'Quinidine';
db.Quinidine.mode = 'BB';
db.Quinidine.MW = 324.42;
db.Quinidine.pKa = [5.4 10];
db.Quinidine.logPow = 3.44;
db.Quinidine.human.fuP = 0.17;
db.Quinidine.human.BP = 0.96;
db.Quinidine.human.CLblood_kgBW.hep = NaN; % to be transformed: 2.7/1000; % Obach99

db.Sematilide.name = 'Sematilide';
db.Sematilide.mode = 'BB';
db.Sematilide.pKa = [7.6 9.5];
db.Sematilide.logPow = 1.36;
db.Sematilide.human.fuP = 0.96;
db.Sematilide.human.BP = 1.09;
db.Sematilide.human.CLblood_kgBW.hep = NaN; % to be transformed: 3.7/1000;

db.Sotalol.name = 'Sotalol';
db.Sotalol.mode = 'BB';
db.Sotalol.MW = 272.36;
db.Sotalol.pKa = [8.25 9.8];
db.Sotalol.logPow = 0.53;
db.Sotalol.human.fuP = 1;
db.Sotalol.human.BP = 1.07;
db.Sotalol.human.CLblood_kgBW.hep = NaN; % to be transformed: 1.8/1000; % 1.5-2.1

db.Sulpiride.name = 'Sulpiride';
db.Sulpiride.mode = 'BB';
db.Sulpiride.MW = 341.43;
db.Sulpiride.pKa = [9.01 10.19];
db.Sulpiride.logPow = 0.57;
db.Sulpiride.human.fuP = 1;
db.Sulpiride.human.BP = 1;
db.Sulpiride.human.CLblood_kgBW.hep = NaN; % to be transformed: 1.5/1000;

db.Tiapamil.name = 'Tiapamil';
db.Tiapamil.mode = 'BB';
db.Tiapamil.pKa = [7 9];
db.Tiapamil.logPow = 2.6;
db.Tiapamil.human.fuP = 0.33;
db.Tiapamil.human.BP = 1;

db.Timolol.name = 'Timolol';
db.Timolol.mode = 'BB';
db.Timolol.MW = 316.42;
db.Timolol.pKa = [8.8 9.21];
db.Timolol.logPow = 1.91;
db.Timolol.human.fuP = 0.4;
db.Timolol.human.BP = 0.81;
db.Timolol.human.CLblood_kgBW.hep = NaN; % to be transformed: 9.17/1000;

db.Trimethoprim.name = 'Trimethoprim';
db.Trimethoprim.mode = 'BB';
db.Trimethoprim.MW = 290.32;
db.Trimethoprim.pKa = [6.6 7.16];
db.Trimethoprim.logPow = 0.91;
db.Trimethoprim.human.fuP = 0.48;
db.Trimethoprim.human.BP = 1.28;


% --- virtual drug for testing purposes---------------------------------

db.drugA.name = 'drugX';
db.drugA.mode = 'B'; 
db.drugA.MW = 500;
db.drugA.pKa = 9;
db.drugA.logPow = 2.5;
db.drugA.human.fuP = 1;
db.drugA.human.BP = 1.1;
db.drugA.human.CLblood_kgBW.hep = NaN; 
db.drugA.human.E.feces   = 1;
db.drugA.human.lambda_po   = 0.05;



%%%%%%%%%%%%%%%% assign values for the model drug %%%%%%%%%%%%%%%%%%%%%%%%

drug = db.(compound); drug.unit = db.unit;

% renal blood clearance not implemented yet.
if db.(compound).human.CLblood_kgBW.ren ~= 0
    error('Renal clearance not yet implemented due to ');
end;


