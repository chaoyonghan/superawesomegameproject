#pragma once
template< typename T >
class SingletonHolder;

template< typename T >
class SingletonHolder
{
public:
	typedef T ObjectType;
	typedef T* ObjectTypePtr;
	typedef T& ObjectTypeRef;

	static ObjectTypeRef Instance()
	{
		if (m_pInstance == nullptr)
		{
			MakeInstance();
		}
		return *m_pInstance;
	}
		
private:

	static void MakeInstance()
	{
		if (!m_pInstance)
		{
			m_pInstance = new T();
		}
	}

	ObjectTypePtr m_pInstance;
};