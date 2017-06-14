#include "stdafx.h"
#include <strstream>

SEArrayVolumePerTime::SEArrayVolumePerTime() : SEArray()
{

}

std::shared_ptr<CDM::ArrayData> SEArrayVolumePerTime::Unload()
{
	if(!IsValid())
		return std::shared_ptr<CDM::ArrayData>((CDM::ArrayData*)NULL);
	std::shared_ptr<CDM::ArrayData> data(new CDM::ArrayVolumePerTimeData());
	SEArray::Unload(data);
	return data;
}

bool SEArrayVolumePerTime::IsValidUnit(CCompoundUnit&  unit)
{
	return CompatibleUnits(SEScalarVolumePerTime::L_Per_s,unit);
}