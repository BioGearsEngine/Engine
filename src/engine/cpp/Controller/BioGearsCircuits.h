/**************************************************************************************
Copyright 2015 Applied Research Associates, Inc.
Licensed under the Apache License, Version 2.0 (the "License"); you may not use
this file except in compliance with the License. You may obtain a copy of the License
at:
http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied. See the License for the
specific language governing permissions and limitations under the License.
**************************************************************************************/

#pragma once
class BioGears;

#include "circuit/SECircuitManager.h"

#include "bind/enumOnOff.hxx"

/**
* @brief Manages all circuits associated with all %BioGears systems/equipement
*/
class BIOGEARS_API BioGearsCircuits : public SECircuitManager
{
public:

  BioGearsCircuits(BioGears& data);
  virtual ~BioGearsCircuits();
  void Clear();

  virtual bool Load(const CDM::CircuitManagerData& in);

  virtual void SetReadOnlyFluid(bool b);

  // Active methods return the circuit configuration to be calculated
  SEFluidCircuit&           GetActiveCardiovascularCircuit();
  SEFluidCircuit&           GetCardiovascularCircuit();
  SEFluidCircuit&           GetRenalCircuit();

  SEFluidCircuit&           GetActiveRespiratoryCircuit();
  SEFluidCircuit&	          GetRespiratoryCircuit();
  SEFluidCircuit&	          GetRespiratoryAndAnesthesiaMachineCircuit();
  SEFluidCircuit&	          GetRespiratoryAndInhalerCircuit();
  SEFluidCircuit&	          GetRespiratoryAndMechanicalVentilatorCircuit();

  SEThermalCircuit&         GetTemperatureCircuit();
  SEThermalCircuit&	        GetExternalTemperatureCircuit();
  SEThermalCircuit&	        GetInternalTemperatureCircuit();

  SEFluidCircuit&	          GetAnesthesiaMachineCircuit();

protected:

  BioGears& m_data;

  // Note, I am not making a tissue or inhaler standalone circuit
  // this are super simple circuits and can't be tested by themselves 

  // The combined cardiovascular circuit only needs to be built once,
  // but we can build it different ways, with or without renal and tissue subcircuits.
  // It depends on how the engine is configured, but this configuration does not change
  // once it's set. Tests can also configure how it's built and test appropriately.

  SEFluidCircuit*            m_CombinedCardiovascularCircuit;

  SEFluidCircuit*	           m_CardiovascularCircuit;
  SEFluidCircuit*         	 m_RenalCircuit;

  SEFluidCircuit*            m_RespiratoryCircuit;

  SEFluidCircuit*            m_AnesthesiaMachineCircuit;

  // Based on if equipment is hooked up, we have to build out the combination of 
  // these 2 circuits and graphs as we don't want to dynamically modify circuits
  // It's quicker and easier to test these combiniation circuits
  SEFluidCircuit*            m_CombinedRespiratoryAnesthesiaCircuit;
  SEFluidCircuit*            m_CombinedRespiratoryInhalerCircuit;
  SEFluidCircuit*            m_CombinedRespiratoryMechanicalVentilatorCircuit;

  SEThermalCircuit*          m_TemperatureCircuit;
  SEThermalCircuit*	         m_InternalTemperatureCircuit;// Attempt at circuit independence
  SEThermalCircuit*          m_ExternalTemperatureCircuit;// Attempt at circuit independence
};

///////////////////////////////
// Respiratory Circuit Enums //
///////////////////////////////

namespace mil {
  namespace tatrc {
    namespace physiology {
      namespace biogears {

        class Circuits
        {
        public:
          DEFINE_STATIC_STRING(FullCardiovascular);
          DEFINE_STATIC_STRING(Cardiovascular);
          DEFINE_STATIC_STRING(Renal);
          DEFINE_STATIC_STRING(Respiratory);
          DEFINE_STATIC_STRING(AnesthesiaMachine);
          DEFINE_STATIC_STRING(RespiratoryAnesthesia);
          DEFINE_STATIC_STRING(RespiratoryInhaler);
          DEFINE_STATIC_STRING(RespiratoryMechanicalVentilator);
          DEFINE_STATIC_STRING(Temperature);
          DEFINE_STATIC_STRING(InternalTemperature);
          DEFINE_STATIC_STRING(ExternalTemperature);
        };

        ///////////////////////////
        // Chyme Circuit Enums //
        ///////////////////////////

        class ChymeNode
        {
        public:
          DEFINE_STATIC_STRING(SmallIntestineC1);
        };

        class ChymePath
        {
        public:
          DEFINE_STATIC_STRING(SmallIntestineC1ToSmallIntestine1);
          DEFINE_STATIC_STRING(GroundToSmallIntestineC1);
          DEFINE_STATIC_STRING(GutT1ToGround);
        };


        class RespiratoryNode
        {
        public:
          DEFINE_STATIC_STRING(Carina);

          DEFINE_STATIC_STRING(LeftAlveoli);
          DEFINE_STATIC_STRING(LeftAlveoliLeak);
          DEFINE_STATIC_STRING(LeftAnatomicDeadSpace);
          DEFINE_STATIC_STRING(LeftChestLeak);
          DEFINE_STATIC_STRING(LeftPleural);
          DEFINE_STATIC_STRING(LeftPleuralConnection);

          DEFINE_STATIC_STRING(Mouth);

          DEFINE_STATIC_STRING(RightAlveoli);
          DEFINE_STATIC_STRING(RightAlveoliLeak);
          DEFINE_STATIC_STRING(RightAnatomicDeadSpace);
          DEFINE_STATIC_STRING(RightChestLeak);
          DEFINE_STATIC_STRING(RightPleural);
          DEFINE_STATIC_STRING(RightPleuralConnection);

          DEFINE_STATIC_STRING(Stomach);
          DEFINE_STATIC_STRING(RespiratoryMuscle);

        };

        class RespiratoryPath
        {
        public:
          DEFINE_STATIC_STRING(CarinaToLeftAnatomicDeadSpace);
          DEFINE_STATIC_STRING(CarinaToRightAnatomicDeadSpace);

          DEFINE_STATIC_STRING(EnvironmentToLeftChestLeak);
          DEFINE_STATIC_STRING(EnvironmentToMouth);
          DEFINE_STATIC_STRING(EnvironmentToRightChestLeak);

          DEFINE_STATIC_STRING(LeftAlveoliLeakToLeftPleural);
          DEFINE_STATIC_STRING(LeftAlveoliToLeftAlveoliLeak);
          DEFINE_STATIC_STRING(LeftAlveoliToLeftPleuralConnection);
          DEFINE_STATIC_STRING(LeftPleuralConnectionToLeftPleural);
          DEFINE_STATIC_STRING(LeftAnatomicDeadSpaceToLeftAlveoli);
          DEFINE_STATIC_STRING(LeftAnatomicDeadSpaceToLeftPleuralConnection);
          DEFINE_STATIC_STRING(LeftChestLeakToLeftPleural);
          DEFINE_STATIC_STRING(LeftPleuralToEnvironment);
          DEFINE_STATIC_STRING(LeftPleuralToRespiratoryMuscle);

          DEFINE_STATIC_STRING(MouthToCarina);
          DEFINE_STATIC_STRING(MouthToStomach);

          DEFINE_STATIC_STRING(RightAlveoliLeakToRightPleural);
          DEFINE_STATIC_STRING(RightAlveoliToRightAlveoliLeak);
          DEFINE_STATIC_STRING(RightAlveoliToRightPleuralConnection);
          DEFINE_STATIC_STRING(RightPleuralConnectionToRightPleural);
          DEFINE_STATIC_STRING(RightAnatomicDeadSpaceToRightAlveoli);
          DEFINE_STATIC_STRING(RightAnatomicDeadSpaceToRightPleuralConnection);
          DEFINE_STATIC_STRING(RightChestLeakToRightPleural);
          DEFINE_STATIC_STRING(RightPleuralToEnvironment);
          DEFINE_STATIC_STRING(RightPleuralToRespiratoryMuscle);

          DEFINE_STATIC_STRING(EnvironmentToRespiratoryMuscle);
          DEFINE_STATIC_STRING(StomachToEnvironment);
        };

        //////////////////////////////////////
        // Anesthesia Machine Circuit Enums //
        //////////////////////////////////////

        class AnesthesiaMachineNode
        {
        public:
          DEFINE_STATIC_STRING(AnesthesiaConnection);
          DEFINE_STATIC_STRING(ExpiratoryLimb);
          DEFINE_STATIC_STRING(GasInlet);
          DEFINE_STATIC_STRING(GasSource);
          DEFINE_STATIC_STRING(InspiratoryLimb);
          DEFINE_STATIC_STRING(ReliefValve);
          DEFINE_STATIC_STRING(Scrubber);
          DEFINE_STATIC_STRING(Selector);
          DEFINE_STATIC_STRING(Ventilator);
          DEFINE_STATIC_STRING(VentilatorConnection);
          DEFINE_STATIC_STRING(YPiece);
        };

        class AnesthesiaMachinePath
        {
        public:
          DEFINE_STATIC_STRING(EnvironmentToVentilator);
          DEFINE_STATIC_STRING(EnvironmentToReliefValve);
          DEFINE_STATIC_STRING(VentilatorToVentilatorConnection);
          DEFINE_STATIC_STRING(VentilatorConnectionToSelector);
          DEFINE_STATIC_STRING(SelectorToReliefValve);
          DEFINE_STATIC_STRING(SelectorToScrubber);
          DEFINE_STATIC_STRING(ScrubberToGasInlet);
          DEFINE_STATIC_STRING(EnvironmentToGasSource);
          DEFINE_STATIC_STRING(GasSourceToGasInlet);
          DEFINE_STATIC_STRING(GasInletToInspiratoryLimb);
          DEFINE_STATIC_STRING(InspiratoryLimbToYPiece);
          DEFINE_STATIC_STRING(YPieceToExpiratoryLimb);
          DEFINE_STATIC_STRING(ExpiratoryLimbToSelector);
          DEFINE_STATIC_STRING(YPieceToAnesthesiaConnection);
          DEFINE_STATIC_STRING(AnesthesiaConnectionToEnvironment);
          DEFINE_STATIC_STRING(SelectorToEnvironment);
        };

        class CombinedAnesthesiaMachinePath
        {
        public:
          DEFINE_STATIC_STRING(AnesthesiaConnectionToMouth);
          DEFINE_STATIC_STRING(GroundConnection);
        };

        ///////////////////////////
        // Inhaler Circuit Enums //
        ///////////////////////////

        class InhalerNode
        {
        public:
          DEFINE_STATIC_STRING(Mouthpiece);
        };

        class InhalerPath
        {
        public:
          DEFINE_STATIC_STRING(EnvironmentToMouthpiece);
          DEFINE_STATIC_STRING(MouthpieceToMouth);
        };

        //////////////////////////////////////////
        // Mechanical Ventilator Circuit Enums //
        ////////////////////////////////////////

        class MechanicalVentilatorNode
        {
        public:
          DEFINE_STATIC_STRING_EX(Connection,MechanicalVentilatorConnection);
        };

        class MechanicalVentilatorPath
        {
        public:
          DEFINE_STATIC_STRING_EX(ConnectionToMouth,MechanicalVentilatorConnectionToMouth);
		  DEFINE_STATIC_STRING_EX(GroundToConnection, MechanicalVentilatorGroundToConnection);
        };


        ///////////////////////////////////
        // Environment Gas Circuit Enums //
        ///////////////////////////////////

        class EnvironmentNode
        {
        public:
          DEFINE_STATIC_STRING(Ambient);
        };

        ///////////////////////////////////////////
        // External Temperature Circuit Enums //
        ///////////////////////////////////////////

        class ExternalTemperatureNode
        {
        public:
          DEFINE_STATIC_STRING(Active);
          DEFINE_STATIC_STRING(Clothing);
          DEFINE_STATIC_STRING(Enclosure);
          DEFINE_STATIC_STRING(ExternalCore);
          DEFINE_STATIC_STRING(ExternalSkin);
          DEFINE_STATIC_STRING(ExternalGround);
          DEFINE_STATIC_STRING(Ambient);
        };

        class ExternalTemperaturePath
        {
        public:
          DEFINE_STATIC_STRING(ActiveToClothing);
          DEFINE_STATIC_STRING(ClothingToEnclosure);
          DEFINE_STATIC_STRING(ClothingToEnvironment);
          DEFINE_STATIC_STRING(ExternalCoreToGround);
          DEFINE_STATIC_STRING(GroundToActive);
          DEFINE_STATIC_STRING(GroundToClothing);
          DEFINE_STATIC_STRING(GroundToEnclosure);
          DEFINE_STATIC_STRING(GroundToEnvironment);
          DEFINE_STATIC_STRING(ExternalSkinToGround);
          DEFINE_STATIC_STRING(ExternalSkinToClothing);
        };

        ////////////////////////////////////////
        // Internal Temperature Circuit Enums //
        ////////////////////////////////////////

        class InternalTemperatureNode
        {
        public:
          DEFINE_STATIC_STRING(InternalCore);
          DEFINE_STATIC_STRING(InternalSkin);
          DEFINE_STATIC_STRING(InternalGround);
        };

        class InternalTemperaturePath
        {
        public:
          DEFINE_STATIC_STRING(GroundToInternalCore);
          DEFINE_STATIC_STRING(InternalCoreToInternalSkin);
          DEFINE_STATIC_STRING(InternalCoreToGround);
          DEFINE_STATIC_STRING(InternalSkinToGround);
        };

        ////////////////////////////////////////
        // Combined Temperature Circuit Enums //
        ////////////////////////////////////////

        class CombinedTemperaturePath
        {
        public:
          DEFINE_STATIC_STRING(InternalCoreToExternalCore);
          DEFINE_STATIC_STRING(InternalSkinToExternalSkin);
        };

        class CardiovascularNode
        {
        public:
          DEFINE_STATIC_STRING(RightHeart1);
          DEFINE_STATIC_STRING(RightHeart2);
          DEFINE_STATIC_STRING(RightHeart3);

          DEFINE_STATIC_STRING(MainPulmonaryArteries);
          DEFINE_STATIC_STRING(LeftIntermediatePulmonaryArteries);
          DEFINE_STATIC_STRING(LeftPulmonaryArteries);
          DEFINE_STATIC_STRING(RightIntermediatePulmonaryArteries);
          DEFINE_STATIC_STRING(RightPulmonaryArteries);

          DEFINE_STATIC_STRING(LeftPulmonaryCapillaries);
          DEFINE_STATIC_STRING(RightPulmonaryCapillaries);

          DEFINE_STATIC_STRING(LeftIntermediatePulmonaryVeins);
          DEFINE_STATIC_STRING(LeftPulmonaryVeins);
          DEFINE_STATIC_STRING(RightIntermediatePulmonaryVeins);
          DEFINE_STATIC_STRING(RightPulmonaryVeins);

          DEFINE_STATIC_STRING(LeftHeart1);
          DEFINE_STATIC_STRING(LeftHeart2);
          DEFINE_STATIC_STRING(LeftHeart3);

          DEFINE_STATIC_STRING(Aorta1);
          DEFINE_STATIC_STRING(Aorta2);
          DEFINE_STATIC_STRING(Aorta3);

          DEFINE_STATIC_STRING(Bone1);
          DEFINE_STATIC_STRING(Bone2);

          DEFINE_STATIC_STRING(Brain1);
          DEFINE_STATIC_STRING(Brain2);

          DEFINE_STATIC_STRING(Fat1);
          DEFINE_STATIC_STRING(Fat2);

          DEFINE_STATIC_STRING(LargeIntestine1);

          DEFINE_STATIC_STRING(Liver1);
          DEFINE_STATIC_STRING(Liver2);
          DEFINE_STATIC_STRING(PortalVein1);

          DEFINE_STATIC_STRING(LeftArm1);
          DEFINE_STATIC_STRING(LeftArm2);

          DEFINE_STATIC_STRING(LeftKidney1);
          DEFINE_STATIC_STRING(LeftKidney2);

          DEFINE_STATIC_STRING(LeftLeg1);
          DEFINE_STATIC_STRING(LeftLeg2);

          DEFINE_STATIC_STRING(Muscle1);
          DEFINE_STATIC_STRING(Muscle2);

          DEFINE_STATIC_STRING(Myocardium1);
          DEFINE_STATIC_STRING(Myocardium2);

          DEFINE_STATIC_STRING(Pericardium1);

          DEFINE_STATIC_STRING(RightArm1);
          DEFINE_STATIC_STRING(RightArm2);

          DEFINE_STATIC_STRING(RightKidney1);
          DEFINE_STATIC_STRING(RightKidney2);

          DEFINE_STATIC_STRING(RightLeg1);
          DEFINE_STATIC_STRING(RightLeg2);

          DEFINE_STATIC_STRING(Skin1);
          DEFINE_STATIC_STRING(Skin2);

          DEFINE_STATIC_STRING(SmallIntestine1);

          DEFINE_STATIC_STRING(Splanchnic1);

          DEFINE_STATIC_STRING(Spleen1);

          DEFINE_STATIC_STRING(VenaCava);

          DEFINE_STATIC_STRING(Ground);
        };

        class CardiovascularPath
        {
        public:
          // Heart and Lungs
          DEFINE_STATIC_STRING(VenaCavaToRightHeart2);
          DEFINE_STATIC_STRING(RightHeart2ToRightHeart1);
          DEFINE_STATIC_STRING(RightHeart1ToRightHeart3);
          DEFINE_STATIC_STRING(RightHeart3ToGround);
          DEFINE_STATIC_STRING(RightHeart1ToMainPulmonaryArteries);
          DEFINE_STATIC_STRING(MainPulmonaryArteriesToRightIntermediatePulmonaryArteries);
          DEFINE_STATIC_STRING(RightIntermediatePulmonaryArteriesToRightPulmonaryArteries);
          DEFINE_STATIC_STRING(RightPulmonaryArteriesToRightPulmonaryVeins);
          DEFINE_STATIC_STRING(RightPulmonaryArteriesToRightPulmonaryCapillaries);
          DEFINE_STATIC_STRING(RightPulmonaryArteriesToGround);
          DEFINE_STATIC_STRING(RightPulmonaryCapillariesToRightPulmonaryVeins);
          DEFINE_STATIC_STRING(RightPulmonaryCapillariesToGround);
          DEFINE_STATIC_STRING(RightPulmonaryVeinsToRightIntermediatePulmonaryVeins);
          DEFINE_STATIC_STRING(RightPulmonaryVeinsToGround);
          DEFINE_STATIC_STRING(RightIntermediatePulmonaryVeinsToLeftHeart2);
          DEFINE_STATIC_STRING(MainPulmonaryArteriesToLeftIntermediatePulmonaryArteries);
          DEFINE_STATIC_STRING(LeftIntermediatePulmonaryArteriesToLeftPulmonaryArteries);
          DEFINE_STATIC_STRING(LeftPulmonaryArteriesToLeftPulmonaryVeins);
          DEFINE_STATIC_STRING(LeftPulmonaryArteriesToLeftPulmonaryCapillaries);
          DEFINE_STATIC_STRING(LeftPulmonaryArteriesToGround);
          DEFINE_STATIC_STRING(LeftPulmonaryCapillariesToGround);
          DEFINE_STATIC_STRING(LeftPulmonaryCapillariesToLeftPulmonaryVeins);
          DEFINE_STATIC_STRING(LeftPulmonaryVeinsToLeftIntermediatePulmonaryVeins);
          DEFINE_STATIC_STRING(LeftPulmonaryVeinsToGround);
          DEFINE_STATIC_STRING(LeftIntermediatePulmonaryVeinsToLeftHeart2)
            DEFINE_STATIC_STRING(LeftHeart2ToLeftHeart1);
          DEFINE_STATIC_STRING(LeftHeart1ToLeftHeart3);
          DEFINE_STATIC_STRING(LeftHeart3ToGround);
          DEFINE_STATIC_STRING(LeftHeart1ToAorta2);
          DEFINE_STATIC_STRING(Aorta2ToAorta3);
          DEFINE_STATIC_STRING(Aorta3ToAorta1);
          DEFINE_STATIC_STRING(Aorta1ToGround);
          // Brain
          DEFINE_STATIC_STRING(Aorta1ToBrain1);
          DEFINE_STATIC_STRING(Brain1ToGround);
          DEFINE_STATIC_STRING(Brain1ToBrain2);
          DEFINE_STATIC_STRING(Brain2ToVenaCava);
          // Bone
          DEFINE_STATIC_STRING(Aorta1ToBone1);
          DEFINE_STATIC_STRING(Bone1ToGround);
          DEFINE_STATIC_STRING(Bone1ToBone2);
          DEFINE_STATIC_STRING(Bone2ToVenaCava);
          // Fat
          DEFINE_STATIC_STRING(Aorta1ToFat1);
          DEFINE_STATIC_STRING(Fat1ToGround);
          DEFINE_STATIC_STRING(Fat1ToFat2);
          DEFINE_STATIC_STRING(Fat2ToVenaCava);
          // Large Intestine
          DEFINE_STATIC_STRING(Aorta1ToLargeIntestine);
          DEFINE_STATIC_STRING(LargeIntestineToGround);
          DEFINE_STATIC_STRING(LargeIntestineToPortalVein);
          // Left Arm
          DEFINE_STATIC_STRING(Aorta1ToLeftArm1);
          DEFINE_STATIC_STRING(LeftArm1ToGround);
          DEFINE_STATIC_STRING(LeftArm1ToLeftArm2);
          DEFINE_STATIC_STRING(LeftArm2ToVenaCava);
          // Left Kidney
          DEFINE_STATIC_STRING(Aorta1ToLeftKidney1);
          DEFINE_STATIC_STRING(LeftKidney1ToGround);
          DEFINE_STATIC_STRING(LeftKidney1ToLeftKidney2);
          DEFINE_STATIC_STRING(LeftKidney2ToVenaCava);
          // Left Leg
          DEFINE_STATIC_STRING(Aorta1ToLeftLeg1);
          DEFINE_STATIC_STRING(LeftLeg1ToGround);
          DEFINE_STATIC_STRING(LeftLeg1ToLeftLeg2);
          DEFINE_STATIC_STRING(LeftLeg2ToVenaCava);
          // Liver
          DEFINE_STATIC_STRING(Aorta1ToLiver1);
          DEFINE_STATIC_STRING(Liver1ToGround);
          DEFINE_STATIC_STRING(PortalVeinToLiver1);
          DEFINE_STATIC_STRING(Liver1ToLiver2);
          DEFINE_STATIC_STRING(Liver2ToVenaCava);
          // Muscle
          DEFINE_STATIC_STRING(Aorta1ToMuscle1);
          DEFINE_STATIC_STRING(Muscle1ToGround);
          DEFINE_STATIC_STRING(Muscle1ToMuscle2);
          DEFINE_STATIC_STRING(Muscle2ToVenaCava);
          // Myocardium
          DEFINE_STATIC_STRING(Aorta1ToMyocardium1);
          DEFINE_STATIC_STRING(Myocardium1ToGround);
          DEFINE_STATIC_STRING(Myocardium1ToMyocardium2);
          DEFINE_STATIC_STRING(Myocardium2ToVenaCava);
          // Pericardium
          DEFINE_STATIC_STRING(Pericardium1ToGround);
          DEFINE_STATIC_STRING(GroundToPericardium1);
          // Right Arm
          DEFINE_STATIC_STRING(Aorta1ToRightArm1);
          DEFINE_STATIC_STRING(RightArm1ToGround);
          DEFINE_STATIC_STRING(RightArm1ToRightArm2);
          DEFINE_STATIC_STRING(RightArm2ToVenaCava);
          // Right Kidney
          DEFINE_STATIC_STRING(Aorta1ToRightKidney1);
          DEFINE_STATIC_STRING(RightKidney1ToGround);
          DEFINE_STATIC_STRING(RightKidney1ToRightKidney2);
          DEFINE_STATIC_STRING(RightKidney2ToVenaCava);
          // Right Leg
          DEFINE_STATIC_STRING(Aorta1ToRightLeg1);
          DEFINE_STATIC_STRING(RightLeg1ToGround);
          DEFINE_STATIC_STRING(RightLeg1ToRightLeg2);
          DEFINE_STATIC_STRING(RightLeg2ToVenaCava);
          // Skin
          DEFINE_STATIC_STRING(Aorta1ToSkin1);
          DEFINE_STATIC_STRING(Skin1ToGround);
          DEFINE_STATIC_STRING(Skin1ToSkin2);
          DEFINE_STATIC_STRING(Skin2ToVenaCava);
          // Small Intestine
          DEFINE_STATIC_STRING(Aorta1ToSmallIntestine);
          DEFINE_STATIC_STRING(SmallIntestineToGround);
          DEFINE_STATIC_STRING(SmallIntestineToPortalVein);
          // Splanchnic
          DEFINE_STATIC_STRING(Aorta1ToSplanchnic);
          DEFINE_STATIC_STRING(SplanchnicToGround);
          DEFINE_STATIC_STRING(SplanchnicToPortalVein);
          // Spleen
          DEFINE_STATIC_STRING(Aorta1ToSpleen);
          DEFINE_STATIC_STRING(SpleenToGround);
          DEFINE_STATIC_STRING(SpleenToPortalVein);
          // Vena Cava
          DEFINE_STATIC_STRING(VenaCavaToGround);
          DEFINE_STATIC_STRING(VenaCavaBleed);
          DEFINE_STATIC_STRING(IVToVenaCava);
        };

        class TissueNode
        {
        public:
          DEFINE_STATIC_STRING(BoneT1);
          DEFINE_STATIC_STRING(BoneT2);
          DEFINE_STATIC_STRING(BoneT3);
          DEFINE_STATIC_STRING(BrainT1);
          DEFINE_STATIC_STRING(BrainT2);
          DEFINE_STATIC_STRING(BrainT3);
          DEFINE_STATIC_STRING(FatT1);
          DEFINE_STATIC_STRING(FatT2);
          DEFINE_STATIC_STRING(FatT3);
          DEFINE_STATIC_STRING(GutT1);
          DEFINE_STATIC_STRING(GutT2);
          DEFINE_STATIC_STRING(GutT3);
          DEFINE_STATIC_STRING(LiverT1);
          DEFINE_STATIC_STRING(LiverT2);
          DEFINE_STATIC_STRING(LiverT3);
          DEFINE_STATIC_STRING(LeftKidneyT1);
          DEFINE_STATIC_STRING(LeftKidneyT2);
          DEFINE_STATIC_STRING(LeftKidneyT3);
          DEFINE_STATIC_STRING(LeftLungT1);
          DEFINE_STATIC_STRING(LeftLungT2);
          DEFINE_STATIC_STRING(LeftLungT3);
          DEFINE_STATIC_STRING(Lymph);
          DEFINE_STATIC_STRING(MuscleT1);
          DEFINE_STATIC_STRING(MuscleT2);
          DEFINE_STATIC_STRING(MuscleT3);
          DEFINE_STATIC_STRING(MyocardiumT1);
          DEFINE_STATIC_STRING(MyocardiumT2);
          DEFINE_STATIC_STRING(MyocardiumT3);
          DEFINE_STATIC_STRING(RightLungT1);
          DEFINE_STATIC_STRING(RightLungT2);
          DEFINE_STATIC_STRING(RightLungT3);
          DEFINE_STATIC_STRING(RightKidneyT1);
          DEFINE_STATIC_STRING(RightKidneyT2);
          DEFINE_STATIC_STRING(RightKidneyT3);
          DEFINE_STATIC_STRING(SkinT1);
          DEFINE_STATIC_STRING(SkinT2);
          DEFINE_STATIC_STRING(SkinT3);
          DEFINE_STATIC_STRING(SpleenT1);
          DEFINE_STATIC_STRING(SpleenT2);
          DEFINE_STATIC_STRING(SpleenT3);
        };

        class TissuePath
        {
        public:
          DEFINE_STATIC_STRING(Bone1ToBoneT2);
          DEFINE_STATIC_STRING(BoneT2ToBoneT1);
          DEFINE_STATIC_STRING(BoneT1ToBoneT3);
          DEFINE_STATIC_STRING(GroundToBoneT3);
          DEFINE_STATIC_STRING(BoneT1ToLymph);

          DEFINE_STATIC_STRING(Brain1ToBrainT2);
          DEFINE_STATIC_STRING(BrainT2ToBrainT1);
          DEFINE_STATIC_STRING(BrainT1ToBrainT3);
          DEFINE_STATIC_STRING(GroundToBrainT3);
          DEFINE_STATIC_STRING(BrainT1ToLymph);

          DEFINE_STATIC_STRING(Fat1ToFatT2);
          DEFINE_STATIC_STRING(FatT2ToFatT1);
          DEFINE_STATIC_STRING(FatT1ToFatT3);
          DEFINE_STATIC_STRING(GroundToFatT3);
          DEFINE_STATIC_STRING(FatT1ToLymph);

          DEFINE_STATIC_STRING(SmallIntestine1ToGutT2);
          DEFINE_STATIC_STRING(LargeIntestine1ToGutT2);
          DEFINE_STATIC_STRING(Splanchnic1ToGutT2);
          DEFINE_STATIC_STRING(GutT2ToGutT1);
          DEFINE_STATIC_STRING(GutT1ToGutT3);
          DEFINE_STATIC_STRING(GroundToGutT3);
          DEFINE_STATIC_STRING(GutT1ToLymph);

          DEFINE_STATIC_STRING(Liver1ToLiverT2);
          DEFINE_STATIC_STRING(LiverT2ToLiverT1);
          DEFINE_STATIC_STRING(LiverT1ToLiverT3);
          DEFINE_STATIC_STRING(GroundToLiverT3);
          DEFINE_STATIC_STRING(LiverT1ToLymph);

          DEFINE_STATIC_STRING(LeftKidney1ToLeftKidneyT2);
          DEFINE_STATIC_STRING(LeftKidneyT2ToLeftKidneyT1);
          DEFINE_STATIC_STRING(LeftKidneyT1ToLeftKidneyT3);
          DEFINE_STATIC_STRING(GroundToLeftKidneyT3);
          DEFINE_STATIC_STRING(LeftKidneyT1ToLymph);

          DEFINE_STATIC_STRING(LeftLung1ToLeftLungT2);
          DEFINE_STATIC_STRING(LeftLungT2ToLeftLungT1);
          DEFINE_STATIC_STRING(LeftLungT1ToLeftLungT3);
          DEFINE_STATIC_STRING(GroundToLeftLungT3);
          DEFINE_STATIC_STRING(LeftLungT1ToLymph);

          DEFINE_STATIC_STRING(LymphToVenaCava);

          DEFINE_STATIC_STRING(Muscle1ToMuscleT2);
          DEFINE_STATIC_STRING(MuscleT2ToMuscleT1);
          DEFINE_STATIC_STRING(MuscleT1ToMuscleT3);
          DEFINE_STATIC_STRING(GroundToMuscleT3);
          DEFINE_STATIC_STRING(MuscleT1ToLymph);

          DEFINE_STATIC_STRING(Myocardium1ToMyocardiumT2);
          DEFINE_STATIC_STRING(MyocardiumT2ToMyocardiumT1);
          DEFINE_STATIC_STRING(MyocardiumT1ToMyocardiumT3);
          DEFINE_STATIC_STRING(GroundToMyocardiumT3);
          DEFINE_STATIC_STRING(MyocardiumT1ToLymph);

          DEFINE_STATIC_STRING(RightLung1ToRightLungT2);
          DEFINE_STATIC_STRING(RightLungT2ToRightLungT1);
          DEFINE_STATIC_STRING(RightLungT1ToRightLungT3);
          DEFINE_STATIC_STRING(GroundToRightLungT3);
          DEFINE_STATIC_STRING(RightLungT1ToLymph);

          DEFINE_STATIC_STRING(RightKidney1ToRightKidneyT2);
          DEFINE_STATIC_STRING(RightKidneyT2ToRightKidneyT1);
          DEFINE_STATIC_STRING(RightKidneyT1ToRightKidneyT3);
          DEFINE_STATIC_STRING(GroundToRightKidneyT3);
          DEFINE_STATIC_STRING(RightKidneyT1ToLymph);

          DEFINE_STATIC_STRING(Skin1ToSkinT2);
          DEFINE_STATIC_STRING(SkinT2ToSkinT1);
          DEFINE_STATIC_STRING(SkinT1ToSkinT3);
          DEFINE_STATIC_STRING(GroundToSkinT3);
          DEFINE_STATIC_STRING(SkinT1ToLymph);
          DEFINE_STATIC_STRING(SkinSweating);

          DEFINE_STATIC_STRING(Spleen1ToSpleenT2);
          DEFINE_STATIC_STRING(SpleenT2ToSpleenT1);
          DEFINE_STATIC_STRING(SpleenT1ToSpleenT3);
          DEFINE_STATIC_STRING(GroundToSpleenT3);
          DEFINE_STATIC_STRING(SpleenT1ToLymph);
        };

        class RenalNode
        {
        public:
          // Blood
          DEFINE_STATIC_STRING(RightAortaConnection);
          DEFINE_STATIC_STRING(RightRenalArtery);
          DEFINE_STATIC_STRING(RightAfferentArteriole);
          DEFINE_STATIC_STRING(RightGlomerularCapillaries);
          DEFINE_STATIC_STRING(RightNetGlomerularCapillaries);
          DEFINE_STATIC_STRING(RightEfferentArteriole);
          DEFINE_STATIC_STRING(RightPeritubularCapillaries);
          DEFINE_STATIC_STRING(RightNetPeritubularCapillaries);
          DEFINE_STATIC_STRING(RightRenalVein);
          DEFINE_STATIC_STRING(RightVenaCavaConnection);
          // Urine
          DEFINE_STATIC_STRING(RightBowmansCapsules);
          DEFINE_STATIC_STRING(RightNetBowmansCapsules);
          DEFINE_STATIC_STRING(RightTubules);
          DEFINE_STATIC_STRING(RightNetTubules);
          DEFINE_STATIC_STRING(RightUreter);

          // Blood
          DEFINE_STATIC_STRING(LeftAortaConnection);
          DEFINE_STATIC_STRING(LeftRenalArtery);
          DEFINE_STATIC_STRING(LeftAfferentArteriole);
          DEFINE_STATIC_STRING(LeftGlomerularCapillaries);
          DEFINE_STATIC_STRING(LeftNetGlomerularCapillaries);
          DEFINE_STATIC_STRING(LeftEfferentArteriole);
          DEFINE_STATIC_STRING(LeftPeritubularCapillaries);
          DEFINE_STATIC_STRING(LeftNetPeritubularCapillaries);
          DEFINE_STATIC_STRING(LeftRenalVein);
          DEFINE_STATIC_STRING(LeftVenaCavaConnection);
          // Urine
          DEFINE_STATIC_STRING(LeftNetBowmansCapsules);
          DEFINE_STATIC_STRING(LeftBowmansCapsules);
          DEFINE_STATIC_STRING(LeftTubules);
          DEFINE_STATIC_STRING(LeftNetTubules);
          DEFINE_STATIC_STRING(LeftUreter);

          DEFINE_STATIC_STRING(Bladder);

          DEFINE_STATIC_STRING(Ground);
        };

        class RenalPath
        {
        public:
          DEFINE_STATIC_STRING(RightAortaConnectionToRenalArtery);
          DEFINE_STATIC_STRING(RightRenalArteryToAfferentArteriole);
          DEFINE_STATIC_STRING(RightRenalArteryCompliance);
          DEFINE_STATIC_STRING(RightAfferentArterioleToGlomerularCapillaries);
          DEFINE_STATIC_STRING(RightGlomerularCapillariesToEfferentArteriole);
          DEFINE_STATIC_STRING(RightGlomerularCapillariesCompliance);
          DEFINE_STATIC_STRING(RightEfferentArterioleToPeritubularCapillaries);
          DEFINE_STATIC_STRING(RightPeritubularCapillariesToRenalVein);
          DEFINE_STATIC_STRING(RightRenalVeinToVenaCavaConnection);
          DEFINE_STATIC_STRING(RightRenalVeinCompliance);

          DEFINE_STATIC_STRING(RightGlomerularCapillariesToNetGlomerularCapillaries);
          DEFINE_STATIC_STRING(RightNetGlomerularCapillariesToNetBowmansCapsules);
          DEFINE_STATIC_STRING(RightBowmansCapsulesToNetBowmansCapsules);
          DEFINE_STATIC_STRING(RightBowmansCapsulesToTubules);
          DEFINE_STATIC_STRING(RightTubulesToUreter);
          DEFINE_STATIC_STRING(RightTubulesToNetTubules);
          DEFINE_STATIC_STRING(RightNetTubulesToNetPeritubularCapillaries);
          DEFINE_STATIC_STRING(RightPeritubularCapillariesToNetPeritubularCapillaries);
          DEFINE_STATIC_STRING(RightUreterToBladder);

          DEFINE_STATIC_STRING(LeftAortaConnectionToRenalArtery);
          DEFINE_STATIC_STRING(LeftRenalArteryToAfferentArteriole);
          DEFINE_STATIC_STRING(LeftRenalArteryCompliance);
          DEFINE_STATIC_STRING(LeftAfferentArterioleToGlomerularCapillaries);
          DEFINE_STATIC_STRING(LeftGlomerularCapillariesToEfferentArteriole);
          DEFINE_STATIC_STRING(LeftGlomerularCapillariesCompliance);
          DEFINE_STATIC_STRING(LeftEfferentArterioleToPeritubularCapillaries);
          DEFINE_STATIC_STRING(LeftPeritubularCapillariesToRenalVein);
          DEFINE_STATIC_STRING(LeftRenalVeinToVenaCavaConnection);
          DEFINE_STATIC_STRING(LeftRenalVeinCompliance);

          DEFINE_STATIC_STRING(LeftGlomerularCapillariesToNetGlomerularCapillaries);
          DEFINE_STATIC_STRING(LeftNetGlomerularCapillariesToNetBowmansCapsules);
          DEFINE_STATIC_STRING(LeftBowmansCapsulesToNetBowmansCapsules);
          DEFINE_STATIC_STRING(LeftBowmansCapsulesToTubules);
          DEFINE_STATIC_STRING(LeftTubulesToUreter);
          DEFINE_STATIC_STRING(LeftTubulesToNetTubules);
          DEFINE_STATIC_STRING(LeftNetTubulesToNetPeritubularCapillaries);
          DEFINE_STATIC_STRING(LeftPeritubularCapillariesToNetPeritubularCapillaries);
          DEFINE_STATIC_STRING(LeftUreterToBladder);

          DEFINE_STATIC_STRING(BladderToGroundPressure);
          DEFINE_STATIC_STRING(BladderToGroundUrinate);
        };

        class DigestionNode
        {
        public:
          DEFINE_STATIC_STRING(GutChyme);
        };

        class DigestionPath
        {
        public:
          DEFINE_STATIC_STRING(GutChymeToSmallIntestineVascular);
        };
      }
    }
  }
}
