// This file is part of the ObjectFusion cross platform (XP)
// class library.
// Copyright (C) 1998 ObjectFusion, L.L.C.
// All rights reserved.

#ifndef __XPHASHTABLE_INL__
#define __XPHASHTABLE_INL__

#include "math.h"

#if defined(ASSERT)
	// Use the MFC version if it has been defined.
	#define XpHashTableAssert ASSERT
#elif defined(assert)
	// Use the standard c library version.
	#define XpHashTableAssert assert
#else		 
	// No assert present.
	#define XpHashTableAssert(Expression) 
#endif 

inline unsigned long XpHashKey(const char* pszString)
{
    unsigned long dwHash = 0;

    while(*pszString)
    {
        dwHash = (dwHash << 5) + dwHash + *pszString;
        pszString ++;
    }

    return(dwHash);
}

inline unsigned long XpHashKey(const wchar_t* pszString)
{
    unsigned long dwHash = 0;

    while(*pszString)
    {
        dwHash = (dwHash << 5) + dwHash + *pszString;
        pszString ++;
    }

    return(dwHash);
}

#ifdef  __AFX_H__
inline unsigned long XpHashKey(const CString& csString)
{
    unsigned long dwHash = 0;
    INT_PTR nIndex = 0;
    const TCHAR* pszString = csString;

    while(pszString[nIndex])
    {
        dwHash = (dwHash << 5) + dwHash + pszString[nIndex];
        nIndex ++;
    }

    return(dwHash);
}

#endif // __AFX_H__	




//-----------------------------------------------------------------
//-----------------------------------------------------------------
//				CXpHashTable Construction/Destruction
//-----------------------------------------------------------------
//-----------------------------------------------------------------
template <class KEY, class TYPE>
CXpHashTable<KEY, TYPE>::CXpHashTable(const INT_PTR nHashTableSize,
                                      const bool bCalculate /*= true*/)
    :   m_nCount(0)
{
    INT_PTR nBucketCount = nHashTableSize;
                       
    if(bCalculate)
    {
        nBucketCount = GetClosestPrime(nHashTableSize);
    }

    if(SUCCEEDED(m_Buckets.SetCount(nBucketCount)))
    {
        for(INT_PTR i = 0; i < nBucketCount; i ++)
        {
            m_Buckets[i] = NULL;
        }
    }        
}

template <class KEY, class TYPE>
CXpHashTable<KEY, TYPE>::~CXpHashTable()
{
    RemoveAll();
}
//-----------------------------------------------------------------
//				CXpHashTable Public Implementation
//-----------------------------------------------------------------
template <class KEY, class TYPE>
HRESULT CXpHashTable<KEY, TYPE>::Insert(const KEY& Key, const TYPE& Value)
{
    HRESULT hr = HRESULT_FROM_WIN32(ERROR_NOT_ENOUGH_MEMORY);
    TXpTableItem Item = FindKey(Key);

    if(m_Buckets.GetCount())
    {
        if(!Item)
        {
            const INT_PTR nIndex = GetBucket(Key);
            SNode* pNode = new SNode(Key, Value);

            if(pNode)
            {
                SNode* pHead = m_Buckets[nIndex];

                pNode->m_pNext = pHead;
                m_Buckets[nIndex] = pNode;

                m_nCount ++;
                hr = S_OK;
            }
        }
        else
        {
            SNode* pNode = (SNode*)Item; 

            XpHashTableAssert(pNode->m_Key == Key);

            pNode->m_Value = Value;
            hr = S_OK;
        }
    }        

    return(hr);
}

template <class KEY, class TYPE>
HRESULT CXpHashTable<KEY, TYPE>::Remove(const KEY& Key)
{
    HRESULT hr = HRESULT_FROM_WIN32(ERROR_INVALID_ADDRESS);
    const INT_PTR nIndex = GetBucket(Key);
    SNode* pPrevious = NULL;
    SNode* pNode = m_Buckets[nIndex];

    while(pNode)
    {
        if(pNode->m_Key == Key)
        {
            if(pPrevious)
            {
                pPrevious->m_pNext = pNode->m_pNext;
            }
            else
            {
                m_Buckets[nIndex] = pNode->m_pNext;
            }

            delete pNode;
            m_nCount --;
            hr = S_OK;
            break;
        }

        pPrevious = pNode;
        pNode = pPrevious->m_pNext;
    }
    
    return(hr);
}

template <class KEY, class TYPE>
HRESULT CXpHashTable<KEY, TYPE>::Intersection(const CXpHashTable& HashTable, CXpHashTable& Result)
{
    HRESULT hr = S_OK;
    
    Result.RemoveAll();
    
    TXpTableItem Item = GetFirst();
        
    while(Item)
    {
        if(HashTable.HasKey(GetKey(Item)))
        {
            hr = Result.Insert(GetKey(Item), GetValue(Item));
            
            if(FAILED(hr))
            {
                break;
            }
        }
        
        Item = GetNext(Item);
    }
    
    return(hr);
}

template <class KEY, class TYPE>
HRESULT CXpHashTable<KEY, TYPE>::Union(const CXpHashTable& HashTable, CXpHashTable& Result)
{
    HRESULT hr = S_OK;
    
    Result.RemoveAll();
    
    hr = Result.Copy(HashTable);
    
    if(SUCCEEDED(hr))
    {
        TXpTableItem Item = GetFirst();
            
        while(Item)
        {
            hr = Result.Insert(GetKey(Item), GetValue(Item));
                
            if(FAILED(hr))
            {
                break;
            }
           
            Item = GetNext(Item);
        }
    }        
    
    return(hr);
}

template <class KEY, class TYPE>
HRESULT CXpHashTable<KEY, TYPE>::Difference(const CXpHashTable& HashTable, CXpHashTable& Result)
{
    HRESULT hr = S_OK;
    
    Result.RemoveAll();
    
    TXpTableItem Item = GetFirst();
        
    while(Item)
    {
        if(!HashTable.HasKey(GetKey(Item)))
        {
            hr = Result.Insert(GetKey(Item), GetValue(Item));
            
            if(FAILED(hr))
            {
                break;
            }
        }
        
        Item = GetNext(Item);
    }
    
    return(hr);
}

template <class KEY, class TYPE>
inline bool CXpHashTable<KEY, TYPE>::operator==(const CXpHashTable& HashTable) const
{
    bool bEqual = true;
    
    if(GetCount() == HashTable.GetCount())
    {
        TXpTableItem Item = GetFirst();
            
        while(Item)
        {
            TXpTableItem TestItem = HashTable.FindKey(GetKey(Item));
            
            if(TestItem)
            {
                if(GetValue(Item) != HashTable.GetValue(TestItem))
                {
                   bEqual = false;
                   break;
                }
            }
            else
            {
                bEqual = false;
                break;
            }
            
            Item = GetNext(Item);
        }    
    
    }
    else
    {
        bEqual = false;
    }
    
    return(bEqual);
}

template <class KEY, class TYPE>
void CXpHashTable<KEY, TYPE>::RemoveAll()
{
    m_nCount = 0;
    const INT_PTR nCount = m_Buckets.GetCount();

    for(INT_PTR i = 0; i < nCount; i ++)
    {
        SNode* pNode = m_Buckets[i];

        m_Buckets[i] = NULL;

        while(pNode)
        {
            SNode* pNext = pNode->m_pNext;

            delete pNode;

            pNode = pNext;
        }
    }
}

template <class KEY, class TYPE>
INT_PTR CXpHashTable<KEY, TYPE>::GetCount() const
{
    return(m_nCount);
}

template <class KEY, class TYPE>
bool CXpHashTable<KEY, TYPE>::HasKey(const KEY& Key) const
{
    return(FindKey(Key) ? true : false);
}

template <class KEY, class TYPE>
TXpTableItem CXpHashTable<KEY, TYPE>::FindKey(const KEY& Key) const
{
    TXpTableItem Item = NULL;
    
    if(m_Buckets.GetCount())
    {
        const INT_PTR nIndex = GetBucket(Key);
        SNode* pNode = m_Buckets[nIndex];

        while(pNode)
        {
            if(pNode->m_Key == Key)
            {
                Item = pNode;
                break;
            }

            pNode = pNode->m_pNext;
        }
    }
    
    return(Item);  
}

template <class KEY, class TYPE>
bool CXpHashTable<KEY, TYPE>::FindValue(const KEY& Key, TYPE& Value) const
{
    TXpTableItem Item = FindKey(Key);
    bool bSuccess = false;

    if(Item)
    {
        SNode* pNode = (SNode*)Item;

        Value = pNode->m_Value;
        bSuccess = true;
    }

    return bSuccess;
}

template <class KEY, class TYPE>
const KEY& CXpHashTable<KEY, TYPE>::GetKey(const TXpTableItem Item) const
{
    XpHashTableAssert(Item);
    return ((SNode*)Item)->m_Key;
}

template <class KEY, class TYPE>
const TYPE& CXpHashTable<KEY, TYPE>::GetValue(const TXpTableItem Item) const
{
    XpHashTableAssert(Item);
    return ((SNode*)Item)->m_Value;
}

template <class KEY, class TYPE>
TYPE& CXpHashTable<KEY, TYPE>::GetValue(const TXpTableItem Item)
{
    XpHashTableAssert(Item);
    return ((SNode*)Item)->m_Value;
}

template <class KEY, class TYPE>
void CXpHashTable<KEY, TYPE>::GetPair(const TXpTableItem Item, KEY& Key, TYPE& Value) const
{
    XpHashTableAssert(Item);
    Key = ((SNode*)Item)->m_Key;
    Value = ((SNode*)Item)->m_Value;
}
                             
template <class KEY, class TYPE>
inline HRESULT CXpHashTable<KEY, TYPE>::Copy(const CXpHashTable& HashTable)
{
    HRESULT hr = S_OK;
    
    if(this != &HashTable)
    {
        const INT_PTR nBucketCount = HashTable.m_Buckets.GetCount();
        
        RemoveAll();
        
        if(SUCCEEDED(m_Buckets.SetCount(nBucketCount)))
        {
            INT_PTR i = 0;
            SNode* pNode = NULL;
            SNode** pPrevious = NULL;
            SNode* pCopy = NULL;  
             
            for(i = 0; i < nBucketCount; i ++)
            {
                m_Buckets[i] = NULL;
            }
            
            for(i = 0; i < nBucketCount; i ++)
            {
                pNode = HashTable.m_Buckets[i];
                pPrevious = &m_Buckets[i];

                while(pNode)
                {
                    pCopy = new SNode(pNode->m_Key, pNode->m_Value);

                    if(pCopy)
                    {
                        *pPrevious = pCopy;
                        pPrevious = &pCopy->m_pNext;
                        m_nCount ++;
                    }
                    else
                    {
                        RemoveAll();
                        hr = HRESULT_FROM_WIN32(ERROR_INVALID_ADDRESS);
                        break; 
                    }

                    pNode = pNode->m_pNext;
                }
            }
        }
        else
        {
            hr = HRESULT_FROM_WIN32(ERROR_NOT_ENOUGH_MEMORY);        
        }            
    }

    return(hr);
}

//-----------------------------------------------------------------
//				ClassName Protected Implementation
//-----------------------------------------------------------------
template <class KEY, class TYPE>
inline INT_PTR CXpHashTable<KEY, TYPE>::GetBucket(const KEY& Key) const
{
    const unsigned long dwHash = XpHashKey(Key);
    INT_PTR nIndex = -1;
    
    if(m_Buckets.GetCount())
    {
        nIndex = dwHash % m_Buckets.GetCount();
    }
    
    return(nIndex);
}

template <class KEY, class TYPE>
inline bool CXpHashTable<KEY, TYPE>::IsPrime(const INT_PTR nNbr)
{
    INT_PTR nIsPrime = nNbr && nNbr != 1 ? true : false;

    if(nIsPrime)
    { 
        INT_PTR nTestDiv = 2;
        const INT_PTR nSquareRoot = (INT_PTR)(sqrt((double)nNbr));

        while( (nIsPrime) && (nTestDiv <= nSquareRoot) )
        {
            nIsPrime = nNbr % nTestDiv;
    
            nTestDiv ++;
        }
    }

    return(nIsPrime ? true : false);
}

template <class KEY, class TYPE>
inline INT_PTR CXpHashTable<KEY, TYPE>::GetClosestPrime(const INT_PTR nNbr)
{
    INT_PTR nPrime = 0;
    static double dMaxInt = pow(static_cast<double>(2.0), static_cast<int>((sizeof(INT_PTR) * 8) - 1));
    static INT_PTR nMaxInt = static_cast<INT_PTR>(dMaxInt) - 1;
            
    for(INT_PTR i = nNbr; i < nMaxInt; i ++)
    {
        if(IsPrime(i))
        {
            // we have a prime, break out of the loop
            nPrime = i;
            break;
        }
    } 

    return(nPrime);
}

template <class KEY, class TYPE>
inline TXpTableItem CXpHashTable<KEY, TYPE>::GetFirst() const
{
    return GetNext(NULL);
}

template <class KEY, class TYPE>
inline TXpTableItem CXpHashTable<KEY, TYPE>::GetNext(const TXpTableItem Item) const
{
    TXpTableItem Next = NULL;
    INT_PTR nStart = 0;

    if(Item)
    {
        const SNode* pNode = (const SNode*)Item;

        Next = pNode->m_pNext;

        if(!Next)
        {
            nStart = GetBucket(pNode->m_Key) + 1;
        }
    }

    if(!Next)
    {
        const INT_PTR nCount = m_Buckets.GetCount();

        for(INT_PTR i = nStart; i < nCount && !Next; i ++)
        {
            Next = m_Buckets[i];
        }
    }

    return Next;
}


#endif //__XPHASHTABLE_INL__