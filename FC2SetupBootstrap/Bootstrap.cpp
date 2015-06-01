#include <Windows.h>
#include <Shlwapi.h>
#include <io.h>
#include <errno.h>
#include <tchar.h>

namespace
{
	TCHAR REGPATH[] = TEXT("SOFTWARE\\Ubisoft\\Eagle Dynamics\\Lock On");
	TCHAR REGKEY[] = TEXT("Path");

	enum LaunchFlag
	{
		lfCreateReg,
		lfCreateFile,
		lfSize,
	};
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	TCHAR path[MAX_PATH];
	TCHAR exe[MAX_PATH];
	DWORD type, size;
	BOOL flag[lfSize] = {0};

	memset(path, 0, sizeof(path));
	size = MAX_PATH;
	SHGetValue(HKEY_LOCAL_MACHINE, REGPATH, REGKEY, &type, (void*)path, &size);

	if (lstrlen(path) == 0)
	{
		flag[lfCreateReg] = TRUE;

		GetTempPath(MAX_PATH, path);

		HKEY key;
		RegCreateKey(HKEY_LOCAL_MACHINE, REGPATH, &key);
		RegSetValueEx(key, REGKEY, 0, REG_SZ, (const BYTE*)path, lstrlen(path) * sizeof(TCHAR));
		RegCloseKey(key);
	}

	wsprintf(exe, TEXT("%s\\LockOn.exe"), path);
	if (_taccess(exe, 0) == -1)
	{
		flag[lfCreateFile] = TRUE;

		TCHAR me[MAX_PATH];
		GetModuleFileName(NULL, me, MAX_PATH);

		CopyFile(me, exe, FALSE);
	}

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	GetStartupInfo(&si);
	TCHAR cmd[MAX_PATH];
	lstrcpy(cmd, TEXT("Setup.exe"));
	if (!CreateProcess(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		MessageBox(NULL, TEXT("Can't start setup.exe!"), NULL, MB_ICONERROR);
	}
	else
	{
		WaitForSingleObject(pi.hThread, INFINITE);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}

	if (flag[lfCreateFile])
	{
		DeleteFile(exe);
	}

	if (flag[lfCreateReg])
	{
		SHDeleteValue(HKEY_LOCAL_MACHINE, REGPATH, REGKEY);
	}

	return 0;
}
