#include "windows.h"
#include <iostream>
#include <stdio.h>
#include <direct.h>
#include <stdlib.h>
#include <ctype.h>


bool IsOnlyInstance(LPCTSTR gameTitle);
bool CheckStorage(const DWORDLONG diskSpaceNeeded);
bool CheckMemory(const DWORDLONG physicalRAMNeeded, const DWORDLONG virtualRAMNeeded);
DWORDLONG CheckPhysicalMemory();
DWORDLONG CheckVirtualMemory();
DWORD ReadCPUSpeed();
DWORD CPUArchitecture();

using namespace std;
int main()
{
	LPCTSTR gameTitle = "Project Bronze";
	DWORDLONG storageRequired = 300.0;
	DWORDLONG physRamNeed = 0;
	DWORDLONG virtRamNeed = 0;
	
	if (IsOnlyInstance(gameTitle))
	{
		cout << "instance found" << endl;
	}
	else
	{
		cout << "no instance found" << endl;
	}
	if (CheckStorage(storageRequired))
	{
		cout << "there is enough storage" << endl;
	}
	else
	{
		cout << "there is not enough storage" << endl;
	}

	cout <<"physical Memory: "<< CheckPhysicalMemory() << endl;
	cout <<"Virtual Memory: " << CheckVirtualMemory() << endl;
	cout << "Read CPU: " <<ReadCPUSpeed() << endl;
	cout << "CPU Architecture: ";
	cout << CPUArchitecture();

	return 0;
}

bool IsOnlyInstance(LPCTSTR gameTitle)
{
	HANDLE handle = CreateMutex(NULL, TRUE, gameTitle);
	if (GetLastError() != ERROR_SUCCESS)
	{
		HWND hWnd = FindWindow(gameTitle, NULL);
		if (hWnd)
		{
			// An instance of your game is already running.
			ShowWindow(hWnd, SW_SHOWNORMAL);
			SetFocus(hWnd);
			SetForegroundWindow(hWnd);	
			SetActiveWindow(hWnd);
			return false;
		}
	}
	return true;
}

bool CheckStorage(const DWORDLONG diskSpaceNeeded)
{
	// Check for enough free disk space on the current disk.
	int const drive = _getdrive();
	struct _diskfree_t diskfree;
	_getdiskfree(drive, &diskfree);
	unsigned __int64 const neededClusters = diskSpaceNeeded / (diskfree.sectors_per_cluster*diskfree.bytes_per_sector);
	if (diskfree.avail_clusters < neededClusters)
	{
		// if you get here you don’t have enough disk space!
		cout << "CheckStorage Failure : Not enough physical storage." << endl;
		return false;
	}
	return true;
}

bool CheckMemory(const DWORDLONG physicalRAMNeeded, const DWORDLONG virtualRAMNeeded)
{
	MEMORYSTATUSEX status;
	GlobalMemoryStatusEx(&status);
	if (status.ullTotalPhys < physicalRAMNeeded)
	{
		cout << "CheckMemory Failure : Not enough physical memory." << endl;
		return false;
	}
	// Check for enough free memory.
	if (status.ullAvailVirtual < virtualRAMNeeded)
	{
		cout << "CheckMemory Failure : Not enough virtual memory." << endl;
		return false;
	}
	char *buff = new char[virtualRAMNeeded];
	if (buff)
	{
		delete[] buff;
	}
	else
	{
		cout << "CheckMemory Failure : Not enough contiguous memory." << endl;
		return false;
	}
}

DWORD ReadCPUSpeed()
{
	DWORD BufSize = sizeof(DWORD);
	DWORD dwMHz = 0;
	DWORD type = REG_DWORD;
	HKEY hKey;
	// open the key where the proc speed is hidden:
	long lError = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey);
	if (lError == ERROR_SUCCESS)
	{
		// query the key:
		RegQueryValueEx(hKey, "~MHz", NULL, &type, (LPBYTE)&dwMHz, &BufSize);
	}
	else 
	{
		cout << "error";
	}
return dwMHz;
}
	
DWORDLONG CheckPhysicalMemory()
{
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status);
	return status.ullAvailPhys;
}

DWORDLONG CheckVirtualMemory()
{
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status);
	return status.ullAvailVirtual;
}
DWORD CPUArchitecture()
{
	DWORD type = REG_SZ;
	DWORD BufSize = sizeof(DWORD);
	char dwMHz[256] = "";
	
	HKEY hKey;
	// open the key where the proc speed is hidden:
	long lError = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey);
	if (lError == ERROR_SUCCESS)
	{
		// query the key:
		RegQueryValueEx(hKey, "ProcessorNameString", NULL, &type, (LPBYTE)&dwMHz, &BufSize);
		while (lError == ERROR_MORE_DATA)
			BufSize++;
		RegQueryValueEx(hKey, "ProcessorNameString", NULL, &type, (LPBYTE)&dwMHz, &BufSize);
	}
	else
	{
		cout << "error";
	}
	cout << dwMHz << endl;
	return 0;
}
