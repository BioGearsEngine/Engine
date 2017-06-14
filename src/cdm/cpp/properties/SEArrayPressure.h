#pragma once
#include "SEProperty.h"

class DLL_DECL SEArrayPressure : public SEArray
{
public:
	SEArrayPressure();

	std::shared_ptr<CDM::ArrayData> SEArrayPressure::Unload();

	virtual bool IsValidUnit(CCompoundUnit& unit);
};