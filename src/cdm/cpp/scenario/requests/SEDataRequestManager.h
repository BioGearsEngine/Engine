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
CDM_BIND_DECL(DataRequestsData)
#include "scenario/requests/SEEnvironmentDataRequest.h"
#include "scenario/requests/SEEquipmentDataRequest.h"
#include "scenario/requests/SEPatientDataRequest.h"
#include "scenario/requests/SEPhysiologyDataRequest.h"
#include "scenario/requests/SESubstanceDataRequest.h"
#include "scenario/requests/SEGasCompartmentDataRequest.h"
#include "scenario/requests/SELiquidCompartmentDataRequest.h"
#include "scenario/requests/SEThermalCompartmentDataRequest.h"
#include "scenario/requests/SETissueCompartmentDataRequest.h"

class DLL_DECL SEDataRequestManager : public Loggable
{
public:
  SEDataRequestManager(Logger* logger);
  ~SEDataRequestManager();

  void Clear();

  bool Load(const CDM::DataRequestsData& in, SESubstanceManager& subMgr);
  CDM::DataRequestsData* Unload() const;
protected:
  void Unload(CDM::DataRequestsData& data)const;

public:
  bool HasResultsFilename() const { return !m_ResultsFile.empty(); }
  std::string GetResultFilename() const { return m_ResultsFile; }
  void SetResultsFilename(const std::string& name) { m_ResultsFile = name; }

  double GetSamplesPerSecond() const { return m_SamplesPerSecond; }
  void SetSamplesPerSecond(double num) { m_SamplesPerSecond = num; }

  bool HasDataRequests() { return !m_Requests.empty(); }
  const std::vector<SEDataRequest*>& GetDataRequests() { return m_Requests; }

  virtual bool HasDefaultDecimalFormatting() const;
  virtual SEDecimalFormat& GetDefaultDecimalFormatting();
  virtual void RemoveDefaultDecimalFormatting();

  virtual bool HasOverrideDecimalFormatting() const;
  virtual SEDecimalFormat& GetOverrideDecimalFormatting();
  virtual void RemoveOverrideDecimalFormatting();

  SEEnvironmentDataRequest&        CreateEnvironmentDataRequest(const SEDecimalFormat* dfault = nullptr);
  SEEquipmentDataRequest&          CreateEquipmentDataRequest(const SEDecimalFormat* dfault = nullptr);
  SEPatientDataRequest&            CreatePatientDataRequest(const SEDecimalFormat* dfault = nullptr);
  SEPhysiologyDataRequest&         CreatePhysiologyDataRequest(const SEDecimalFormat* dfault = nullptr);
  SESubstanceDataRequest&          CreateSubstanceDataRequest(const SEDecimalFormat* dfault = nullptr);

  SEGasCompartmentDataRequest&     CreateGasCompartmentDataRequest(const SEDecimalFormat* dfault = nullptr);
  SELiquidCompartmentDataRequest&  CreateLiquidCompartmentDataRequest(const SEDecimalFormat* dfault = nullptr);
  SEThermalCompartmentDataRequest& CreateThermalCompartmentDataRequest(const SEDecimalFormat* dfault = nullptr);
  SETissueCompartmentDataRequest&  CreateTissueCompartmentDataRequest(const SEDecimalFormat* dfault = nullptr);
  
protected:

  double                       m_SamplesPerSecond;
  std::vector<SEDataRequest*>  m_Requests;
  std::string                  m_ResultsFile;

  SEDecimalFormat*             m_DefaultDecimalFormatting;
  SEDecimalFormat*             m_OverrideDecimalFormatting;

  static SEDataRequest* newFromBind(const CDM::DataRequestData& dataRequest, SESubstanceManager& substances, const SEDecimalFormat* dfault = nullptr);
};