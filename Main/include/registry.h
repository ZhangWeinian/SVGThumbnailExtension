#pragma once

#include "common.h"

using REGKEY_SUBKEY = struct _REGKEY_SUBKEY
{
	HKEY	hKey;
	LPCWSTR lpszSubKey;
};

using REGKEY_SUBKEY_AND_VALUE = struct _REGKEY_SUBKEY_AND_VALUE
{
	HKEY	  hKey;
	LPCWSTR	  lpszSubKey;
	LPCWSTR	  lpszValue;
	DWORD	  dwType;
	DWORD_PTR dwData;
};

/**
 * @brief Creates or updates registry keys
 *
 * @param aKeys Registry keys and values to update
 * @param cKeys The number of keys in the aKeys array.
 */
STDAPI CreateRegistryKeys(REGKEY_SUBKEY_AND_VALUE* aKeys, ULONG cKeys);

/**
 * @brief Deletes the specified registry keys
 *
 * @param aKeys Registry keys to delete
 * @param cKeys The number of keys in the aKeys array.
 */
STDAPI DeleteRegistryKeys(REGKEY_SUBKEY* aKeys, ULONG cKeys);
