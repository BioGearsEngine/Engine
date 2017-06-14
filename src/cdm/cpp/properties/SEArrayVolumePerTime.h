#pragma once
#include "SEProperty.h"

class DLL_DECL SEArrayVolumePerTime : public SEArray
{
public:
	SEArrayVolumePerTime();

	std::shared_ptr<CDM::ArrayData> Unload();

	virtual bool IsValidUnit(CCompoundUnit& unit);
};