#ifndef __XPARRAY_INL__
#define __XPARRAY_INL__

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
			 
#if defined(ASSERT)
	// Use the MFC version if it has been defined.
	#define XpArrayVerifyBounds(Index) {ASSERT( (0 <= Index) && (Index < m_nElementCount) );} 
	#define XpArrayAssert ASSERT
#elif defined(assert)
	// Use the standard c library version.
	#define XpArrayVerifyBounds(Index) {assert( (0 <= Index) && (Index < m_nElementCount) );}
	#define XpArrayAssert assert
#else		 
	// No assert present.
	#define XpArrayVerifyBounds(Index) 
	#define XpArrayAssert(Expression) 
#endif 

#ifndef XpMin
	#define XpMin(a, b)  (((a) < (b)) ? (a) : (b))  
	#define XpMax(a, b)  (((a) > (b)) ? (a) : (b)) 
#endif //#ifndef XpMin




//-----------------------------------------------------------------
//-----------------------------------------------------------------
//				CXpArray Construction/Destruction
//-----------------------------------------------------------------
//-----------------------------------------------------------------
template <typename TYPE> 
CXpArray<TYPE>::CXpArray(const INT_PTR nGrowBy /*= 10*/) 
    :   m_pData(NULL),
        m_nElementCount(0),   
        m_nAllocedCount(0),
        m_nGrowBy(XpMax(nGrowBy, 1))
{
}

template <typename TYPE> 
CXpArray<TYPE>::CXpArray(const CXpArray& Array)
    :   m_pData(NULL),
        m_nElementCount(0),   
        m_nAllocedCount(0),
        m_nGrowBy(Array.m_nGrowBy)
{
    Copy(Array);
}

template <typename TYPE> 
CXpArray<TYPE>::~CXpArray()
{
    RemoveAll();
}

template <typename TYPE>
void* CXpArray<TYPE>::m_pfnMoveCallback = NULL;


//-----------------------------------------------------------------
//				CXpArray Public Implementation
//-----------------------------------------------------------------
template <typename TYPE> 
void CXpArray<TYPE>::SetMoveCallback(void (* pfnMoveCallback)(TYPE* pObject, const INT_PTR nCount))
{
    m_pfnMoveCallback = pfnMoveCallback;
}	

template <typename TYPE> 
inline HRESULT CXpArray<TYPE>::Add(const TYPE& Element, INT_PTR* pNewIndex /*= NULL*/)
{                        
    // Append the new element to the end of the array.
    HRESULT hr = S_OK;
    
    hr = Insert(m_nElementCount, Element);
    
    if(SUCCEEDED(hr))
    {
        XpArrayAssert(m_nElementCount);

        if(pNewIndex)
        {
            *pNewIndex = m_nElementCount - 1;
        }        
    }
  
    return(hr);
}

template <typename TYPE> 
inline HRESULT CXpArray<TYPE>::Insert(const INT_PTR nIndex, const TYPE& Element)
{
    HRESULT hr = S_OK;
    const INT_PTR nPreviousCount = m_nElementCount;
    const INT_PTR nNewCount = XpMax(nIndex + 1, m_nElementCount + 1); 

    XpArrayAssert(0 <= nIndex);

    if(0 <= nIndex)
    {
        // Allocate another element(s).  
        hr = Allocate(nNewCount);
        
        if(SUCCEEDED(hr))
        {
            // If the new element is not the last one,
            // move all of the elements to the right of 
            // nIndex by one.
            if( (nPreviousCount) && (nIndex != nPreviousCount) )
            {
                const INT_PTR nMoveCount = nPreviousCount - nIndex;

                // Move all elements to the right one.
                memmove(&m_pData[nIndex + 1], &m_pData[nIndex], nMoveCount * sizeof(TYPE));

                // Inform the object(s) that they have moved.
                if(m_pfnMoveCallback)
                {
                    (*(PFN_MoveCallback*)m_pfnMoveCallback)(&m_pData[nIndex + 1], nMoveCount);
                }

                // Call the constructor of for first object that  
                // was moved. If not the object gets destroyed twice.
                ConstructElement(nIndex);
            }

            // Copy the value.
            m_pData[nIndex] = Element;
        }
    }        
    else
    {
        hr = HRESULT_FROM_WIN32(ERROR_INVALID_ADDRESS);
    }

    return(hr);
}

template <typename TYPE> 
inline HRESULT CXpArray<TYPE>::Remove(const INT_PTR nIndex, const INT_PTR nCount /*= 1*/)
{ 
    HRESULT hr = HRESULT_FROM_WIN32(ERROR_INVALID_ADDRESS);

    if( (0 <= nIndex) &&
        (nIndex < m_nElementCount) && 
        (0 < nCount) )
    {
        const INT_PTR nSafeCount = nIndex + nCount <= m_nElementCount ? nCount : m_nElementCount - nIndex;
        const INT_PTR nMoveCount = m_nElementCount - (nIndex + nSafeCount);

        // Destroy the requested elements.
        DestructElement(nIndex, nSafeCount);

        // If there are elements to the right of what we 
        // destroyed, move them to the left to fill the gap.
        if(nMoveCount)
        {
            memmove(&m_pData[nIndex], &m_pData[nIndex + nSafeCount], nMoveCount * sizeof(TYPE));

            // Inform the object(s) that they have moved.
            if(m_pfnMoveCallback)
            {
                (*(PFN_MoveCallback*)m_pfnMoveCallback)(&m_pData[nIndex], nMoveCount);
            }
        }

        // Adjust the count.
        m_nElementCount -= nSafeCount;
        hr = S_OK;
    } 
    
    return(hr);
}

template <typename TYPE> 
inline void CXpArray<TYPE>::RemoveAll()
{
    if(m_pData)
    {
        // Call the destructors for the objects that are inuse.
        DestructElement(0, m_nElementCount);

        free(m_pData);
        m_pData = NULL;

        m_nAllocedCount = 0;
        m_nElementCount = 0;
    }
}  

template <typename TYPE> 
inline INT_PTR CXpArray<TYPE>::GetCount() const
{
    return(m_nElementCount);
}

template <typename TYPE> 
inline HRESULT CXpArray<TYPE>::SetCount(const INT_PTR nNewCount)
{
    return(Allocate(nNewCount));
}

template <typename TYPE> 
inline INT_PTR CXpArray<TYPE>::GetGrowBy() const
{
    return(m_nGrowBy);
}

template <typename TYPE> 
inline void CXpArray<TYPE>::SetGrowBy(const INT_PTR nGrowBy)
{
    if(0 < nGrowBy)
    {
        m_nGrowBy = nGrowBy;
    }
    else
    {
        m_nGrowBy = 1;
    }
}

template <typename TYPE> 
inline HRESULT CXpArray<TYPE>::PreAlloc(const INT_PTR nCount)
{
    // Destroy any previously allocated objects.
    DestructElement(0, m_nElementCount);

    m_nElementCount = 0;

    // Allocate the objects, but do not construct them.
    return(Allocate(nCount, false));
}

template <typename TYPE> 
inline HRESULT CXpArray<TYPE>::FreeExtra()
{
    HRESULT hr = S_OK;

    // Shrink if needed.
    if(m_nElementCount < m_nAllocedCount)
    {
        TYPE* pPreviousLocation = m_pData;

        // Allocate the exact size needed and none for growth.
        m_pData = (TYPE*)realloc(m_pData, m_nElementCount * sizeof(TYPE));

        if(pPreviousLocation != m_pData)
        {
            if(pPreviousLocation)
            {
                // Inform the object(s) that they have moved.
                if(m_pfnMoveCallback)
                {
                    (*(PFN_MoveCallback*)m_pfnMoveCallback)(m_pData, m_nElementCount);
                }
            }
            else
            {
                // Realloc failed, restore the previous pointer and set the error.
                m_pData = pPreviousLocation;
                hr = HRESULT_FROM_WIN32(ERROR_NOT_ENOUGH_MEMORY);
            }    
        }

        // Save the new allocated size.
        m_nAllocedCount = m_nElementCount;
    }
    return(hr);
}

template <typename TYPE> 
inline HRESULT CXpArray<TYPE>::Append(const CXpArray& Array)
{
	// The original count is saved so that self append
	// will operate correctly.
	const INT_PTR nAppendCount = Array.m_nElementCount;
	const INT_PTR nExistingCount = m_nElementCount;
	const INT_PTR nNewCount = nExistingCount + Array.m_nElementCount;
	HRESULT hr = Allocate(nNewCount);
	
	if(SUCCEEDED(hr))
	{
		for(INT_PTR i = 0; i < nAppendCount; i ++)
		{
			m_pData[nExistingCount + i] = Array.m_pData[i];
		}
	}
    
    return(hr);
}

template <typename TYPE> 
HRESULT CXpArray<TYPE>::Mid(CXpArray& Subset, const INT_PTR nFrom, const INT_PTR nCount /*= -1*/) const
{
    const INT_PTR nMax = GetCount();
    INT_PTR nStart = nFrom;
    INT_PTR nEnd = -1 != nCount ? nFrom + nCount : nMax;
    HRESULT hr = S_OK;

    // Confine the ranges so that we don't
    // go out of bounds!
    nStart = XpMax(nStart, 0);
    nStart = XpMin(nStart, nMax);

    nEnd = XpMin(nEnd, nMax);
    nEnd = XpMax(nStart, nEnd);

    const INT_PTR nLength = nEnd - nStart;
    hr = Subset.SetCount(nLength);

    if(SUCCEEDED(hr))
    {
        for(INT_PTR i = 0; i < nLength; i ++)
        {
            Subset.m_pData[i] = m_pData[nStart + i];
        }
    }

    return(hr);
}

template <typename TYPE> 
HRESULT CXpArray<TYPE>::Left(CXpArray& Subset, const INT_PTR nCount) const
{
	return(Mid(Subset, 0, nCount));
}

template <typename TYPE> 
HRESULT CXpArray<TYPE>::Right(CXpArray& Subset, const INT_PTR nCount) const
{
	const INT_PTR nEnd = GetCount();

	return(Mid(Subset, nEnd - nCount, nCount));
}

template <typename TYPE> 
INT_PTR CXpArray<TYPE>::Find(const TYPE& Element, const INT_PTR nStart /*= 0*/) const
{
    INT_PTR nIndex = -1;	

    if( (0 <= nStart) && (nStart < m_nElementCount) )
    {	
        const TYPE* pCurrent = &m_pData[nStart];
        const TYPE* pEnd = &m_pData[m_nElementCount];

        while(pCurrent < pEnd)
        {
            if(Element != *pCurrent)
            {
                pCurrent ++;
            }
            else
            {
                nIndex = static_cast<INT_PTR>(pCurrent - m_pData);
                break;
            }
        }
    }

    return(nIndex);
}

template <typename TYPE> 
INT_PTR CXpArray<TYPE>::Find(const CXpArray& Subset, const INT_PTR nStart /*= 0*/) const
{
    INT_PTR nIndex = -1;
 
    if( (0 <= nStart) && (nStart < m_nElementCount) )
    {
        INT_PTR i = nStart;
        INT_PTR j = 0;
        INT_PTR k = 0;
        const INT_PTR nSetLength = GetCount();
        const INT_PTR nSubsetLength = Subset.GetCount();

        // This is just a brute force string search,
        // there are faster ways to do this.
        while(i < nSetLength)
        {
            k = i;
            j = 0;

            while( (k < nSetLength) &&
	               (j < nSubsetLength) && 
	               (m_pData[k] == Subset.m_pData[j]) )
            {
                j ++;
                k ++;
            }

            if( (nSubsetLength) && (j == nSubsetLength) )
            {
                nIndex = i;
                break;
            }
            else 
            {
                i ++;
            }
        }
    }

    return(nIndex);
}
						
template <typename TYPE> 
TYPE* CXpArray<TYPE>::GetData()
{
	return(m_pData);
}				 
						 
template <typename TYPE> 
const TYPE* CXpArray<TYPE>::GetData() const
{
	return(m_pData);
}				 


template <typename TYPE> 
TYPE& CXpArray<TYPE>::operator[](const INT_PTR nIndex)
{
	XpArrayVerifyBounds(nIndex);

	return(m_pData[nIndex]);  
}

template <typename TYPE> 
const TYPE& CXpArray<TYPE>::operator[](const INT_PTR nIndex) const
{
	XpArrayVerifyBounds(nIndex);

	return(m_pData[nIndex]);  
}

template <typename TYPE> 
inline bool CXpArray<TYPE>::operator==(const CXpArray& Array) const
{
    bool bAreEqual = true;

    if(m_nElementCount == Array.m_nElementCount)
    {
        for(INT_PTR i = 0; i < m_nElementCount; i ++)
        {
            // It is more common for objects to 
            // have the == operator rather than the != operator
            // so this uses the ==.
            if(!(m_pData[i] == Array.m_pData[i]))
            {
                bAreEqual = false;
                break;
            }
        }    
    }
    else
    {
        bAreEqual = false;
    }

    return(bAreEqual);
}

template <typename TYPE> 
HRESULT CXpArray<TYPE>::Copy(const CXpArray& Array)
{
    HRESULT hr = S_OK;

    if(this != &Array)
    {
        m_nGrowBy = Array.m_nGrowBy;
        hr = Allocate(Array.m_nElementCount);
        
        if(SUCCEEDED(hr))
        {
            XpArrayAssert(Array.m_nElementCount <= m_nElementCount);
            TYPE* pDestination = m_pData;
            const TYPE* pSource = Array.m_pData;
            const TYPE* pSourceEnd = &Array.m_pData[m_nElementCount];

            while(pSource < pSourceEnd)
            {
                *pDestination = *pSource;
                pDestination ++;
                pSource ++;
            }
        }
    }

    return(hr);  
}
//-----------------------------------------------------------------
//				CXpArray Protected Implementation
//-----------------------------------------------------------------
template <typename TYPE> 
HRESULT CXpArray<TYPE>::Allocate(const INT_PTR nCount, const bool bInitialize /*= true*/)
{
    HRESULT hr = HRESULT_FROM_WIN32(ERROR_NOT_ENOUGH_MEMORY);
    XpArrayAssert(0 <= nCount);

    // The objects in this array are only
    // constructed if they are actually needed.  When an 
    // object is allocated construction is delayed until
    // the last possible moment when it will first be used.

    if(m_nElementCount >= nCount)
    {
        // The space is already allocated and initialized.

        // If the count is less that what we previously had,
        // we have to destroy the objects.
        const INT_PTR nObjectsToDestroy = m_nElementCount - nCount;

        DestructElement(nCount, nObjectsToDestroy);

        m_nElementCount	= bInitialize ? nCount : 0;

        hr = S_OK;
    }
    else if(m_nAllocedCount >= nCount)
    {
        // The space has been previously allocated,
        // but the objects are not yet constructed.
        const INT_PTR nObjectsToInitialize = nCount -	m_nElementCount;

        if(bInitialize)
        {
            ConstructElement(m_nElementCount, nObjectsToInitialize);
        }

        m_nElementCount	= bInitialize ? nCount : 0;

        hr = S_OK;
    }
    else	     
    {
        const TYPE* pPreviousLocation = m_pData;
        const INT_PTR nNewCount = m_nGrowBy + nCount;
        XpArrayAssert(m_nElementCount < nNewCount);

        // Resize the memory.
        m_pData = (TYPE*)realloc(m_pData, nNewCount * sizeof(TYPE));

        if(m_pData)
        {
            // Call the contructors for the objects.
            const INT_PTR nObjectsToInitialize = nCount -	m_nElementCount;

            if(bInitialize)
            {
                ConstructElement(m_nElementCount, nObjectsToInitialize);
                m_nElementCount = nCount;
            }
            else
            {
                m_nElementCount = 0;
            }

            // Save the new allocated size. 
            // Note that objects after m_nElementCount are not
            // initialized.
            m_nAllocedCount = nNewCount;


            // If the starting address of the array changed,
            // all objects need to know.  Do a self assignment  
            // to communicate the change.
            if( (pPreviousLocation) &&
                (pPreviousLocation != m_pData) &&
                (m_pfnMoveCallback) )
            {
                (*(PFN_MoveCallback*)m_pfnMoveCallback)(m_pData, m_nElementCount);
            }

            hr = S_OK;
        }
    }

    return(hr);
}

template <typename TYPE> 
inline void CXpArray<TYPE>::ConstructElement(const INT_PTR nIndex, const INT_PTR nCount /*= 1*/) 
{	
    TYPE* pCurrent = &m_pData[nIndex];
    TYPE* pEnd = &m_pData[nIndex + nCount];

    while(pCurrent < pEnd)
    {
	    ::new( (void*)( pCurrent ) ) TYPE;
	    pCurrent ++;
    }
}

template <typename TYPE> 
inline void CXpArray<TYPE>::DestructElement(const INT_PTR nIndex, const INT_PTR nCount /*= 1*/) 
{							 
    TYPE* pCurrent = &m_pData[nIndex];
	TYPE* pEnd = &m_pData[nIndex + nCount];
	
	while(pCurrent < pEnd)
	{
		pCurrent->~TYPE();
		pCurrent ++; 
	}
}

#endif //__XPARRAY_INL__
