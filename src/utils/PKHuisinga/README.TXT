%%% README.TXT
%%%
%%% Version: August 31st, 2012
%%%
%%% This text specifies all inputs and outputs of the main script
%%% GenericPBPKmodel.m
%%%
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

ADVISE: First, have a look at GenericPBPKmodel.m and its structure in order to 
get a rough idea of the overall structure/philosophy 



%%% =================================================================
%%% VARIABLE: species
%%% =================================================================

species is a STRUCTURE with the FIELD specifying the age and sex, e.g.,
'age5' (unisex), 'age10' (unisex), 'age15m', 'age15f, 'age35m', 'age35f'

Each age/sex FIELD contains various SUBFIELDS containing all the reference
anatomical and physiological values for that specific age/sex class, e.g.,

species.age35m

contains all information for a reference male human. Most entries
are self-explanatory. In addition, the units of the entries are
specified in species.age35m.units. The subfield 'tissue' contains
the indices of the different tissues and groups of tissues. Hence,
the reference value of the liver volume of a 15y old boy can be
accessed by

species.age15m.V(liv)

To look at all 13 volumes in a nicely formated way, use

printNicely(species.age15m.V)

The structure is assigned in GenericPBPKmodel_physiology.m



%%% =================================================================
%%% VARIABLE: drug
%%% =================================================================

drug is a STRUCTURE with the FIELD specifying the physico-chemical and
pharmacokinetic properties of the drug. Since the latter values are
species specific, drug.human and drug.rat contains the PK values related
to human and rat, respectively.

The structure is assigned in GenericPBPKmodel_drugDatabase.m



%%% =================================================================
%%% VARIABLE: study
%%% =================================================================

study is a STRUCTURE with FIELDs specifying the dose and
the route of application (iv bolus, iv infusion, po), the simulation time 
in [min], the output time points, etc. Dosing can be fixed in unit [mg] or 
per body size descriptor (BSD) in unit [mg/per unit of the BSD]. 
For a constant dose (e.g., 100mg), choose the option 'fixed'.

The structure is assigned in GenericPBPKmodel.m



%%% =================================================================
%%% VARIABLE: reference
%%% =================================================================

reference is a STRUCTURE build from the species structure. While the 
species structure is like a database containing all species information, 
the reference structure merges the relevant information of the three
structures species, drug and study. One might think of the reference
structure representing a volunteer/patient that is given a drug in a
predescribed way.  

The substructure of reference is analogous to the species substructure. 

The structure is assigned in GenericPBPKmodel_referenceIndividual.m



%%% =================================================================
%%% VARIABLE: population
%%% =================================================================

population is a STRUCTURE containing all relevant information to
generate a virtual population comprising a specified number (size) 
of individuals. The generation of an individual is based on the 
following information

age (class), sex, BH, BW

Based on this information and the data of the reference (of that age/sex),
an individual is generated based on scaling (see article by Huisinga et al.,
PSP 2012 referenced above for more details on the algorithm).

The data regarding a single individual of the population
are stored in the vector individual (see below). 

The structure population contains a summary of the population
characteristics, e.g.,

population.BMI

gives the body mass indices of all individuals of the population. 
   
For information on available virtual populations see GenericPBPKmodel.m.

The structure is assigned in GenericPBPKmodel.m and filled with further 
information in GenericPBPKmodel_randomEffects.m.



%%% =================================================================
%%% VARIABLE: individual
%%% =================================================================

individual is a VECTOR of STRUCTURES build from the reference structure 
and target age, sex, BH, BW specified in population. For a population
of at least three individuals, 

individual(3)

contains all information regarding individual No 3. Individual(3) is 
a STRUCTURE analogous to the reference structure for given age/sex.

The structure is assigned in GenericPBPKmodel_virtualPopulation.m, 
potentially reassigned in GenericPBPKmodel_randomEffect.m. Predicted
concentration time data are generated in 
GenericPBPKmodel_simulatePBPKmodel.m.



%%% =================================================================
%%% VARIABLE: randomEffects
%%% =================================================================

randomEffects is a STRUCTURE containing information about possible
random effects (perturbations) of the two parameters CL (blood clearance)
and Ktis (tissue-to-blood partition coefficient). The size of the 
random perturbations is specified by facX. As a consequence, all 
individuals will be assigned randomly perturbed CL and Ktis values, if
facX is different from 0. 

The random effects are realized in GenericPBPKmodel_randomEffects.m.



%%% =================================================================
%%% VARIABLE: ShowGraphics
%%% =================================================================

ShowGraphics is a STRUCTURE specifying the output options. 

The structure is assigned in GenericPBPKmodel.m.



%%% =================================================================
%%% VARIABLE: lumping
%%% =================================================================

lumping is a CELL specifying the grouping of the tissues into different
lumped compartments. The central compartment is defined as the one
that contains ven. For example,

lumping = {[T.mus,T.adi,T.bon,T.ski],[T.bra,T.hea,T.kid,T.gut,T.spl,T.liv,T.lun,T.art,T.ven]};

generates a two compartment model with peripheral compartment containing
mus, adi, bon and ski, while the central compartment comprises the 
remaining tissues.

Based on the lumping variable, a lumped compartment model is constructed
and simulated. The results are assigned to the STRUCTURE L or minL.

The cell lumping is assigned in GenericPBPKmodel.m. The structure L and
minL are assigned in GenericPBPKmodel_mechanisticLumping.m.


