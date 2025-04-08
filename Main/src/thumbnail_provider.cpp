#pragma once

#include "thumbnail_provider.h"
#include "common.h"

#include <assert.h>
#include <gdiplus.h>

#include <QtCore/QDateTime>

#ifndef NDEBUG
	#include <QtCore/QDir>
	#include <QtCore/QFile>
	#include <QtCore/QString>
#endif

#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>

// using namespace Gdiplus;

CThumbnailProvider::CThumbnailProvider(void)
{
	DllAddRef();
}

CThumbnailProvider::~CThumbnailProvider(void)
{
	if (m_pSite)
	{
		m_pSite->Release();
		m_pSite = nullptr;
	}

	DllRelease();
}

/*
 * ===============
 * IUnkown methods
 * ===============
 */
HRESULT CThumbnailProvider::QueryInterfaceFactory(REFIID riid, void** ppvObject)
{
	*ppvObject = nullptr;

	CThumbnailProvider* provider { new CThumbnailProvider() };

	if (provider == nullptr)
	{
		return E_OUTOFMEMORY;
	}

	auto result { provider->QueryInterface(riid, ppvObject) };

	provider->Release();

	return result;
}

STDMETHODIMP CThumbnailProvider::QueryInterface(REFIID riid, void** ppvObject)
{
	static const QITAB qit[] = {
		QITABENT(CThumbnailProvider, IInitializeWithStream),
		QITABENT(CThumbnailProvider, IThumbnailProvider),
		QITABENT(CThumbnailProvider, IObjectWithSite),
		{ nullptr },
	};

	return QISearch(this, qit, riid, ppvObject);
}

STDMETHODIMP_(ULONG) CThumbnailProvider::AddRef()
{
	LONG cRef { InterlockedIncrement(&m_cRef) };
	return (ULONG)cRef;
}

STDMETHODIMP_(ULONG) CThumbnailProvider::Release()
{
	LONG cRef { InterlockedDecrement(&m_cRef) };

	if (cRef == 0)
	{
		delete this;
	}

	return (ULONG)cRef;
}

/*
 * ===============
 * End IUnkown methods
 * ===============
 */

/*
 * ============================
 * IInitializeWithSteam methods
 * ============================
 */

STDMETHODIMP CThumbnailProvider::Initialize(IStream* pstm, DWORD grfMode)
{
	ULONG	len {};
	STATSTG stat {};

	Q_UNUSED(grfMode)

	if (loaded)
	{
		return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED);
	}

	if (pstm->Stat(&stat, STATFLAG_DEFAULT) != S_OK)
	{
		return S_FALSE;
	}

	char* data { new char[stat.cbSize.QuadPart] };

	if (pstm->Read(data, stat.cbSize.QuadPart, &len) != S_OK)
	{
		return S_FALSE;
	}

	QByteArray bytes { QByteArray(data, stat.cbSize.QuadPart) };

	loaded = renderer.load(bytes);

	return S_OK;
}

/*
 * ============================
 * End IInitializeWithSteam methods
 * ============================
 */

/*
 * ============================
 * IThumbnailProvider methods
 * ============================
 */

STDMETHODIMP CThumbnailProvider::GetThumbnail(UINT cx, HBITMAP* phbmp, WTS_ALPHATYPE* pdwAlpha)
{
	*phbmp	  = nullptr;
	*pdwAlpha = WTSAT_ARGB;

#ifdef NDEBUG
	if (!loaded)
	{
		return S_FALSE;
	}
#endif

	// Fit the render into a (cx * cx) square while maintaining the aspect ratio.
	QSize size { renderer.defaultSize() };
	size.scale(cx, cx, Qt::AspectRatioMode::KeepAspectRatio);

#ifndef NDEBUG
	QDir debugDir("C:\\dev");
	if (debugDir.exists())
	{
		QFile f("C:\\dev\\svg.log");
		f.open(QFile::Append);
		f.write(QString("Size: %1 \n").arg(cx).toUtf8());
		f.flush();
		f.close();
	}
#endif

	QImage* device { new QImage(size, QImage::Format_ARGB32) };
	device->fill(Qt::transparent);
	QPainter painter(device);

	painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);

	assert(device->paintingActive() && painter.isActive());
	if (loaded)
	{
		renderer.render(&painter);
	}
	else
	{
		QFont  font;
		QColor color_font { QColor(255, 0, 0) };
		int	   font_size { static_cast<int>(cx / 10) };

		font.setStyleHint(QFont::Monospace);
		font.setPixelSize(font_size);

		painter.setPen(color_font);
		painter.setFont(font);
		painter.drawText(font_size, (cx - font_size) / 2, "Invalid SVG file.");
	}
	painter.end();

	assert(!device->isNull());

#ifndef NDEBUG
	device->save(QString("C:\\dev\\%1.png").arg(QDateTime::currentMSecsSinceEpoch()), "PNG");
#endif

	*phbmp = ((QPixmap::fromImage(*device)).toImage().convertToFormat(QImage::Format_ARGB32)).toHBITMAP();

	assert(*phbmp != nullptr);

	delete device;

	if (*phbmp != nullptr)
	{
		return S_OK;
	}

	return S_FALSE;
}

/*
 * ============================
 * End IThumbnailProvider methods
 * ============================
 */

/*
 * ============================
 * IObjectWithSite methods
 * ============================
 */

STDMETHODIMP CThumbnailProvider::GetSite(REFIID riid, void** ppvSite)
{
	if (m_pSite)
	{
		return m_pSite->QueryInterface(riid, ppvSite);
	}

	return E_NOINTERFACE;
}

STDMETHODIMP CThumbnailProvider::SetSite(IUnknown* pUnkSite)
{
	if (m_pSite)
	{
		m_pSite->Release();
		m_pSite = nullptr;
	}

	m_pSite = pUnkSite;

	if (m_pSite)
	{
		m_pSite->AddRef();
	}

	return S_OK;
}

/*
 * ============================
 * End IObjectWithSite methods
 * ============================
 */
