#ifndef __XPMAP_INL__
#define __XPMAP_INL__

//-----------------------------------------------------------------
//-----------------------------------------------------------------
//				CXpMap Construction/Destruction
//-----------------------------------------------------------------
//-----------------------------------------------------------------
template <class KEY, class TYPE>
CXpMap<KEY, TYPE>::CXpMap(const int nGrowBy /*= 10*/)
{
	m_SortedArray.SetGrowBy(nGrowBy);
}

template <class KEY, class TYPE>
CXpMap<KEY, TYPE>::CXpMap(const CXpMap<KEY, TYPE>& Map)
{
	m_SortedArray.SetGrowBy(Map.m_SortedArray.GetGrowBy());

	operator=(Map);
}

template <class KEY, class TYPE>
CXpMap<KEY, TYPE>::~CXpMap()
{
	RemoveAll();
}

//-----------------------------------------------------------------
//				CXpMap Public Implementation
//-----------------------------------------------------------------
template <class KEY, class TYPE>
INT_PTR CXpMap<KEY, TYPE>::Insert(const KEY& Key, const TYPE& Value)
{
	INT_PTR nIndex = m_SortedArray.GetCount();

	if(BinarySearch(Key, nIndex))
	{
		// Duplicate
		m_SortedArray[nIndex]->m_Value = Value;
	}
	else
	{
		SNode* pNode = new SNode(Key, Value);

		if(pNode)
		{
			m_SortedArray.Insert(nIndex, pNode);  
		}
	}

	return(nIndex);
}

template <class KEY, class TYPE>
void CXpMap<KEY, TYPE>::Remove(const KEY& Key)
{
	INT_PTR nIndex = -1;

	if(BinarySearch(Key, nIndex))
	{
		SNode* pNode = m_SortedArray[nIndex];

		delete pNode;
		pNode = NULL;

		m_SortedArray.Remove(nIndex);
	}
}

template <class KEY, class TYPE>
void CXpMap<KEY, TYPE>::RemoveAll()
{
	const INT_PTR nCount = m_SortedArray.GetCount();
	INT_PTR i = 0;

	for(i = 0; i < nCount; i ++)
	{
		if(m_SortedArray[i])
		{
			delete m_SortedArray[i];
		}
	}

	m_SortedArray.RemoveAll();
}

template <class KEY, class TYPE>
inline INT_PTR CXpMap<KEY, TYPE>::GetCount() const
{
	return(m_SortedArray.GetCount());
}

template <class KEY, class TYPE>
inline bool CXpMap<KEY, TYPE>::HasKey(const KEY& Key) const
{
	return(-1 != FindKey(Key));
}

template <class KEY, class TYPE>
inline INT_PTR CXpMap<KEY, TYPE>::FindKey(const KEY& Key) const
{
	INT_PTR nIndex = 0; 

	if(!BinarySearch(Key, nIndex))
	{
		nIndex = -1;
	}

	return(nIndex);
}

template <class KEY, class TYPE>
inline bool CXpMap<KEY, TYPE>::FindValue(const KEY& Key, TYPE& Value) const
{
	INT_PTR nIndex = 0; 
	bool bSuccess = false;;

	if(BinarySearch(Key, nIndex))
	{
		Value = m_SortedArray[nIndex]->m_Value;
		bSuccess = true;
	}

	return(bSuccess);
}

template <class KEY, class TYPE>
inline const KEY& CXpMap<KEY, TYPE>::GetKey(const INT_PTR nIndex) const
{
	return(m_SortedArray[nIndex]->m_Key);
}

template <class KEY, class TYPE>
inline KEY& CXpMap<KEY, TYPE>::GetKey(const INT_PTR nIndex)
{
	return(m_SortedArray[nIndex]->m_Key);
}

template <class KEY, class TYPE>
inline const TYPE& CXpMap<KEY, TYPE>::GetValue(const INT_PTR nIndex) const
{
	return(m_SortedArray[nIndex]->m_Value);
}

template <class KEY, class TYPE>
inline TYPE& CXpMap<KEY, TYPE>::GetValue(const INT_PTR nIndex)
{
	return(m_SortedArray[nIndex]->m_Value);
}

template <class KEY, class TYPE>
inline void CXpMap<KEY, TYPE>::GetPair(const INT_PTR nIndex, KEY& Key, TYPE& Value) const
{
	Key = m_SortedArray[nIndex]->m_Key;
	Value = m_SortedArray[nIndex]->m_Value;
}

template <class KEY, class TYPE>
inline CXpMap<KEY, TYPE> CXpMap<KEY, TYPE>::Union(const CXpMap<KEY, TYPE>& Right) const
{
	CXpMap Result;
	const CXpMap<KEY, TYPE>& Left = *this;
	const INT_PTR nLeftCount = Left.GetCount();
	const INT_PTR nRightCount = Right.GetCount();
	INT_PTR i = 0;

	for(i = 0; i < nLeftCount; i ++)
	{
		Result.Insert(Left.GetKey(i), Left.GetValue(i));
	}

	for(i = 0; i < nRightCount; i ++)
	{
		Result.Insert(Right.GetKey(i), Right.GetValue(i));
	}

	return(Result);
}

template <class KEY, class TYPE>
inline CXpMap<KEY, TYPE> CXpMap<KEY, TYPE>::Difference(const CXpMap<KEY, TYPE>& Right) const
{
	CXpMap Result;
	const CXpMap<KEY, TYPE>& Left = *this;
	const INT_PTR nLeftCount = Left.GetCount();
	const INT_PTR nRightCount = Right.GetCount();
	INT_PTR i = 0;

	for(i = 0; i < nLeftCount; i ++)
	{
		if(!Right.HasKey(Left.GetKey(i)))
		{
			Result.Insert(Left.GetKey(i), Left.GetValue(i));
		}		
	}

	for(i = 0; i < nRightCount; i ++)
	{
		if(!Left.HasKey(Right.GetKey(i)))
		{
			Result.Insert(Right.GetKey(i), Right.GetValue(i));
		}		
	}

	return(Result);
}

template <class KEY, class TYPE>
inline CXpMap<KEY, TYPE> CXpMap<KEY, TYPE>::Intersection(const CXpMap<KEY, TYPE>& Right) const
{
	CXpMap Result;
	const CXpMap<KEY, TYPE>& Left = *this;
	const INT_PTR nLeftCount = Left.GetCount();
	const INT_PTR nRightCount = Right.GetCount();
	INT_PTR i = 0;
	
	for(i = 0; i < nLeftCount; i ++)
	{
		if(Right.HasKey(Left.GetKey(i)))
		{
			Result.Insert(Left.GetKey(i), Left.GetValue(i));
		}		
	}
	
	return(Result);
}

template <class KEY, class TYPE>
inline const TYPE& CXpMap<KEY, TYPE>::operator[](const INT_PTR nIndex) const
{
	return(m_SortedArray[nIndex]->m_Value);
}

template <class KEY, class TYPE>
inline TYPE& CXpMap<KEY, TYPE>::operator[](const INT_PTR nIndex)
{
	return(m_SortedArray[nIndex]->m_Value);
}

template <class KEY, class TYPE>
HRESULT CXpMap<KEY, TYPE>::Copy(const CXpMap<KEY, TYPE>& Map)
{
	HRESULT hr = S_OK;
	const INT_PTR nCount = Map.m_SortedArray.GetCount();
	INT_PTR i = 0;
	
	RemoveAll();
	
	hr = m_SortedArray.SetCount(nCount);

	if(SUCCEEDED(hr))
	{
		for(i = 0; i < nCount; i ++)
		{
			m_SortedArray[i] = NULL;
		}

		for(i = 0; i < nCount; i ++)
		{
			m_SortedArray[i] = new SNode(Map.m_SortedArray[i]->m_Key, Map.m_SortedArray[i]->m_Value);

			if(!m_SortedArray[i])
			{
				RemoveAll();
				hr = HRESULT_FROM_WIN32(ERROR_NOT_ENOUGH_MEMORY);
				break;
			}
		}
	}

	return(hr);
}

template <class KEY, class TYPE>
bool CXpMap<KEY, TYPE>::operator==(const CXpMap& Map) const
{
	bool bEqual = false;
	const INT_PTR nCount = m_SortedArray.GetCount();

	if(nCount == Map.m_SortedArray.GetCount())
	{
		bEqual = true;

		for(INT_PTR i = 0; i < nCount; i ++)
		{
			if( (!(m_SortedArray[i]->m_Key == Map.m_SortedArray[i]->m_Key)) ||
				(!(m_SortedArray[i]->m_Value == Map.m_SortedArray[i]->m_Value)) )
			{
				bEqual = false;
				break;
			}
		}
	}

	return(bEqual);
}

template <class KEY, class TYPE>
CXpMap<KEY, TYPE>& CXpMap<KEY, TYPE>::operator=(const CXpMap<KEY, TYPE>& Map)
{
	Copy(Map);

	return(*this);
}

//-----------------------------------------------------------------
//				CXpMap Protected Implementation
//-----------------------------------------------------------------
template <class KEY, class TYPE>
bool CXpMap<KEY, TYPE>::BinarySearch(const KEY& Key, INT_PTR& nIndex) const
{
	bool bFound = false;
	const INT_PTR nCount = m_SortedArray.GetCount();

	nIndex = 0;

	if(nCount)
	{
		INT_PTR nHigh = nCount;
		INT_PTR nLow = -1;

		while(1 < (nHigh - nLow))
		{
			nIndex = (nHigh + nLow) / 2;

			const KEY& TestKey = m_SortedArray[nIndex]->m_Key;

			if(Key <= TestKey)
			{
				nHigh = nIndex;
			}
			else
			{
				nLow  = nIndex;
			}
		}

		nIndex = nHigh;

		if(nHigh < nCount)
		{
			bFound = Key == m_SortedArray[nHigh]->m_Key;
		}
	}

	return(bFound);
}

#endif //__XPMAP_INL__

