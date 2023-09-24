#include "Includes.h"

#ifndef UTLVECTOR
#define UTLVECTOR

template <class T>
FORCEINLINE void Destruct(T* pMemory)
{
	pMemory->~T();
}

template< class T, class I = int >
class CUtlMemory
{
public:
	// constructor, destructor
	CUtlMemory(int nGrowSize = 0, int nInitSize = 0);
	CUtlMemory(T* pMemory, int numElements);
	CUtlMemory(const T* pMemory, int numElements);
	~CUtlMemory();

	CUtlMemory(const CUtlMemory&) = delete;
	CUtlMemory& operator=(const CUtlMemory&) = delete;

	CUtlMemory(CUtlMemory&& moveFrom);
	CUtlMemory& operator=(CUtlMemory&& moveFrom);

	// Set the size by which the memory grows
	void Init(int nGrowSize = 0, int nInitSize = 0);

	class Iterator_t
	{
	public:
		Iterator_t(I i) : index(i) {}
		I index;

		bool operator==(const Iterator_t it) const { return index == it.index; }
		bool operator!=(const Iterator_t it) const { return index != it.index; }
	};

	Iterator_t First() const { return Iterator_t(IsIdxValid(0) ? 0 : InvalidIndex()); }
	Iterator_t Next(const Iterator_t& it) const { return Iterator_t(IsIdxValid(it.index + 1) ? it.index + 1 : InvalidIndex()); }
	I GetIndex(const Iterator_t& it) const { return it.index; }
	bool IsIdxAfter(I i, const Iterator_t& it) const { return i > it.index; }
	bool IsValidIterator(const Iterator_t& it) const { return IsIdxValid(it.index); }
	Iterator_t InvalidIterator() const { return Iterator_t(InvalidIndex()); }

	// Can we use this index?
	bool IsIdxValid(I i) const;

	// Specify the invalid ('null') index that we'll only return on failure
	static const I INVALID_INDEX = (I)-1; // For use with COMPILE_TIME_ASSERT
	static I InvalidIndex() { return INVALID_INDEX; }

	// element access
	T& operator[](I i);
	const T& operator[](I i) const;
	T& Element(I i);
	const T& Element(I i) const;

	// Gets the base address (can change when adding elements!)
	T* Base();
	const T* Base() const;

	int Count() const;

	// Attaches the buffer to external memory....
	void SetExternalBuffer(T* pMemory, int numElements);
	void SetExternalBuffer(const T* pMemory, int numElements);
	void AssumeMemory(T* pMemory, int nSize);
	T* Detach();
	void* DetachMemory();

	void Grow(int num = 1);

	// Makes sure we've got at least this much memory
	void EnsureCapacity(int num);

	// Memory deallocation
	void Purge();

	// Purge all but the given number of elements
	void Purge(int numElements);
protected:
	enum
	{
		EXTERNAL_BUFFER_MARKER = -1,
		EXTERNAL_CONST_BUFFER_MARKER = -2,
	};

	T* m_pMemory;
	int m_nAllocationCount;
	int m_nGrowSize;
};

template< class T, class I >
FORCEINLINE CUtlMemory<T, I>::CUtlMemory(int nGrowSize, int nInitAllocationCount) : m_pMemory(0),
m_nAllocationCount(nInitAllocationCount), m_nGrowSize(nGrowSize)
{
	if (m_nAllocationCount)
	{
		m_pMemory = (T*)malloc(m_nAllocationCount * sizeof(T));
	}
}

template< class T, class I >
FORCEINLINE CUtlMemory<T, I>::CUtlMemory(T* pMemory, int numElements) : m_pMemory(pMemory),
m_nAllocationCount(numElements)
{
	m_nGrowSize = EXTERNAL_BUFFER_MARKER;
}

template< class T, class I >
FORCEINLINE CUtlMemory<T, I>::CUtlMemory(const T* pMemory, int numElements) : m_pMemory((T*)pMemory),
m_nAllocationCount(numElements)
{
	m_nGrowSize = EXTERNAL_CONST_BUFFER_MARKER;
}

template< class T, class I >
FORCEINLINE CUtlMemory<T, I>::~CUtlMemory()
{
	Purge();
}

template< class T, class I >
FORCEINLINE CUtlMemory<T, I>::CUtlMemory(CUtlMemory&& moveFrom)
	: m_pMemory(moveFrom.m_pMemory)
	, m_nAllocationCount(moveFrom.m_nAllocationCount)
	, m_nGrowSize(moveFrom.m_nGrowSize)
{
	moveFrom.m_pMemory = nullptr;
	moveFrom.m_nAllocationCount = 0;
	moveFrom.m_nGrowSize = 0;
}

template< class T, class I >
FORCEINLINE CUtlMemory<T, I>& CUtlMemory<T, I>::operator=(CUtlMemory&& moveFrom)
{
	// Copy member variables to locals before purge to handle self-assignment
	T* pMemory = moveFrom.m_pMemory;
	int nAllocationCount = moveFrom.m_nAllocationCount;
	int nGrowSize = moveFrom.m_nGrowSize;

	moveFrom.m_pMemory = nullptr;
	moveFrom.m_nAllocationCount = 0;
	moveFrom.m_nGrowSize = 0;

	// If this is a self-assignment, Purge() is a no-op here
	Purge();

	m_pMemory = pMemory;
	m_nAllocationCount = nAllocationCount;
	m_nGrowSize = nGrowSize;

	return *this;
}

template< class T, class I >
FORCEINLINE void CUtlMemory<T, I>::Init(int nGrowSize /*= 0*/, int nInitSize /*= 0*/)
{
	Purge();

	m_nGrowSize = nGrowSize;
	m_nAllocationCount = nInitSize;

	if (m_nAllocationCount)
	{
		m_pMemory = (T*)malloc(m_nAllocationCount * sizeof(T));
	}
}

template< class T, class I >
FORCEINLINE T& CUtlMemory<T, I>::operator[](I i)
{
	return m_pMemory[i];
}

template< class T, class I >
FORCEINLINE const T& CUtlMemory<T, I>::operator[](I i) const
{
	return m_pMemory[i];
}

template< class T, class I >
FORCEINLINE T& CUtlMemory<T, I>::Element(I i)
{
	return m_pMemory[i];
}

template< class T, class I >
FORCEINLINE const T& CUtlMemory<T, I>::Element(I i) const
{
	return m_pMemory[i];
}

template< class T, class I >
inline T* CUtlMemory<T, I>::Base()
{
	return m_pMemory;
}

template< class T, class I >
FORCEINLINE const T* CUtlMemory<T, I>::Base() const
{
	return m_pMemory;
}

template< class T, class I >
FORCEINLINE int CUtlMemory<T, I>::Count() const
{
	return m_nAllocationCount;
}

template< class T, class I >
FORCEINLINE void CUtlMemory<T, I>::SetExternalBuffer(T* pMemory, int numElements)
{
	// Blow away any existing allocated memory
	Purge();

	m_pMemory = pMemory;
	m_nAllocationCount = numElements;

	// Indicate that we don't own the memory
	m_nGrowSize = EXTERNAL_BUFFER_MARKER;
}

template< class T, class I >
FORCEINLINE void CUtlMemory<T, I>::SetExternalBuffer(const T* pMemory, int numElements)
{
	// Blow away any existing allocated memory
	Purge();

	m_pMemory = const_cast<T*>(pMemory);
	m_nAllocationCount = numElements;

	// Indicate that we don't own the memory
	m_nGrowSize = EXTERNAL_CONST_BUFFER_MARKER;
}

template< class T, class I >
FORCEINLINE void CUtlMemory<T, I>::AssumeMemory(T* pMemory, int numElements)
{
	// Blow away any existing allocated memory
	Purge();

	// Simply take the pointer but don't mark us as external
	m_pMemory = pMemory;
	m_nAllocationCount = numElements;
}

template< class T, class I >
FORCEINLINE void* CUtlMemory<T, I>::DetachMemory()
{
	void* pMemory = m_pMemory;
	m_pMemory = 0;
	m_nAllocationCount = 0;
	return pMemory;
}

template< class T, class I >
FORCEINLINE T* CUtlMemory<T, I>::Detach()
{
	return (T*)DetachMemory();
}

//-----------------------------------------------------------------------------
// Is element index valid?
//-----------------------------------------------------------------------------
template< class T, class I >
FORCEINLINE bool CUtlMemory<T, I>::IsIdxValid(I i) const
{
	// GCC warns if I is an unsigned type and we do a ">= 0" against it (since the comparison is always 0).
	// We get the warning even if we cast inside the expression. It only goes away if we assign to another variable.
	long x = i;
	return (x >= 0) && (x < m_nAllocationCount);
}

FORCEINLINE int UtlMemory_CalcNewAllocationCount(int nAllocationCount, int nGrowSize, int nNewSize, int nBytesItem)
{
	if (nGrowSize)
	{
		nAllocationCount = ((1 + ((nNewSize - 1) / nGrowSize)) * nGrowSize);
	}
	else
	{
		if (!nAllocationCount)
		{
			// Compute an allocation which is at least as big as a cache line...
			nAllocationCount = (31 + nBytesItem) / nBytesItem;
			// If the requested amount is larger then compute an allocation which
			// is exactly the right size. Otherwise we can end up with wasted memory
			// when CUtlVector::EnsureCount(n) is called.
			if (nAllocationCount < nNewSize)
				nAllocationCount = nNewSize;
		}

		while (nAllocationCount < nNewSize)
		{
			nAllocationCount *= 2;
		}
	}

	return nAllocationCount;
}

template< class T, class I >
FORCEINLINE void CUtlMemory<T, I>::Grow(int num)
{
	int nAllocationRequested = m_nAllocationCount + num;

	int nNewAllocationCount = UtlMemory_CalcNewAllocationCount(m_nAllocationCount, m_nGrowSize, nAllocationRequested, sizeof(T));

	// if m_nAllocationRequested wraps index type I, recalculate
	if ((int)(I)nNewAllocationCount < nAllocationRequested)
	{
		if ((int)(I)nNewAllocationCount == 0 && (int)(I)(nNewAllocationCount - 1) >= nAllocationRequested)
		{
			--nNewAllocationCount; // deal w/ the common case of m_nAllocationCount == MAX_USHORT + 1
		}
		else
		{
			if ((int)(I)nAllocationRequested != nAllocationRequested)
			{
				return;
			}
			while ((int)(I)nNewAllocationCount < nAllocationRequested)
			{
				nNewAllocationCount = (nNewAllocationCount + nAllocationRequested) / 2;
			}
		}
	}

	m_nAllocationCount = nNewAllocationCount;

	if (m_pMemory)
	{
		m_pMemory = (T*)realloc(m_pMemory, m_nAllocationCount * sizeof(T));
	}
	else
	{
		m_pMemory = (T*)malloc(m_nAllocationCount * sizeof(T));
	}
}

template< class T, class I >
FORCEINLINE void CUtlMemory<T, I>::EnsureCapacity(int num)
{
	if (m_nAllocationCount >= num)
		return;

	m_nAllocationCount = num;

	if (m_pMemory)
	{
		m_pMemory = (T*)realloc(m_pMemory, m_nAllocationCount * sizeof(T));
	}
	else
	{
		m_pMemory = (T*)malloc(m_nAllocationCount * sizeof(T));
	}
}


//-----------------------------------------------------------------------------
// Memory deallocation
//-----------------------------------------------------------------------------
template< class T, class I >
FORCEINLINE void CUtlMemory<T, I>::Purge()
{
	if (m_pMemory)
	{
		free((void*)m_pMemory);
		m_pMemory = 0;
	}

	m_nAllocationCount = 0;
}

template< class T, class I >
FORCEINLINE void CUtlMemory<T, I>::Purge(int numElements)
{
	if (numElements > m_nAllocationCount)
		return;

	// If we have zero elements, simply do a purge:
	if (numElements == 0)
	{
		Purge();
		return;
	}

	// If the number of elements is the same as the allocation count, we are done.
	if (numElements == m_nAllocationCount)
		return;


	if (!m_pMemory)
		return;

	m_nAllocationCount = numElements;

	m_pMemory = (T*)realloc(m_pMemory, m_nAllocationCount * sizeof(T));
}

//CUtlMemory
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//CUtlVector

template< class T, class A = CUtlMemory<T> >
class CUtlVector
{
	typedef A CAllocator;
public:
	typedef T ElemType_t;
	typedef T* iterator;
	typedef const T* const_iterator;

	// Initialize with separately allocated buffer, setting the capacity and count.
	// The container will not be growable.
	CUtlVector(T* pMemory, int initialCapacity, int initialCount = 0);
	~CUtlVector();

	// Copy the array.
	CUtlVector<T, A>& operator=(const CUtlVector<T, A>& other);

	// element access
	T& operator[](int i);
	const T& operator[](int i) const;
	T& Element(int i);
	const T& Element(int i) const;
	T& Head();
	const T& Head() const;
	T& Tail();
	const T& Tail() const;

	// Gets the base address (can change when adding elements!)
	T* Base() { return m_Memory.Base(); }
	const T* Base() const { return m_Memory.Base(); }

	// Returns the number of elements in the vector
	int Count() const;
	/// are there no elements? For compatibility with lists.
	inline bool IsEmpty(void) const
	{
		return (Count() == 0);
	}

	int Find(const T& src) const;

	void RemoveAll();
	
	void Purge();

	// Purges the list and calls delete on each element in it.
	void PurgeAndDeleteElements();
protected:
	CAllocator m_Memory;
	int m_Size;
	T* m_pElements;
};

template< typename T, class A >
FORCEINLINE CUtlVector<T, A>::CUtlVector(T* pMemory, int allocationCount, int numElements) :
	m_Memory(pMemory, allocationCount), m_Size(numElements)
{
}

template< typename T, class A >
FORCEINLINE CUtlVector<T, A>::~CUtlVector()
{
	Purge();
}

template< typename T, class A >
FORCEINLINE CUtlVector<T, A>& CUtlVector<T, A>::operator=(const CUtlVector<T, A>& other)
{
	int nCount = other.Count();
	SetSize(nCount);
	for (int i = 0; i < nCount; i++)
	{
		(*this)[i] = other[i];
	}
	return *this;
}


//-----------------------------------------------------------------------------
// element access
//-----------------------------------------------------------------------------
template< typename T, class A >
FORCEINLINE T& CUtlVector<T, A>::operator[](int i)
{
	return m_Memory[i];
}

template< typename T, class A >
FORCEINLINE const T& CUtlVector<T, A>::operator[](int i) const
{
	return m_Memory[i];
}

template< typename T, class A >
FORCEINLINE T& CUtlVector<T, A>::Element(int i)
{
	return m_Memory[i];
}

template< typename T, class A >
FORCEINLINE const T& CUtlVector<T, A>::Element(int i) const
{
	return m_Memory[i];
}

template< typename T, class A >
FORCEINLINE T& CUtlVector<T, A>::Head()
{
	return m_Memory[0];
}

template< typename T, class A >
FORCEINLINE const T& CUtlVector<T, A>::Head() const
{
	return m_Memory[0];
}

template< typename T, class A >
FORCEINLINE T& CUtlVector<T, A>::Tail()
{
	return m_Memory[m_Size - 1];
}

template< typename T, class A >
FORCEINLINE const T& CUtlVector<T, A>::Tail() const
{
	return m_Memory[m_Size - 1];
}

template< typename T, class A >
FORCEINLINE int CUtlVector<T, A>::Count() const
{
	return m_Size;
}

template< typename T, class A >
FORCEINLINE int CUtlVector<T, A>::Find(const T& src) const
{
	for (int i = 0; i < Count(); ++i)
	{
		if (Element(i) == src)
			return i;
	}
	return -1;
}

template< typename T, class A >
FORCEINLINE void CUtlVector<T, A>::RemoveAll()
{
	for (int i = m_Size; --i >= 0; )
	{
		// Global scope to resolve conflict with Scaleform 4.0
		::Destruct(&Element(i));
	}

	m_Size = 0;
}

template< typename T, class A >
FORCEINLINE void CUtlVector<T, A>::Purge()
{
	RemoveAll();
	m_Memory.Purge();
}


template< typename T, class A >
FORCEINLINE void CUtlVector<T, A>::PurgeAndDeleteElements()
{
	for (int i = 0; i < m_Size; i++)
	{
		delete Element(i);
	}
	Purge();
}

#endif