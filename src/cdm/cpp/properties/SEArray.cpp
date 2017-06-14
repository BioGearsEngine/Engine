#include "stdafx.h"
#include <strstream>
#include <string>

SEArray::SEArray() : SEProperty()
{
	m_dData.clear();
	m_unit = "";
}

SEArray::~SEArray()
{
}


void SEArray::Invalidate()
{
	m_dData.clear();
	m_unit="";
}

bool SEArray::IsValid()
{
	if(!IsValidUnit(m_unit))
		return false;
	if(m_dData.size()==0)
		return false;
	return true;
}

std::shared_ptr<CDM::PropertyData> SEArray::UnloadData()
{
	std::shared_ptr<CDM::PropertyData> a(Unload().get());
	return a;
}

bool SEArray::Load(CDM::ArrayData *in)
{
	m_dData.clear();
	//std::string u;
	//if(in->unit().present())
	//{
	//	u = in->unit().get();
	//	if(!this->IsValidUnit(u))
	//		return false;
	//	m_unit = in->unit().get();
	//}
	//Property::Load(in);
	//for(std::vector<double>::size_type i = 0; i < in->value().size(); i++)
	//	m_dData.push_back(in->value()[i]);
	//m_valid = true;
	return true;
}

std::shared_ptr<CDM::ArrayData> SEArray::Unload()
{
	if(!IsValid())
		return std::shared_ptr<CDM::ArrayData>((CDM::ArrayData*)NULL);
	std::shared_ptr<CDM::ArrayData> data(new CDM::ArrayData());
	Unload(data);
	return data;
}

void SEArray::Unload(std::shared_ptr<CDM::ArrayData> data)
{
	SEProperty::Unload(std::shared_ptr<CDM::PropertyData>(data));
	/*if(!m_unit.empty())
	data->unit(m_unit);
	for(std::size_t i=0; i<m_dData.size(); i++)
	data->value().push_back(m_dData[i]);*/
}

unsigned int SEArray::Size()
{
	return m_dData.size();
}

double SEArray::GetIndex(unsigned int idx, const std::string&  unit)
{
	// TODO Convert!
	return m_dData.at(idx);
}

void SEArray::SetIndex(unsigned int idx, double value, const std::string&  unit)
{
	if(m_dData.size()<=idx)
		m_dData.resize(idx+1);
	m_dData[idx]=value;
}

bool SEArray::Append(double value, const std::string&  unit)
{
	if(!IsValidUnit(unit))
		return false; // Probably need exceptions here...
	// Convert if necessary
	m_dData.push_back(value);
	return true;
}


bool SEArray::SetStringData(const std::string&  data, const std::string&  unit)
{
	if(data.c_str() && data.c_str()[0] == '\0')
		return false;
	if(!IsValidUnit(unit))
		return false;
	m_unit  = unit;
	m_dData.clear();

	
	std::string tok;
	std::istringstream iss(data);
	while(getline(iss,tok,','))
	{
		m_dData.push_back(atof(tok.c_str()));
	}

	return true;
}

bool SEArray::SetDoubleData(double *data, int len, const std::string&  unit)
{
	if(!data)
		return false;
	if(!IsValidUnit(unit))
		return false;
	m_unit = unit;
	
	m_dData.clear();
	for(int i=0;i<len;i++)
		m_dData.push_back(data[i]);
	return true;
}

std::string SEArray::GetStringData(const std::string&  unit)
{
	std::string ret = '\0';
	if(!IsValidUnit(unit))
		return ret;
	//bool conv=true;
	//if(CCompoundUnit(m_unit)==CCompoundUnit(unit)||unit.empty())
	//  conv=false;

	//CCompoundUnit ToUnit(unit);
	//CCompoundUnit FromUnit(m_unit);
	//CUnitConversionEngine &UCE = CUnitConversionEngine::GetEngine();
	//
	//std::stringstream str;
	//for(std::size_t i=0;i<m_dData.size();i++)
	//{
	//	if(conv)
	//		str << UCE.ConvertValue(m_dData[i],FromUnit,ToUnit);
	//	else
	//		str << m_dData[i];

	//	if(i!=m_dData.size()-1)
	//		str << ",";
	//}
	//ret = str.str().c_str();
	return ret;
}

/**
 * Returns a Copy of the data, caller is responsable for cleaning up the array recieved
 */
bool SEArray::GetDoubleData(double *&data, int &len, const std::string&  unit)
{
	if(!IsValidUnit(unit))
		return false;
	/*
	data = new double[m_dData.size()];
	if(CCompoundUnit(m_unit)==CCompoundUnit(unit))
	{
		for(std::size_t i=0;i<m_dData.size();i++)
		{
			data[i] = m_dData[i];
		}
	}
	else
	{
		CCompoundUnit ToUnit(unit);
		CCompoundUnit FromUnit(m_unit);
		CUnitConversionEngine &UCE = CUnitConversionEngine::GetEngine();
		for (std::size_t i=0; i<m_dData.size(); i++)
		{
            data[i] = UCE.ConvertValue(m_dData[i],FromUnit,ToUnit);
		}
	}
	len = m_dData.size();*/
	return true;
}

bool SEArray::SetVectorData(std::vector<double> data, const std::string&  unit)
{
	if(!IsValidUnit(unit))
		return false;
	m_unit = unit;
	m_dData.clear();
	for(std::size_t i=0; i<data.size(); i++)
		m_dData.push_back(data[i]);
	return true;
}

bool SEArray::GetVectorData(std::vector<double> data, const std::string&  unit)
{
	if(!IsValidUnit(unit))
		return false;

	/*data.clear();
	if(unit.empty()||CCompoundUnit(m_unit)==CCompoundUnit(unit))
	{
		for(std::size_t i=0;i<m_dData.size();i++)
		{
			data.push_back(m_dData[i]);
		}
	}
	else
	{
		CCompoundUnit ToUnit(unit);
		CCompoundUnit FromUnit(m_unit);
		CUnitConversionEngine &UCE = CUnitConversionEngine::GetEngine();
		for (std::size_t i=0; i<m_dData.size(); i++)
		{
			data.push_back(UCE.ConvertValue(m_dData[i],FromUnit,ToUnit));
		}
	}*/
	return true;
}