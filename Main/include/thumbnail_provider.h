#pragma once

#include "common.h"

class CThumbnailProvider: public IThumbnailProvider,
						  IObjectWithSite,
						  IInitializeWithStream
{
private:
	LONG		 m_cRef { 1 };
	IUnknown*	 m_pSite { nullptr };
	bool		 loaded { false };
	QSvgRenderer renderer {};

	~CThumbnailProvider(void);

public:
	CThumbnailProvider(void);

	// Helper
	static HRESULT QueryInterfaceFactory(REFIID, void**);

	//  IUnknown methods
	STDMETHOD(QueryInterface)(REFIID, void**);
	STDMETHOD_(ULONG, AddRef)(void);
	STDMETHOD_(ULONG, Release)(void);

	//  IInitializeWithSteam methods
	STDMETHOD(Initialize)(IStream*, DWORD);

	//  IThumbnailProvider methods
	STDMETHOD(GetThumbnail)(UINT, HBITMAP*, WTS_ALPHATYPE*);

	//  IObjectWithSite methods
	STDMETHOD(GetSite)(REFIID, void**);
	STDMETHOD(SetSite)(IUnknown*);
};
