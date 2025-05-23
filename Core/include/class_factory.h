#pragma once

#include "common.h"

// 创建缩略图提供器的实例
class CClassFactory: public IClassFactory
{
private:
	LONG m_cRef { 1 };

	~CClassFactory(void);

public:
	CClassFactory(void);

	// Helper
	static HRESULT QueryInterfaceFactory(REFIID, void**);

	//  IUnknown methods
	STDMETHOD(QueryInterface)(REFIID, void**);
	STDMETHOD_(ULONG, AddRef)(void);
	STDMETHOD_(ULONG, Release)(void);

	//  IClassFactory methods
	STDMETHOD(CreateInstance)(IUnknown*, REFIID, void**);
	STDMETHOD(LockServer)(BOOL);
};
