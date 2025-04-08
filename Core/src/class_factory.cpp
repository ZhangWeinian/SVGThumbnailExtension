#pragma once

#include "class_factory.h"
#include "common.h"
#include "thumbnail_provider.h"

CClassFactory::CClassFactory(void)
{
	DllAddRef();
}

CClassFactory::~CClassFactory(void)
{
	DllRelease();
}

HRESULT CClassFactory::QueryInterfaceFactory(REFIID riid, void** ppvObject)
{
	CClassFactory* factory { new CClassFactory() };

	if (factory == nullptr)
	{
		return E_OUTOFMEMORY;
	}

	auto result { factory->QueryInterface(riid, ppvObject) };

	factory->Release();

	return result;
}

STDMETHODIMP CClassFactory::QueryInterface(REFIID riid, void** ppvObject)
{
	static const QITAB qit[] = {
		QITABENT(CClassFactory, IClassFactory),
		{ nullptr },
	};

	return QISearch(this, qit, riid, ppvObject);
}

STDMETHODIMP_(ULONG) CClassFactory::AddRef(void)
{
	LONG cRef { InterlockedIncrement(&m_cRef) };
	return (ULONG)cRef;
}

STDMETHODIMP_(ULONG) CClassFactory::Release(void)
{
	LONG cRef { InterlockedDecrement(&m_cRef) };

	if (0 == cRef)
	{
		delete this;
	}

	return (ULONG)cRef;
}

STDMETHODIMP CClassFactory::CreateInstance(IUnknown* punkOuter, REFIID riid, void** ppvObject)
{
	if (nullptr != punkOuter)
	{
		return CLASS_E_NOAGGREGATION;
	}

	return CThumbnailProvider::QueryInterfaceFactory(riid, ppvObject);
}

STDMETHODIMP CClassFactory::LockServer(BOOL fLock)
{
	Q_UNUSED(fLock);
	return E_NOTIMPL;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv)
{
	if (nullptr == ppv)
	{
		return E_INVALIDARG;
	}

	if (!IsEqualCLSID(CLSID_SampleThumbnailProvider, rclsid))
	{
		return CLASS_E_CLASSNOTAVAILABLE;
	}

	auto result { CClassFactory::QueryInterfaceFactory(riid, ppv) };

	return result;
}
