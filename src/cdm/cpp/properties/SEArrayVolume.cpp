#include "stdafx.h"
#include <strstream>

SEArrayVolume::SEArrayVolume() : SEArray()
{

}

std::shared_ptr<CDM::ArrayData> SEArrayVolume::Unload()
{
	if(!IsValid())
		return std::shared_ptr<CDM::ArrayData>((CDM::ArrayData*)NULL);
	std::shared_ptr<CDM::ArrayData> data(new CDM::ArrayVolumeData());
	SEArray::Unload(data);
	return data;
}

bool SEArrayVolume::IsValidUnit(CCompoundUnit&  unit)
{
	return CompatibleUnits(SEScalarVolume::L,unit);
}