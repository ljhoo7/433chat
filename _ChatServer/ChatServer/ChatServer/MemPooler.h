#pragma once
#include <assert.h>

// Edited from C8MemPool class (Hitel, gma 7 3) - 1CoinClear -

template<class Type>
class MemPooler
{

public:
	// ������ 
	MemPooler(int nNumOfBlock) :m_nNumofBlock(nNumOfBlock),
		m_nAllocCount(0)
	{
		assert(nNumOfBlock>0);
		m_nListBlockSize = sizeof(Type);
		Create();
	}

	// �Ҹ���
	~MemPooler()
	{
		Destroy();
	}

	// �޸� �Ҵ� 
	Type* Alloc()
	{
		Type* pRet = NULL;

		EnterCriticalSection(&m_cs);
		////////////////////////////
		
		if (m_freeVector.size() > 0)
		{
			m_nAllocCount++;
			Type* ret = (Type *)m_freeVector.front();
			m_freeVector.pop_front();
			pRet = reinterpret_cast<Type*>(ret);
		}

		////////////////////////////
		LeaveCriticalSection(&m_cs);
		
		return pRet;
	}

	BOOL Free(Type* freeBlock)
	{
		BOOL bRet = FALSE;

		EnterCriticalSection(&m_cs);
		///////////////////////////
		if (m_nAllocCount>0)
		{
			m_freeVector.push_back(freeBlock);
			m_nAllocCount--;
			bRet = TRUE;
		}

		///////////////////////////
		LeaveCriticalSection(&m_cs);
		return bRet;
	}

	int GetCount()
	{
		return m_nAllocCount;
	}


protected:
	void Create()
	{

		for (int i = 0; i < m_nNumofBlock; i++){
			m_freeVector.push_back(new Type());
		}
		InitializeCriticalSectionAndSpinCount(&m_cs, 4000);

	}

	void Destroy()
	{
		DeleteCriticalSection(&m_cs);
	}

protected:
	int m_nNumofBlock;      // �޸� �Ҵ��� �� ��
	int m_nListBlockSize;   // �� �� ������ 
	int m_nAllocCount;      // �Ҵ�� �޸� �� ����

	std::list<Type*> m_freeVector;
	CRITICAL_SECTION m_cs;  // For Thread-Safe;
};