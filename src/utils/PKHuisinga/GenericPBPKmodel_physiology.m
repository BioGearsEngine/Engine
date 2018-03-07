%%%
%%% Version: August 31st, 2012
%%% 
%%% Check at URL: http://www.pharmacometrics.de
%%% for potential updates and comments
%%%
%%% This function specifies the physiological data for humans of 
%%% different age and sex. 
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

function [species,T] = GenericPBPKmodel_physiology(speciestype) 

fprintf('\n\n   Loading: physiology data')

%%% ========================================================================================================
%%% Indexing of organs, tissues and other spaces
%%%
lun =  1; tissue.lun = lun; tissue.name{lun} = 'lun';
art =  2; tissue.art = art; tissue.name{art} = 'art';
bra =  3; tissue.bra = bra; tissue.name{bra} = 'bra';
adi =  4; tissue.adi = adi; tissue.name{adi} = 'adi';
hea =  5; tissue.hea = hea; tissue.name{hea} = 'hea';
kid =  6; tissue.kid = kid; tissue.name{kid} = 'kid';
mus =  7; tissue.mus = mus; tissue.name{mus} = 'mus';
bon =  8; tissue.bon = bon; tissue.name{bon} = 'bon';
ski =  9; tissue.ski = ski; tissue.name{ski} = 'ski';
gut = 10; tissue.gut = gut; tissue.name{gut} = 'gut';
spl = 11; tissue.spl = spl; tissue.name{spl} = 'spl';
liv = 12; tissue.liv = liv; tissue.name{liv} = 'liv';
ven = 13; tissue.ven = ven; tissue.name{ven} = 'ven';
rob = 14; tissue.rob = rob; tissue.name{rob} = 'rob';
blo = 15; tissue.blo = blo; tissue.name{blo} = 'blo';
pla = 16; tissue.pla = pla; tissue.name{pla} = 'pla';
ery = 17; tissue.ery = ery; tissue.name{ery} = 'ery';

tissue.allTis       = [lun art bra adi hea kid mus bon ski gut spl liv ven];
tissue.allTisAndRob = [1:14];
tissue.numberOfTis  = length(tissue.allTis);

tissue.allTisExVen  = [lun art bra adi hea kid mus bon ski gut spl liv    ];
tissue.allTisExBlo  = [lun     bra adi hea kid mus bon ski gut spl liv    ];
tissue.allTisExAdi  = [lun art bra     hea kid mus bon ski gut spl liv ven];
tissue.intoVen      = [        bra adi hea kid mus bon ski         liv    ];
tissue.intoLiv      = [    art                             gut spl        ];
tissue.V1           = [lun art bra     hea kid             gut spl liv ven];
tissue.V2           = [            adi         mus bon ski                   ];

tissue.allTisAlphabetic  = [adi art bon bra gut hea kid liv lun mus ski spl ven];
tissue.allTisSVtisScaled = [lun art         hea kid mus bon     gut spl liv ven];

initialize = NaN*ones(1,length(tissue.name));
tissue.initialize = initialize;



%%% ========================================================================================================
%%% == human = human = human = human = = human = human = human = human = = human = human = human = human ===                           

units = 'weight = kg; volume = L; time = min; BH = cm; BSA = m2; ';

%%% ========================================================================================================
%%% Model: Age and sex
%%%
%%% Children:
%%% newborn (nb), age 1 (age1u), 5 (age5u), 10 (age10u) , all uni sex
%%% age 15 male (age15m), age 15 female (age15f), 
%%% Adults:
%%% age 20-50 male (age35m), age 20-50 female (age35f)
%%% As in the source, we associate an average age of 35 with the adult
%%% 
%%% Source: ICRP report
%%%
newborn = 1; age1 = 2; age5 = 3; age10 = 4; age15m = 5; age15f = 6; age35m = 7; age35f = 8;
human.type   = 'human';
human.AgeSexClasses   = {'newborn','age1','age5','age10','age15m','age15f','age35m','age35f'};
human.allAgeClasses   = [1:8];
human.childrenAgeClasses = [1:6];


%%% ========================================================================================================
%%% Data  : Body weight (BW), body height (BH) and body surface area (BSA)
%%%
%%% Units : BW in kg, BH in cm, BSA in m^2
%%% Source: ICRP report (Table 2.9)
%%%
%%%                 nb   age1    age5   age10  age15m  age15f  age35m   age35f
ICRP.BW  =      [ 3.50  10.00   19.00   32.00   56.00   53.00   73.00   60.00];
ICRP.BH  =      [51.00  76.00  109.00  138.00  167.00  161.00  176.00  163.00];
ICRP.BSA =      [ 0.24   0.48    0.78    1.12    1.62    1.55    1.90    1.66];
ICRP.sex =      {'uni'  'uni'   'uni'   'uni'  'male' 'female' 'male' 'female'};
ICRP.age =      [    0      1       5      10     15       15      35      35];

%%% Assign values to all age/sex classes
for a = human.allAgeClasses   
    age = char(human.AgeSexClasses(a));
    human.(age).type   = 'human';
    human.(age).age    = ICRP.age(a);    human.(age).unit.age = 'years';
    human.(age).sex    = ICRP.sex{a};
    human.(age).tissue = tissue;
    human.(age).BW     = ICRP.BW(a);     human.(age).unit.BW = 'kg';
    human.(age).BH     = ICRP.BH(a)/100; human.(age).unit.BH = 'm'; % change of units: cm -> m
    human.(age).BSA    = ICRP.BSA(a);    human.(age).unit.BSA = 'm^2';
    human.(age).BMI    = human.(age).BW/human.(age).BH^2; human.(age).unit.BMI = 'kg/m^2';
     
    human.(age).tis    = tissue.name;
end;


%%% ========================================================================================================
%%% Data  : Organ weight, density and volume 
%%%
%%% Unit  : weight in g, density in g/l, volume in l
%%% Source: ICRP report (Table 2.8)
%%% 
%%%                                              nb   age1   age5  age10  age15m  age15f  age35m   age35f  
ICRP.OW.adipose_tissue =                       [930   3800   5500   8600   12000   18700   18200   22500]; % entry duplicates other mass information
ICRP.OW.separable_adipose =                    [890   3600   5000   7500    9500   16000   14500   19000];
ICRP.OW.adrenals =                             [  6      4      5      7      10       9      14      13];
% alimentary system
ICRP.OW.tongue =                               [  3.5   10     19     32      56      53      73      60];
ICRP.OW.salivary_glands =                      [  6     24     34     44      68      65      85      70];
ICRP.OW.oesophagus_wall =                      [  2      5     10     18      30      30      40      35];
ICRP.OW.stomach_wall =                         [  7     20     50     85     120     120     150     140];
ICRP.OW.stomach_contents =                     [ 40     67     83    117     200     200     250     230];
ICRP.OW.small_intestine_wall =                 [ 30     85    220    370     520     520     650     600];
ICRP.OW.small_intestine_contents =             [ 56     93    117    163     280     280     350     280];
ICRP.OW.large_intestine_right_colon_wall =     [  7     20     49     85     122     122     150     145];
ICRP.OW.large_intestine_right_colon_contents = [ 24     40     50     70     120     120     150     160];
ICRP.OW.large_intestine_left_colon_wall =      [  7     20     49     85     122     122     150     145];
ICRP.OW.large_intestine_left_colon_contents =  [ 12     20     25     35      60      60      75      80];
ICRP.OW.large_intestine_rectosigmoid_wall =    [  3     10     22     40      56      56      70      70];
ICRP.OW.large_intestine_rectosigmoid_content = [ 12     20     25     35      60      60      75      80];
ICRP.OW.liver =                                [130    330    570    830    1300    1300    1800    1400];
ICRP.OW.gallbladder_wall =                     [  0.5    1.4    2.6    4.4     7.7     7.3    10       8];
ICRP.OW.gallbladder_content =                  [  2.8    8     15     26      45      42      58      48];
ICRP.OW.pancreas =                             [  6     20     35     60     110     100     140     120];

%%% Note: brain organ weight age5  = 1245 = mean value of 1310 (male) and 1180 (female) 
%%%       brain organ weight age10 = 1310 = mean value of 1400 (male) and 1220 (female) 
ICRP.OW.brain =                                [380    950   1245   1310    1420    1300    1450    1300]; 
ICRP.OW.breasts =                              [NaN    NaN    NaN    NaN      15     250      25     500];
% circulatory system
ICRP.OW.heart_with_blood =                     [ 46     98    220    370     660     540     840     620]; % entry duplicates other mass information
ICRP.OW.heart_tissue_only =                    [ 20     50     85    140     230     220     330     250];
% Table sec 7.4
ICRP.OW.total_blood =                          [270    500   1400   2400    4500    3300    5300    3900];
ICRP.OW.eyes =                                 [  6      7     11     12      13      13      15      15];
ICRP.OW.storage_fat =                          [370   2300   3600   6000    9000   14000   14600   18000]; % entry duplicates other mass information
ICRP.OW.skin =                                 [175    350    570    820    2000    1700    3300    2300];
ICRP.OW.skeletal_muscle =                      [800   1900   5600  11000   24000   17000   29000   17500];
ICRP.OW.pituitary_gland =                      [  0.1   0.15   0.25   0.35     0.5     0.5     0.6     0.6];
% Respiratory system
ICRP.OW.larynx =                               [  1.3    4      7     12      22      15      28      19];
ICRP.OW.trachea =                              [  0.5    1.5    2.5    4.5     7.5     6      10       8];
ICRP.OW.lung_with_blood =                      [ 60    150    300    500     900     750    1200     950]; % entry duplicates other mass information
ICRP.OW.lung_tissue_only =                     [ 30     80    125    210     330     290     500     420];
% skeletal system
ICRP.OW.total_skeleton =                       [370   1170   2430   4500    7950    7180   10500    7800]; % entry duplicates other mass information
ICRP.OW.bone_cortical =                        [135    470   1010   1840    3240    2960    4400    3200];
ICRP.OW.bone_trabecular =                      [ 35    120    250    460     810     740    1100     800];
% bone_total = bone_cortical + bone_trabecular
ICRP.OW.bone_total =                           [170    590   1260   2300    4050    3700    5500    4000]; % entry duplicates other mass information
ICRP.OW.marrow_active =                        [ 50    150    340    630    1080    1000    1170     900];
ICRP.OW.marrow_inactive =                      [  0     20    160    630    1480    1380    2480    1800];
ICRP.OW.cartilage =                            [130    360    600    820    1140     920    1100     900];
ICRP.OW.teeth =                                [  0.7    5     15     30      45      35      50      40];
ICRP.OW.skeleton_miscellaneous =               [ 20     45     55     90     155     145     200     160];
ICRP.OW.spleen =                               [  9.5   29     50     80     130     130     150     130];


%%% Note: thymus organ weight age10 = 37.5 = mean value of 40 (male) and 35 (female) 
ICRP.OW.thymus =                               [ 13     30     30     37.5    35      30      25      20];
ICRP.OW.thyroid =                              [  1.3    1.8    3.4    7.9    12      12      20      17];
ICRP.OW.tonsils =                              [  0.1    0.5    2      3       3       3        3      3];
% Urogenital system
ICRP.OW.kidneys =                              [ 25     70    110    180     250     240     310     275];
ICRP.OW.ureters =                              [  0.77   2.2    4.2    7.0    12      12      16      15];
ICRP.OW.urinary_bladder =                      [  4      9     16     25      40      35      50      40];

%%% Note: urethra organ weight nb    = 0.31 = mean value of 0.48 (male) and 0.14 (female) 
%%%       urethra organ weight age1  = 0.91 = mean value of 1.4  (male) and 0.42 (female) 
%%%       urethra organ weight age5  = 1.69 = mean value of 2.6  (male) and 0.78 (female) 
%%%       urethra organ weight age10 = 2.85 = mean value of 4.4  (male) and 1.3  (female) 
ICRP.OW.urethra =                              [  0.31   0.91   1.69   2.85    7.7     2.3    10       3];
ICRP.OW.testes =                               [  0.85   1.5    1.7    2      16       0      35       0];
ICRP.OW.epididymes =                           [  0.25   0.35   0.45   0.6     1.6     0       4       0];
ICRP.OW.prostate =                             [  0.8    1.0    1.2    1.6     4.3     0      17       0];
ICRP.OW.ovaries =                              [  0.3    0.8    2.0    3.5     0       6       0      11];
ICRP.OW.fallopian_tubes =                      [  0.25   0.25   0.35   0.50    0       1.1     0     2.1];
ICRP.OW.uterus =                               [  4.0    1.5    3      4       0      30       0      80];


%%% ========================================================================================================
%%% Data  : Regional blood volumes as percentage of total blood volume
%%%
%%% Unit  : percent (%)
%%% Source: ICRP report (Table 2.14)
%%%
%%%                                              nb   age1   age5  age10  age15m  age15f  age35m   age35f  
ICRP.fVblood.fat =                             [NaN    NaN    NaN    NaN     NaN     NaN    5.00    8.50];
ICRP.fVblood.brain =                           [NaN    NaN    NaN    NaN     NaN     NaN    1.20    1.20];
ICRP.fVblood.stomach_and_oesophages =          [NaN    NaN    NaN    NaN     NaN     NaN    1.00    1.00];
ICRP.fVblood.small_intestine =                 [NaN    NaN    NaN    NaN     NaN     NaN    3.80    3.80];
ICRP.fVblood.large_intestine =                 [NaN    NaN    NaN    NaN     NaN     NaN    2.20    2.20];
ICRP.fVblood.right_heart =                     [NaN    NaN    NaN    NaN     NaN     NaN    4.50    4.50];
ICRP.fVblood.left_heart =                      [NaN    NaN    NaN    NaN     NaN     NaN    4.50    4.50];
ICRP.fVblood.coronary_tissue =                 [NaN    NaN    NaN    NaN     NaN     NaN    1.00    1.00];
ICRP.fVblood.kidneys =                         [NaN    NaN    NaN    NaN     NaN     NaN    2.00    2.00];
ICRP.fVblood.liver =                           [NaN    NaN    NaN    NaN     NaN     NaN   10.00   10.00];
ICRP.fVblood.bronchial_tissue =                [NaN    NaN    NaN    NaN     NaN     NaN    2.00    2.00];
ICRP.fVblood.skeletal_muscle =                 [NaN    NaN    NaN    NaN     NaN     NaN   14.00   10.50];
ICRP.fVblood.pancreas =                        [NaN    NaN    NaN    NaN     NaN     NaN    0.60    0.60];
ICRP.fVblood.skeleton.total =                  [NaN    NaN    NaN    NaN     NaN     NaN    7.00    7.00];
ICRP.fVblood.skeleton.red_marrow =             [NaN    NaN    NaN    NaN     NaN     NaN    4.00    4.00]; % entry duplicates other information
ICRP.fVblood.skeleton.trabecular_bone =        [NaN    NaN    NaN    NaN     NaN     NaN    1.20    1.20]; % entry duplicates other information
ICRP.fVblood.skeleton.cortical_bone =          [NaN    NaN    NaN    NaN     NaN     NaN    0.80    0.80]; % entry duplicates other information
ICRP.fVblood.skeleton.others =                 [NaN    NaN    NaN    NaN     NaN     NaN    1.00    1.00]; % entry duplicates other information
ICRP.fVblood.skin =                            [NaN    NaN    NaN    NaN     NaN     NaN    3.00    3.00];
ICRP.fVblood.spleen =                          [NaN    NaN    NaN    NaN     NaN     NaN    1.40    1.40];
ICRP.fVblood.thyroid =                         [NaN    NaN    NaN    NaN     NaN     NaN    0.06    0.06];
ICRP.fVblood.lymph_nodes =                     [NaN    NaN    NaN    NaN     NaN     NaN    0.20    0.20];
ICRP.fVblood.gonads =                          [NaN    NaN    NaN    NaN     NaN     NaN    0.04    0.02];
ICRP.fVblood.adrenals =                        [NaN    NaN    NaN    NaN     NaN     NaN    0.06    0.06];
ICRP.fVblood.urinary_bladder =                 [NaN    NaN    NaN    NaN     NaN     NaN    0.02    0.02];
ICRP.fVblood.all_other_tissues =               [NaN    NaN    NaN    NaN     NaN     NaN    1.92    1.92];


%%% ========================================================================================================
%%% Data  : Volume of blood plasma and red blood cells
%%%
%%% Unit  : ml
%%% Source: ICRP report (Table 2.12)
%%%                                              nb   age1   age5  age10  age15m  age15f  age35m   age35f
ICRP.erythrocyte_volume =                      [NaN    NaN    NaN    NaN     NaN     NaN    2300    1500];
ICRP.plasma_volume =                           [NaN    NaN    NaN    NaN     NaN     NaN    3000    2400];


%%% ========================================================================================================
%%% Data  : Distribution of blood in the vascular system
%%%
%%% Unit  : Percentage (%) of total blood volume
%%% Source: ICRP report (Table 2.13) 
%%%                                              nb   age1   age5  age10  age15m  age15f  age35m   age35f
ICRP.fVblood.heart_chambers =                  [NaN    NaN    NaN    NaN     NaN     NaN    9.00     NaN];
ICRP.fVblood.pulmonary.total =                 [NaN    NaN    NaN    NaN     NaN     NaN   10.50     NaN];
ICRP.fVblood.pulmonary.arteries =              [NaN    NaN    NaN    NaN     NaN     NaN    3.00     NaN];
ICRP.fVblood.pulmonary.capillaries =           [NaN    NaN    NaN    NaN     NaN     NaN    2.00     NaN];
ICRP.fVblood.pulmonary.veins =                 [NaN    NaN    NaN    NaN     NaN     NaN    5.50     NaN];
ICRP.fVblood.systemic.total =                  [NaN    NaN    NaN    NaN     NaN     NaN   80.50     NaN];
ICRP.fVblood.systemic.aorta_large_arteries =   [NaN    NaN    NaN    NaN     NaN     NaN    6.00     NaN];
ICRP.fVblood.systemic.small_arteries =         [NaN    NaN    NaN    NaN     NaN     NaN   10.00     NaN];
ICRP.fVblood.systemic.capillaries =            [NaN    NaN    NaN    NaN     NaN     NaN    5.00     NaN];
ICRP.fVblood.systemic.small_veins =            [NaN    NaN    NaN    NaN     NaN     NaN   41.50     NaN];
ICRP.fVblood.systemic.large_veins =            [NaN    NaN    NaN    NaN     NaN     NaN   18.00     NaN];


%%% ========================================================================================================
%%% Data  : Density of tissue
%%%
%%% Unit  : kg/l
%%% Source: Brown 1997 and ICRP report 
%%% 
human.OrganDensity      = ones(size(initialize)); % all organs except for adi and bon
human.OrganDensity(adi) = 0.92;                   % Brown, 1997
human.OrganDensity(bon) = 1.3;                    % ICRP report


%%% ========================================================================================================
%%% Model: Vascular tissue volumes (fraction vascular blood = fvB
%%%
%%% Unit : fraction of total blood
%%%
for a = human.allAgeClasses
    
    age = char(human.AgeSexClasses(a));
    human.(age).fvB      = initialize;
    human.(age).fvB(adi) = ICRP.fVblood.fat(a);
    human.(age).fvB(bon) = ICRP.fVblood.skeleton.total(a);
    human.(age).fvB(bra) = ICRP.fVblood.brain(a);
    human.(age).fvB(gut) = ICRP.fVblood.small_intestine(a)+ICRP.fVblood.large_intestine(a);
    human.(age).fvB(hea) = ICRP.fVblood.coronary_tissue(a);
    human.(age).fvB(kid) = ICRP.fVblood.kidneys(a);
    human.(age).fvB(liv) = ICRP.fVblood.liver(a);
    human.(age).fvB(lun) = ICRP.fVblood.bronchial_tissue(a);
    human.(age).fvB(mus) = ICRP.fVblood.skeletal_muscle(a);
    human.(age).fvB(ski) = ICRP.fVblood.skin(a);
    human.(age).fvB(spl) = ICRP.fVblood.spleen(a);
    human.(age).fvB(blo) = 100 - sum(human.(age).fvB(tissue.allTisExBlo));
    human.(age).fvB(art) = (0.5*ICRP.fVblood.heart_chambers(a) +...
                           ICRP.fVblood.pulmonary.veins(a) + ...
                           0.5*ICRP.fVblood.pulmonary.capillaries(a) +...
                           ICRP.fVblood.systemic.aorta_large_arteries(a) + ...
                           0.5*ICRP.fVblood.systemic.capillaries(a) + ...
                           ICRP.fVblood.systemic.small_arteries(a))/100*human.(age).fvB(blo);
    human.(age).fvB(ven) = (0.5*ICRP.fVblood.heart_chambers(a) + ...
                           ICRP.fVblood.pulmonary.arteries(a) +...
                           0.5*ICRP.fVblood.pulmonary.capillaries(a) + ...
                           0.5*ICRP.fVblood.systemic.capillaries(a) +...
                           ICRP.fVblood.systemic.small_veins(a) + ...
                           ICRP.fVblood.systemic.large_veins(a))/100*human.(age).fvB(blo);
                       
    human.(age).fvB = human.(age).fvB/100; % change of units: % -> fraction
    human.(age).unit.fvB = 'fraction';
    
end;
%%%
%%% DUE TO LACK OF DATA: 
%%%
%%% For children age classes: age35m data were adopted for all fvB entries
%%% except for age15fm, where age35f data were taken
%%% For age35f:age35m data were adopted for fvB values of art and ven 
%%%
human.('age35f').fvB([art ven]) = human.('age35m').fvB([art ven])./human.('age35m').fvB(blo)*...
                                  human.('age35f').fvB(blo);
human.('newborn').fvB = human.('age35m').fvB;
human.('age1').fvB    = human.('age35m').fvB;
human.('age5').fvB    = human.('age35m').fvB;
human.('age10').fvB   = human.('age35m').fvB;
human.('age15m').fvB  = human.('age35m').fvB;
human.('age15f').fvB  = human.('age35f').fvB;


%%% ========================================================================================================
%%% Model: Organ weights (OW) and tissue volumes (V)
%%%
%%% Unit : OW in kg and V in L 
%%% NOTE : total blood V(blo) is partitioned into artery blood V(art) and
%%% venous blood V(ven) according to fvB(art)/fvB(blo) and
%%% fvB(ven)/fvB(blo). I.e., all vascular spaces fvB(tis) are partitioned
%%% into an artery and venous part according to fvB(art)/fvB(blo) and
%%% fvB(ven)/fvB(blo)
%%%
for a = human.allAgeClasses
    
    age = char(human.AgeSexClasses(a));
    human.(age).OW      = initialize; human.(age).unit.OW = 'kg';
    human.(age).OW(adi) = ICRP.OW.separable_adipose(a);
    human.(age).OW(bon) = ICRP.OW.total_skeleton(a); % includes total bone and marrow
    human.(age).OW(bra) = ICRP.OW.brain(a);
    human.(age).OW(gut) = ICRP.OW.small_intestine_wall(a) + ICRP.OW.large_intestine_right_colon_wall(a) + ...
                          ICRP.OW.large_intestine_left_colon_wall(a) + ICRP.OW.large_intestine_rectosigmoid_wall(a);
    human.(age).OW(hea) = ICRP.OW.heart_tissue_only(a);
    human.(age).OW(kid) = ICRP.OW.kidneys(a);
    human.(age).OW(liv) = ICRP.OW.liver(a);
    human.(age).OW(lun) = ICRP.OW.lung_tissue_only(a);
    human.(age).OW(mus) = ICRP.OW.skeletal_muscle(a);
    human.(age).OW(ski) = ICRP.OW.skin(a);
    human.(age).OW(spl) = ICRP.OW.spleen(a);
    human.(age).OW(blo) = ICRP.OW.total_blood(a);
    human.(age).OW(art) = (human.(age).fvB(art)/human.(age).fvB(blo)) *human.(age).OW(blo);
    human.(age).OW(ven) = (human.(age).fvB(ven)/human.(age).fvB(blo)) *human.(age).OW(blo);
    
    human.(age).OW      = human.(age).OW/1000; % change of units: g -> kg
    
    human.(age).OW(rob) = human.(age).BW - sum(human.(age).OW(tissue.allTis));  
    human.(age).LBW     = human.(age).BW - human.(age).OW(adi);  human.(age).unit.LBW = 'kg';

    human.(age).V       = human.(age).OW./human.OrganDensity; human.(age).unit.V = 'L'; % organ weights -> volumes

    human.(age).V(ery)  = ICRP.erythrocyte_volume(a)/1000;                       % change of units: ml -> l
    human.(age).V(pla)  = ICRP.plasma_volume(a)/1000;                            % change of units: ml -> l
    human.(age).hct     = human.(age).V(ery)/human.(age).V(blo); human.(age).unit.hct = 'unitless';
    
end;
%%%
%%% DUE TO LACK OF DATA: 
%%%
%%% For all children age classes: age35m data were adopted for
%%% hematocrit value hct
%%%
%%% Age 15, male and age 10, 5, 1, newborn
for a = {'newborn','age1','age5','age10','age15f'}
    age = char(a);
    hct = human.('age35f').hct;      % set identical to hct of female adult
    human.(age).hct    = hct;        % in line with NHANES study (for age 5 and older)
    human.(age).V(ery) = hct*human.(age).V(blo);
    human.(age).V(pla) = (1-hct)*human.(age).V(blo);
end;
%%% Age 15, male
hct = human.('age35m').hct;        % set identical to hct of male adult
human.('age15m').hct    = hct;     % in line with NHANES study
human.('age15m').V(ery) = hct*human.(age).V(blo);
human.('age15m').V(pla) = (1-hct)*human.(age).V(blo);


%%% ========================================================================================================
%%% Data  : Cardiac output
%%%
%%% Unit  : L/min
%%% Source: ICRP report (Table 2.39)
%%%                                              nb   age1   age5  age10  age15m  age15f  age35m   age35f
ICRP.co  =                                     [NaN    NaN    3.4    5.0     6.1     6.1     6.5     5.9];
ICRP.co([newborn age1]) =                      [44     109]/60; % Alverson et al 1987 (cited in Abraham et al 2005)
 

%%% ========================================================================================================
%%% Data  : blood flow rates
%%%
%%% Unit  : percentage (%) of cardiac output
%%% Source: ICRP report (Table 2.40)
%%%                                              nb   age1   age5  age10  age15m  age15f  age35m   age35f
ICRP.fQco.fat =                                [NaN    NaN    NaN    NaN     NaN     NaN    5.00    8.50];
ICRP.fQco.brain =                              [NaN    NaN    NaN    NaN     NaN     NaN   12.00   12.00];
ICRP.fQco.stomach_and_oesophagus =             [NaN    NaN    NaN    NaN     NaN     NaN    1.00    1.00]; % not used in PBPK model
ICRP.fQco.small_intestine =                    [NaN    NaN    NaN    NaN     NaN     NaN   10.00   11.00];
ICRP.fQco.large_intestine =                    [NaN    NaN    NaN    NaN     NaN     NaN    4.00    5.00];
ICRP.fQco.coronary_tissue =                    [NaN    NaN    NaN    NaN     NaN     NaN    4.00    5.00];
ICRP.fQco.kidney =                             [NaN    NaN    NaN    NaN     NaN     NaN   19.00   17.00];
ICRP.fQco.liver =                              [NaN    NaN    NaN    NaN     NaN     NaN   25.50   27.00]; % total, arterial = [6.5 6.5]
ICRP.fQco.bronchial_tissue =                   [NaN    NaN    NaN    NaN     NaN     NaN    2.50    2.50]; % not used in PBPK model
ICRP.fQco.skeletal_muscle =                    [NaN    NaN    NaN    NaN     NaN     NaN   17.00   12.00];
ICRP.fQco.pancreas =                           [NaN    NaN    NaN    NaN     NaN     NaN    1.00    1.00]; % not used in PBPK model
ICRP.fQco.skeleton.total =                     [NaN    NaN    NaN    NaN     NaN     NaN    5.00    5.00];
ICRP.fQco.skeleton.red_marrow =                [NaN    NaN    NaN    NaN     NaN     NaN    3.00    3.00];
ICRP.fQco.skeleton.trabecular_bone =           [NaN    NaN    NaN    NaN     NaN     NaN    0.90    0.90];
ICRP.fQco.skeleton.cortical_bone =             [NaN    NaN    NaN    NaN     NaN     NaN    0.60    0.60];
ICRP.fQco.skeleton.others =                    [NaN    NaN    NaN    NaN     NaN     NaN    0.50    0.50];
ICRP.fQco.skin =                               [NaN    NaN    NaN    NaN     NaN     NaN    5.00    5.00];
ICRP.fQco.spleen =                             [NaN    NaN    NaN    NaN     NaN     NaN    3.00    3.00];
ICRP.fQco.thyroid =                            [NaN    NaN    NaN    NaN     NaN     NaN    1.50    1.50]; % not used in PBPK model
ICRP.fQco.lymph_nodes =                        [NaN    NaN    NaN    NaN     NaN     NaN    1.70    1.70]; % not used in PBPK model
ICRP.fQco.gonads =                             [NaN    NaN    NaN    NaN     NaN     NaN    0.05    0.02]; % not used in PBPK model
ICRP.fQco.adrenals =                           [NaN    NaN    NaN    NaN     NaN     NaN    0.30    0.30]; % not used in PBPK model
ICRP.fQco.urinary_bladder =                    [NaN    NaN    NaN    NaN     NaN     NaN    0.06    0.06]; % not used in PBPK model
ICRP.fQco.all_other_tissues =                  [NaN    NaN    NaN    NaN     NaN     NaN    1.39    1.92]; % not used in PBPK model 


%%% ========================================================================================================
%%% Model: Tissue blood flow (fraction of cardiac output and absolut values)
%%%
%%% Unit : fraction and L/min
%%% 
%%% 
for a = human.allAgeClasses
    
    age = char(human.AgeSexClasses(a));
    human.(age).fQco      = initialize; human.(age).unit.fQco = 'unitless';
    human.(age).Q         = initialize; human.(age).unit.Q = 'L/min';
    human.(age).fQco(adi) = ICRP.fQco.fat(a);
    human.(age).fQco(bon) = ICRP.fQco.skeleton.total(a);
    human.(age).fQco(bra) = ICRP.fQco.brain(a);
    human.(age).fQco(gut) = ICRP.fQco.small_intestine(a)+ICRP.fQco.large_intestine(a);
    human.(age).fQco(hea) = ICRP.fQco.coronary_tissue(a);
    human.(age).fQco(kid) = ICRP.fQco.kidney(a);
    human.(age).fQco(liv) = ICRP.fQco.liver(a);
    human.(age).fQco(mus) = ICRP.fQco.skeletal_muscle(a);
    human.(age).fQco(ski) = ICRP.fQco.skin(a);
    human.(age).fQco(spl) = ICRP.fQco.spleen(a);

    human.(age).fQco(rob) = 100 - sum(human.(age).fQco(tissue.intoVen));
    
    human.(age).fQco(lun) = 100 - human.(age).fQco(rob); % ensures closed blood flow system
    human.(age).fQco(art) = human.(age).fQco(lun);
    human.(age).fQco(ven) = human.(age).fQco(lun);
    
    human.(age).fQco      = human.(age).fQco/100;        % change of units: [%] -> fraction
    
    human.(age).co        = ICRP.co(a); human.(age).unit.co = 'L/min';
    human.(age).Q         = human.(age).fQco*human.(age).co;


end;
%%%
%%% DUE TO LACK OF DATA: 
%%%
%%% For all children age classes: we estimated the fQco values based on the
%%% approach presented in Abraham et al (2005)
%%%

%%% For age   'newborn','age1',  'age5' , 'age10', 'age15m','age15f'
refAgeClass = {'age35m','age35m','age35m','age35m','age35m','age35m'};  
% Note: Scaling 'age15f' based on 'age35f' would result in liver blood of 'age15f' larger than for 'age35f'
Q_perKg_bra = [50 59 71 68 57 57]*10/1000;        % brain: ml/min/100g -> L/min/kg, Chiron et al, 1992
for a = human.childrenAgeClasses
    age    = char(human.AgeSexClasses(a));
    refAge = char(refAgeClass(a));
    
    Q_perKg = human.(refAge).Q./human.(refAge).V;
    Q_hat   = Q_perKg.*human.(age).V;
    
    Q_tis      = initialize;
    Q_tis(kid) = Q_hat(kid);                      % kidneys: Grunert et al, 1990, Q per g tissue independent of age
    Q_tis(bra) = Q_perKg_bra(a)*human.(age).V(bra); 
    
    tis = [tissue.intoVen rob];                   % all tissues with infow into the ven, including rob
    tis(find([kid==tis])) = []; tis(find([bra==tis])) = []; % as above, but excluding bra and kid
    normalization_Qco = sum(Q_hat(tis))/(human.(age).co - Q_tis(kid) - Q_tis(bra)); 
    if normalization_Qco >1
        error('Scaling blood flows to children resulted in cardiac output that is larger than the experimental reported one!');
    end;
    Q_tis(tis) = Q_hat(tis)/normalization_Qco;
    
    Q_tis([art lun ven]) = human.(age).co - Q_tis(rob);
    % pv = portal vein
    Q_perKg_hepart = (human.(refAge).Q(liv)-human.(refAge).Q(gut)-human.(refAge).Q(spl))/human.(refAge).V(liv);
    Q_hat_hepart   = Q_perKg_hepart*human.(age).V(liv);
    normalization_Qpv  = (Q_hat_hepart+Q_hat(gut)+Q_hat(spl))/Q_tis(liv);
    if normalization_Qpv >1
        error('Scaling blood flows to children resulted in liver blood flow that is larger than the experimental reported one!');
    end;
    tis = [gut spl];
    Q_tis(tis) = Q_hat(tis)/normalization_Qpv;
    
    human.(age).Q    = Q_tis;
    human.(age).fQco = human.(age).Q/human.(age).co;
    human.(age).normalization_Qco = normalization_Qco;
    
end;




%%% ========================================================================================================
%%% Data  : Fraction of interstitial space (fVtis.int) in rat (based on
%%%         tissue weight INCLUDING vascular blood!)
%%%
%%% Unit  : fraction of tissue space
%%% Source: Kawai etal, J Pharmacokinet Biopharm, Vol 22, 1994 (Table B-I),
%%%         based on measurements in nonbled rats
%%%
rat.fVtis.vas = initialize;
rat.fVtis.vas(adi) = 0.010; 
rat.fVtis.vas(bon) = 0.041; 
rat.fVtis.vas(bra) = 0.037; 
rat.fVtis.vas(gut) = 0.024; 
rat.fVtis.vas(hea) = 0.262;
rat.fVtis.vas(kid) = 0.105;
rat.fVtis.vas(liv) = 0.115;
rat.fVtis.vas(lun) = 0.262;
rat.fVtis.vas(mus) = 0.026;
rat.fVtis.vas(ski) = 0.019;
rat.fVtis.vas(spl) = 0.282;
rat.fVtis.vas(art) = 1;
rat.fVtis.vas(ven) = 1;

rat.fVtis.res = initialize;
rat.fVtis.res(adi) = 0.005; 
rat.fVtis.res(bon) = 0.019; 
rat.fVtis.res(bra) = 0.014; 
rat.fVtis.res(gut) = 0.010; 
rat.fVtis.res(hea) = 0.061;
rat.fVtis.res(kid) = 0.046;
rat.fVtis.res(liv) = 0.057;
rat.fVtis.res(lun) = 0.175;
rat.fVtis.res(mus) = 0.004;
rat.fVtis.res(ski) = 0.002;
rat.fVtis.res(spl) = 0.321;
rat.fVtis.res(art) = 0;
rat.fVtis.res(ven) = 0;

%%% ========================================================================================================
%%% Data  : Fraction of interstitial and intra-cellular space (fVtis.int, fVtis.cel) in rat
%%%         (based on tissue weight INCLUDING vascular blood!)
%%%
%%% Unit  : fraction of tissue space
%%% Source: Kawai etal, J Pharmacokinet Biopharm, Vol 22, 1994 (Table B-I) 
%%%
rat.fVtis.int = initialize;
rat.fVtis.int(adi) = 0.135;
rat.fVtis.int(bon) = 0.100; 
rat.fVtis.int(bra) = 0.004; 
rat.fVtis.int(gut) = 0.094; 
rat.fVtis.int(hea) = 0.100;
rat.fVtis.int(kid) = 0.200;
rat.fVtis.int(liv) = 0.163;
rat.fVtis.int(lun) = 0.188;
rat.fVtis.int(mus) = 0.120;
rat.fVtis.int(ski) = 0.302;
rat.fVtis.int(spl) = 0.150;
rat.fVtis.int(art) = 0;
rat.fVtis.int(ven) = 0;
%%%
rat.fVtis.cel = 1-rat.fVtis.vas-rat.fVtis.int;
%%%
%%% Determining fraction of interstitial and intra-cellular space with
%%% respect to tissue weight NOT INCLUDING vascular blood so that
%%% fVtis.int+fVtis.cel = 1
%%%
m = tissue.allTisExBlo;
rat.fVtis.int(m) = rat.fVtis.int(m)./(1-rat.fVtis.vas(m));
rat.fVtis.cel(m) = rat.fVtis.cel(m)./(1-rat.fVtis.vas(m));

%%% ========================================================================================================
%%% Data  : Total tissue water (Vwt)
%%%
%%% Unit  : fraction of tissue volume
%%% Source: Rodgers and Rowland (2006) including correction for residual blood
%%%
rat.fVtis.wex = initialize;  %%% tissue water (extra-cellular)
rat.fVtis.wex(adi) = 0.135;
rat.fVtis.wex(bon) = 0.100;
rat.fVtis.wex(bra) = 0.162;
rat.fVtis.wex(gut) = 0.282;
rat.fVtis.wex(hea) = 0.320;
rat.fVtis.wex(kid) = 0.273;
rat.fVtis.wex(liv) = 0.161;
rat.fVtis.wex(lun) = 0.336;
rat.fVtis.wex(mus) = 0.118;  
rat.fVtis.wex(ski) = 0.382;
rat.fVtis.wex(spl) = 0.207;
rat.fVtis.wex(pla) = NaN;
rat.fVtis.wex(ery) = 0; 
%
rat.fVtis.wic = initialize;  %%% tissue water (intra-cellular)
rat.fVtis.wic(adi) = 0.017;
rat.fVtis.wic(bon) = 0.346;
rat.fVtis.wic(bra) = 0.620;
rat.fVtis.wic(gut) = 0.475;
rat.fVtis.wic(hea) = 0.456;
rat.fVtis.wic(kid) = 0.483;
rat.fVtis.wic(liv) = 0.573;
rat.fVtis.wic(lun) = 0.446;
rat.fVtis.wic(mus) = 0.630;  
rat.fVtis.wic(ski) = 0.291;
rat.fVtis.wic(spl) = 0.579;
rat.fVtis.wic(pla) = NaN;
rat.fVtis.wic(ery) = NaN; 

%%% Source: Poulin & Theil, J Pharm Sci, Vol 91, 2002 and Poulin & Theil, J Pharm Sci, 2009 for ery value
%%% 
human.fVtis.wtot = initialize;
human.fVtis.wtot(lun) = 0.811;
human.fVtis.wtot(kid) = 0.783;
human.fVtis.wtot(spl) = 0.778;
human.fVtis.wtot(bra) = 0.77;
human.fVtis.wtot(mus) = 0.76;
human.fVtis.wtot(hea) = 0.758;
human.fVtis.wtot(liv) = 0.751;
human.fVtis.wtot(gut) = 0.718;
human.fVtis.wtot(ski) = 0.718; 
human.fVtis.wtot(bon) = 0.439; 
human.fVtis.wtot(adi) = 0.18;
human.fVtis.wtot(pla) = 0.945;
human.fVtis.wtot(ery) = 0.63;       
%%%
%%% DUE TO LACK OF DATA: 
%%%
%%% Assume that ratio wex-to-wtot is the same as in RAT
%%%
r_wex_to_wtot   = rat.fVtis.wex./(rat.fVtis.wex+rat.fVtis.wic);
%%%
human.fVtis.wex = initialize; %%% fraction of extra-celluar water (wex)
human.fVtis.wex = r_wex_to_wtot.*human.fVtis.wtot;
human.fVtis.wex(ery) = 0;
%%%
human.fVtis.wic = initialize; %%% fraction of intracellular water (wic)
human.fVtis.wic = human.fVtis.wtot-human.fVtis.wex;



%%% ========================================================================================================
%%% Data  : Neutral lipids (fVtis.nlt) and neutal phospholipids (fVtis.npt)
%%%
%%% Unit  : fraction of tissue volume
%%% Source: Poulin and Theil, J Pharm Sci, Vol. 91, 2002
%%%         Rodgers and Rowland, J Pharm Res, Vol. 24, 2007 (erythrocyte
%%%         values only), (Table VII)
%%%
human.fVtis.nlt = initialize;  %%% neutral lipids
human.fVtis.nlt(adi) = 0.79;
human.fVtis.nlt(bon) = 0.074;
human.fVtis.nlt(bra) = 0.051;
human.fVtis.nlt(gut) = 0.0487;
human.fVtis.nlt(hea) = 0.0115;
human.fVtis.nlt(kid) = 0.0207;
human.fVtis.nlt(liv) = 0.0348;
human.fVtis.nlt(lun) = 0.003;
human.fVtis.nlt(mus) = 0.0238;  
human.fVtis.nlt(ski) = 0.0284;
human.fVtis.nlt(spl) = 0.0201;
human.fVtis.nlt(pla) = 0.0035;
human.fVtis.nlt(ery) = 0.0033; 
%%%
human.fVtis.npt = initialize;  %%% neutral phospholipids
human.fVtis.npt(bra) = 0.0565;
human.fVtis.npt(liv) = 0.0252; 
human.fVtis.npt(spl) = 0.0198;
human.fVtis.npt(hea) = 0.0166;
human.fVtis.npt(gut) = 0.0163;
human.fVtis.npt(kid) = 0.0162;
human.fVtis.npt(ski) = 0.0111;
human.fVtis.npt(lun) = 0.009;
human.fVtis.npt(mus) = 0.0072; 
human.fVtis.npt(adi) = 0.002;
human.fVtis.npt(bon) = 0.0011;
human.fVtis.npt(pla) = 0.00225;
human.fVtis.npt(ery) = 0.0012;  


%%% Source: Graham etal, J Pharm Pharmacol (2011), 
%%% based on: Rodgers and Rowland (2006)
%%%
rat.fVtis.nlt = initialize;  %%% neutral lipids
rat.fVtis.nlt(adi) = 0.8530;
rat.fVtis.nlt(bon) = 0.0174;
rat.fVtis.nlt(bra) = 0.0391;
rat.fVtis.nlt(gut) = 0.0375;
rat.fVtis.nlt(hea) = 0.0135;
rat.fVtis.nlt(kid) = 0.0121;
rat.fVtis.nlt(liv) = 0.0135;
rat.fVtis.nlt(lun) = 0.0215;
rat.fVtis.nlt(mus) = 0.0100;  
rat.fVtis.nlt(ski) = 0.0603;
rat.fVtis.nlt(spl) = 0.0071;
rat.fVtis.nlt(pla) = NaN;
rat.fVtis.nlt(ery) = NaN; 
%%%
rat.fVtis.npt = initialize;  %%% neutral lipids
rat.fVtis.npt(adi) = 0.0016;
rat.fVtis.npt(bon) = 0.0016;
rat.fVtis.npt(bra) = 0.0015;
rat.fVtis.npt(gut) = 0.0124;
rat.fVtis.npt(hea) = 0.0106;
rat.fVtis.npt(kid) = 0.0240;
rat.fVtis.npt(liv) = 0.0238;
rat.fVtis.npt(lun) = 0.0123;
rat.fVtis.npt(mus) = 0.0072;  
rat.fVtis.npt(ski) = 0.0044;
rat.fVtis.npt(spl) = 0.0170;
rat.fVtis.npt(pla) = NaN;
rat.fVtis.npt(ery) = NaN; 


%%% ========================================================================================================
%%% Data  : Intra-cellular acidic phospholipids (APmt) in rat
%%%
%%% Unit  : mg/g tissue
%%% Source: Rodgers, Leahy, and Rowland, J Pharm Sci, Vol 94, 2005
%%%
rat.fVtis.APmt = initialize;
rat.fVtis.APmt(kid) = 5.03;
rat.fVtis.APmt(liv) = 4.56;
rat.fVtis.APmt(lun) = 3.91;
rat.fVtis.APmt(spl) = 3.18;
rat.fVtis.APmt(gut) = 2.41;
rat.fVtis.APmt(hea) = 2.25;
rat.fVtis.APmt(mus) = 1.53;
rat.fVtis.APmt(ski) = 1.32;
rat.fVtis.APmt(bon) = 0.67;
rat.fVtis.APmt(bra) = 0.4;
rat.fVtis.APmt(adi) = 0.4;
rat.fVtis.APmt(pla) = NaN;
rat.fVtis.APmt(ery) = 0.5;
rat.fVtis.APmt = rat.fVtis.APmt/1000;  % scaling from: mg/g -->  percentage
%%%
%%% DUE TO LACK OF DATA: 
%%%
%%% Rat values adopted for human (see Rodgers, Leahy, and Rowland, J Pharm
%%% Sci 94 (2005), page 925 (right, 2nd paragraph)
%%%
human.fVtis.APmt = rat.fVtis.APmt;


%%% ========================================================================================================
%%% Data  : Albumin (Atp) in rat
%%%
%%% Unit  : Tissue-to-plasma ratio
%%% Source: Rodgers, and Rowland, J Pharm Sci 95 (2006)
%%% Note  : Correction for residual blood applied according to
%%%         Rodgers and Rowland, J Pharm Sci, Vol 95, 2006 (Appendix A),
%%% 
rat.r.Atp  = initialize;
rat.r.Atp(ski) = 0.277;
rat.r.Atp(lun) = 0.212;
rat.r.Atp(gut) = 0.158;
rat.r.Atp(hea) = 0.157;
rat.r.Atp(kid) = 0.130;
rat.r.Atp(bon) = 0.100;
rat.r.Atp(spl) = 0.097;
rat.r.Atp(liv) = 0.086;
rat.r.Atp(mus) = 0.064;
rat.r.Atp(adi) = 0.049;
rat.r.Atp(bra) = 0.048;  
%%%
%%% DUE TO LACK OF DATA: 
%%%
%%% Rat values adopted for human (see Rodgers, Leahy, and Rowland, J Pharm
%%% Sci 94 (2005), page 925 (right, 2nd paragraph)
%%%
human.r.Atp      = rat.r.Atp; 


%%% ========================================================================================================
%%% Data  : Lipoproteine (Ltp) in rat
%%%
%%% Unit  : Tissue-to-plasma ratio
%%% Source: Rodgers, and Rowland, J Pharm Sci 95 (2006)
%%% Note  : Correction for residual blood applied according to
%%%         Rodgers and Rowland, J Pharm Sci, Vol 95, 2006 (Appendix A),
%%% 
rat.r.Ltp = initialize;
rat.r.Ltp(spl) = 0.207;
rat.r.Ltp(lun) = 0.168;
rat.r.Ltp(hea) = 0.160;
rat.r.Ltp(gut) = 0.141;
rat.r.Ltp(liv) = 0.161;
rat.r.Ltp(kid) = 0.137;
rat.r.Ltp(ski) = 0.096;
rat.r.Ltp(adi) = 0.068;
rat.r.Ltp(mus) = 0.059;
rat.r.Ltp(bon) = 0.050;
rat.r.Ltp(bra) = 0.041;            
%%%
%%% DUE TO LACK OF DATA: 
%%%
%%% Rat values adopted for human (see Rodgers, Leahy, and Rowland, J Pharm
%%% Sci 94 (2005), page 925 (right, 2nd paragraph)
%%%
human.r.Ltp = rat.r.Ltp;


%%% ========================================================================================================
%%% Model : Assignment of fractional volumes and ratio to all age classes
%%%
for a = human.allAgeClasses
    
    age = char(human.AgeSexClasses(a));
    human.(age).fVtis = human.fVtis; human.(age).unit.fVtis = 'fraction';
    human.(age).r     = human.r; human.(age).unit.r = 'unitless';
    
end;


%%% ========================================================================================================
%%% Model : Assignment of species and tissue
%%%
switch speciestype
    case 'human'
        species = human;
    otherwise
        error('Unknown species')
end;
%%%
T = tissue;

