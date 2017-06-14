#pragma once
#include "SEProperty.h"

class DLL_DECL SEArrayVolume : public SEArray
{
protected:
	std::string m_unit;
	std::vector<double> m_dData;

public:
	SEArrayVolume();

	std::shared_ptr<CDM::ArrayData> Unload();

	virtual bool IsValidUnit(CCompoundUnit& unit);
};