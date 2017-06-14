#pragma once
#include "SEProperty.h"

class DLL_DECL SEArray : public SEProperty
{
protected:	
	std::vector<double> m_dData;
	std::shared_ptr<CCompoundUnit> m_unit;

public:
	SEArray();
	~SEArray();

	virtual std::shared_ptr<CDM::PropertyData> UnloadData();
	std::shared_ptr<CDM::ArrayData> Unload();
	void Unload(std::shared_ptr<CDM::ArrayData> data);
	bool Load(std::shared_ptr<CDM::ArrayData> in);

	virtual bool IsValid();
	virtual void Invalidate();

	virtual bool IsValidUnit(const std::string& unit) { return true; };
	virtual bool IsValidUnit(const std::shared_ptr<CCompoundUnit>& unit);
	// TODO std::string GetUnit() { return m_unit; }

	unsigned int Size();
	
	std::string GetStringData(const std::string& unit=SEScalar::unitless);
	bool SetStringData(const std::string& data, const std::string& unit=SEScalar::unitless);

	bool SetDoubleData(double *data, int len, const std::string& unit=SEScalar::unitless);
	bool GetDoubleData(double *&data, int &len, const std::string& unit=SEScalar::unitless);

	double GetIndex(unsigned int idx, const std::string& unit=SEScalar::unitless);
	void   SetIndex(unsigned int idx, double value, const std::string& unit=SEScalar::unitless);
	bool   Append(double value, const std::string& unit=SEScalar::unitless);

	bool SetVectorData(std::vector<double> data, const std::string&  unit=SEScalar::unitless);
	bool GetVectorData(std::vector<double> data, const std::string&  unit=SEScalar::unitless);
};
//STL_DECL template class DLL_DECL std::vector<double>;