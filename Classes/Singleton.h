#ifndef __Singleton_h__
#define __Singleton_h__

#include "sys/types.h"

template<typename Derived> class CSingleton
{
private:
	static Derived* ms_pSingleton;

protected:
	CSingleton(void);
	virtual ~CSingleton(void);

public:
	inline static Derived& getInstance(void);
	inline static Derived* getInstancePtr(void);
};

template<typename Derived> CSingleton<Derived>::CSingleton(void)
{
	size_t nOffset = (size_t)(Derived*)0x1 - (size_t)(CSingleton<Derived>*)0x1;
	ms_pSingleton = (Derived*)((size_t)this + nOffset);
}

template<typename Derived> CSingleton<Derived>::~CSingleton(void)
{
	ms_pSingleton = 0;
}

template<typename Derived> inline Derived& CSingleton<Derived>::getInstance(void)
{
	return (*ms_pSingleton);
}

template<typename Derived> inline Derived* CSingleton<Derived>::getInstancePtr(void)
{
	return ms_pSingleton;
}

template<typename Derived> Derived* CSingleton<Derived>::ms_pSingleton = 0;
#endif // __Singleton_h__
