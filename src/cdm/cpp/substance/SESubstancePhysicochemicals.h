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
CDM_BIND_DECL(SubstancePhysicochemicalData)
#include "bind/enumSubstanceIonicState.hxx"
#include "bind/enumSubstanceBindingProtein.hxx"

class DLL_DECL SESubstancePhysicochemicals : public Loggable
{
public:

	SESubstancePhysicochemicals(Logger* logger);
	virtual ~SESubstancePhysicochemicals();

  virtual void Clear();
  virtual bool IsValid() const;

	virtual const SEScalar* GetScalar(const std::string& name);

	virtual bool Load(const CDM::SubstancePhysicochemicalData& in);
	virtual CDM::SubstancePhysicochemicalData* Unload() const;
protected:
	virtual void Unload(CDM::SubstancePhysicochemicalData& data) const;

public:
	virtual bool HasAcidDissociationConstant() const;
	virtual SEScalar& GetAcidDissociationConstant();
	virtual double GetAcidDissociationConstant() const;

	virtual CDM::enumSubstanceBindingProtein::value GetBindingProtein() const;
	virtual void SetBindingProtein(CDM::enumSubstanceBindingProtein::value state);
	virtual bool HasBindingProtein() const;
	virtual void InvalidateBindingProtein();

	virtual bool HasBloodPlasmaRatio() const;
	virtual SEScalar& GetBloodPlasmaRatio();
	virtual double GetBloodPlasmaRatio() const;

	virtual bool HasFractionUnboundInPlasma() const;
	virtual SEScalarFraction& GetFractionUnboundInPlasma();
	virtual double GetFractionUnboundInPlasma() const;

	virtual CDM::enumSubstanceIonicState::value GetIonicState() const;
	virtual void SetIonicState(CDM::enumSubstanceIonicState::value state);
	virtual bool HasIonicState() const;
	virtual void InvalidateIonicState();

	virtual bool HasLogP() const;
	virtual SEScalar& GetLogP();
	virtual double GetLogP() const;

	virtual bool HasOralAbsorptionRateConstant() const;
	virtual SEScalar& GetOralAbsorptionRateConstant();
	virtual double GetOralAbsorptionRateConstant() const;

protected: 
	SEScalar*					                      m_AcidDissociationConstant;
	CDM::enumSubstanceBindingProtein::value m_BindingProtein;
	SEScalar*					                      m_BloodPlasmaRatio;
	SEScalarFraction*			                  m_FractionUnboundInPlasma;
	CDM::enumSubstanceIonicState::value	    m_IonicState;
	SEScalar*					                      m_LogP;
	SEScalar*					                      m_OralAbsorptionRateConstant;
};