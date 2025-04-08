#pragma once

#define INITGUID

#include "logging.h"
#include "registry.h"

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QString>

static_assert(QT_VERSION >= QT_VERSION_CHECK(6, 0, 0), "Qt version must be 6.0 or higher");

HINSTANCE	  g_hinstDll { nullptr };
LONG		  g_cRef { 0 };
QApplication* app {};

Q_LOGGING_CATEGORY(svgExtension, "SvgSee")

void Initialize(HMODULE hinstDll)
{
	WCHAR path[2048];
	ZeroMemory(path, sizeof(path));
	auto length { GetModuleFileNameW(hinstDll, path, 2048) };

	if ((GetLastError() != ERROR_SUCCESS) || (length <= 0))
	{
		debugLog << "Failed to retrieve module name";
	}

	auto modulePath { QString::fromWCharArray(path, length) };
	debugLog << "Module path is: " << modulePath;

	QFileInfo	dll(modulePath);
	QDir		libraryPath { dll.dir() };
	QStringList libraryPaths { (QStringList() << libraryPath.absolutePath()) };
	QApplication::setLibraryPaths(libraryPaths);

	int c { 0 };
	app = new QApplication(c, (char**)0, 0);
}

BOOL APIENTRY DllMain(HINSTANCE hinstDll, DWORD dwReason, LPVOID pvReserved)
{
	Q_UNUSED(pvReserved)

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		debugLog << "DLL_PROCESS_ATTACH";
		g_hinstDll = hinstDll;
		Initialize(hinstDll);
	}

	return TRUE;
}

STDAPI_(HINSTANCE) DllInstance(void)
{
	return g_hinstDll;
}

STDAPI DllCanUnloadNow(void)
{
	return g_cRef ? S_FALSE : S_OK;
}

STDAPI_(ULONG) DllAddRef(void)
{
	LONG cRef { InterlockedIncrement(&g_cRef) };

	return cRef;
}

STDAPI_(ULONG) DllRelease(void)
{
	LONG cRef { InterlockedDecrement(&g_cRef) };

	if (0 > cRef)
	{
		cRef = 0;
	}

	return cRef;
}

STDAPI DllRegisterServer(void)
{
	debugLog << "Enter: DLLRegisterServer";

	WCHAR szModule[MAX_PATH];
	ZeroMemory(szModule, sizeof(szModule));
	GetModuleFileName(g_hinstDll, szModule, ARRAYSIZE(szModule));

	// clang-format off
	REGKEY_SUBKEY_AND_VALUE keys[] = {
		{
			HKEY_CLASSES_ROOT,
			L"CLSID\\" szCLSID_SampleThumbnailProvider,
			nullptr,
			REG_SZ,
			(DWORD_PTR)L"SvgSee"
		},
		{
			HKEY_CLASSES_ROOT,
			L"CLSID\\" szCLSID_SampleThumbnailProvider L"\\InprocServer32",
			nullptr,
			REG_SZ,
			(DWORD_PTR)szModule
		},
		{
			HKEY_CLASSES_ROOT,
			L"CLSID\\" szCLSID_SampleThumbnailProvider L"\\InprocServer32",
			L"ThreadingModel",
			REG_SZ,
			(DWORD_PTR)L"Apartment"
		},
		{
			HKEY_CLASSES_ROOT,
			L".SVG\\shellex\\{E357FCCD-A995-4576-B01F-234630154E96}",
			nullptr,
			REG_SZ,
			(DWORD_PTR)szCLSID_SampleThumbnailProvider
		},
		{
			HKEY_CLASSES_ROOT,
			L".SVGZ\\shellex\\{E357FCCD-A995-4576-B01F-234630154E96}",
			nullptr,
			REG_SZ,
			(DWORD_PTR)szCLSID_SampleThumbnailProvider
		}
	};
	// clang-format on

	auto result { CreateRegistryKeys(keys, ARRAYSIZE(keys)) };

	debugLog << "Leaving: DLLRegisterServer";

	return result;
}

STDAPI DllUnregisterServer(void)
{
	debugLog << "Enter: DLLRegisterServer";

	// clang-format off
	REGKEY_SUBKEY keys[] = {
		{
			HKEY_CLASSES_ROOT,
			L"CLSID\\" szCLSID_SampleThumbnailProvider
		}
	};
	// clang-format on

	auto result { DeleteRegistryKeys(keys, ARRAYSIZE(keys)) };

	debugLog << "Leaving: DLLRegisterServer";

	return result;
}
