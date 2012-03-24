#pragma once
template< typename T >
class SingletonHolder;

template< typename T >
class SingletonHolder
{
public:
	typedef typename T ObjectType;
	typedef typename T* ObjectTypePtr;
	typedef typename T& ObjectTypeRef;

	static ObjectTypePtr Instance()
	{
		if (!m_pInstance)
		{
			MakeInstance();
		}
		return m_pInstance;
	}
		
private:
	
	ObjectTypeRef operator = ( const ObjectTypeRef );

	static void MakeInstance()
	{
		if (!m_pInstance)
		{
			m_pInstance = new T();
		}
	}

	static ObjectTypePtr m_pInstance;
};

template <typename Type> Type* SingletonHolder<Type>::m_pInstance = 0;