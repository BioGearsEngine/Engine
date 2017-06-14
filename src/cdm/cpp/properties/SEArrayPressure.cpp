#include "stdafx.h"
#include <strstream>

SEArrayPressure::SEArrayPressure() : SEArray()
{

}

std::shared_ptr<CDM::ArrayData> SEArrayPressure::Unload()
{
	if(!IsValid())
		return std::shared_ptr<CDM::ArrayData>((CDM::ArrayData*)NULL);
	std::shared_ptr<CDM::ArrayData> data(new CDM::ArrayPressureData());
	SEArray::Unload(data);
	return data;
}

bool SEArrayPressure::IsValidUnit(CCompoundUnit&  unit)
{
	return CompatibleUnits(SEScalarPressure::Pa,unit);
}