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
CDM_BIND_DECL(SubstanceData)
#include "bind/enumSubstanceState.hxx"

#include "substance/SESubstanceAerosolization.h"
#include "substance/SESubstanceClearance.h"
#include "substance/SESubstancePharmacokinetics.h"
#include "substance/SESubstancePharmacodynamics.h"

class DLL_DECL SESubstance : public Loggable
{
public:

	SESubstance(Logger* logger);
	virtual ~SESubstance();

  virtual void Clear();

	virtual const SEScalar* GetScalar(const std::string& name);

	virtual bool Load(const CDM::SubstanceData& in);
	virtual CDM::SubstanceData* Unload() const;
protected:
	virtual void Unload(CDM::SubstanceData& data) const;

public:
	
	virtual std::string GetName() const;
	virtual void SetName(const std::string& name);
	virtual bool HasName() const;
	virtual void InvalidateName();

	virtual CDM::enumSubstanceState::value GetState() const;
	virtual void SetState(CDM::enumSubstanceState::value state);
	virtual bool HasState() const;
	virtual void InvalidateState();

	virtual bool HasDensity() const;
	virtual SEScalarMassPerVolume& GetDensity();
  virtual double GetDensity(const MassPerVolumeUnit& unit) const;

	virtual bool HasMolarMass() const;
	virtual SEScalarMassPerAmount& GetMolarMass();
  virtual double GetMolarMass(const MassPerAmountUnit& unit) const;

  // Diffusion-ish
  virtual bool HasMaximumDiffusionFlux() const;
  virtual SEScalarMassPerAreaTime& GetMaximumDiffusionFlux();
  virtual double GetMaximumDiffusionFlux(const MassPerAreaTimeUnit& unit) const;

  virtual bool HasMichaelisCoefficient() const;
  virtual SEScalar& GetMichaelisCoefficient();
  virtual double GetMichaelisCoefficient() const;

	// Liquid-ish
	virtual bool HasAerosolization() const;
	virtual SESubstanceAerosolization& GetAerosolization();
	virtual const SESubstanceAerosolization* GetAerosolization() const;
	virtual void RemoveAerosolization();

  virtual bool HasBloodConcentration() const;
  virtual SEScalarMassPerVolume& GetBloodConcentration();
  virtual double GetBloodConcentration(const MassPerVolumeUnit& unit) const;

	virtual bool HasMassInBody() const;
	virtual SEScalarMass& GetMassInBody();
  virtual double GetMassInBody(const MassUnit& unit) const;

	virtual bool HasMassInBlood() const;
	virtual SEScalarMass& GetMassInBlood();
  virtual double GetMassInBlood(const MassUnit& unit) const;

	virtual bool HasMassInTissue() const;
	virtual SEScalarMass& GetMassInTissue();
  virtual double GetMassInTissue(const MassUnit& unit) const;

	virtual bool HasPlasmaConcentration() const;
	virtual SEScalarMassPerVolume& GetPlasmaConcentration();
  virtual double GetPlasmaConcentration(const MassPerVolumeUnit& unit) const;

	virtual bool HasSystemicMassCleared() const;
	virtual SEScalarMass& GetSystemicMassCleared();
  virtual double GetSystemicMassCleared(const MassUnit& unit) const;

  virtual bool HasTissueConcentration() const;
  virtual SEScalarMassPerVolume& GetTissueConcentration();
  virtual double GetTissueConcentration(const MassPerVolumeUnit& unit) const;

	// Gas-ish
	virtual bool HasAlveolarTransfer() const;
	virtual SEScalarVolumePerTime& GetAlveolarTransfer();
  virtual double GetAlveolarTransfer(const VolumePerTimeUnit& unit) const;

	virtual bool HasDiffusingCapacity() const;
	virtual SEScalarVolumePerTimePressure& GetDiffusingCapacity();
  virtual double GetDiffusingCapacity(const VolumePerTimePressureUnit& unit) const;

	virtual bool HasEndTidalFraction() const;
	virtual SEScalarFraction& GetEndTidalFraction();
	virtual double GetEndTidalFraction() const;

  virtual bool HasEndTidalPressure() const;
  virtual SEScalarPressure& GetEndTidalPressure();
  virtual double GetEndTidalPressure(const PressureUnit& unit) const;

	virtual bool HasRelativeDiffusionCoefficient() const;
	virtual SEScalar& GetRelativeDiffusionCoefficient();
	virtual double GetRelativeDiffusionCoefficient() const;

	virtual bool HasSolubilityCoefficient() const;
	virtual SEScalarInversePressure& GetSolubilityCoefficient();
  virtual double GetSolubilityCoefficient(const InversePressureUnit& unit) const;

	virtual bool HasClearance() const;
	virtual SESubstanceClearance& GetClearance();
	virtual const SESubstanceClearance* GetClearance() const;
	virtual void RemoveClearance();

	virtual bool HasPK() const;
	virtual SESubstancePharmacokinetics& GetPK();
	virtual const SESubstancePharmacokinetics* GetPK() const;
	virtual void RemovePK();

	virtual bool HasPD() const;
	virtual SESubstancePharmacodynamics& GetPD();
	virtual const SESubstancePharmacodynamics* GetPD() const;
	virtual void RemovePD();

	
protected: 

	std::string                       m_Name;
	CDM::enumSubstanceState::value    m_State;
	SEScalarMassPerVolume*            m_Density;
	SEScalarMassPerAmount*            m_MolarMass;

  SEScalarMassPerAreaTime*          m_MaximumDiffusionFlux;
  SEScalar*                         m_MichaelisCoefficient;

	SESubstanceAerosolization*        m_Aerosolization;
  SEScalarMassPerVolume*   	        m_BloodConcentration;
	SEScalarMass*				              m_MassInBody;
	SEScalarMass*				              m_MassInBlood;
	SEScalarMass*				              m_MassInTissue;
	SEScalarMassPerVolume*   	        m_PlasmaConcentration;
	SEScalarMass*				              m_SystemicMassCleared;
  SEScalarMassPerVolume*   	        m_TissueConcentration;

	SEScalarVolumePerTime*            m_AlveolarTransfer;
	SEScalarVolumePerTimePressure*    m_DiffusingCapacity;
	SEScalarFraction*			            m_EndTidalFraction;
  SEScalarPressure*                 m_EndTidalPressure;
	SEScalar*					                m_RelativeDiffusionCoefficient;
	SEScalarInversePressure*          m_SolubilityCoefficient;

	SESubstanceClearance*             m_Clearance;
	SESubstancePharmacokinetics*      m_PK;
	SESubstancePharmacodynamics*      m_PD;
};