#pragma once
#include <vector>
enum LoginType{
	NORMAL,
	ADMIN
};

struct DatabaseInfo{
	CString server;
	CString database;
	CString username;
	CString password;
};

struct FileUint{
	CString fileName;
	BOOL isCheck;
};

struct OutUnit{
	CString departName;
	BOOL isDepartment;	
	std::vector<FileUint> fileEle;
};

struct OutputInfo{
	HWND hWnd;
	DatabaseInfo info;
	CString folderPath;COleDateTime startDateTime;COleDateTime endDateTime;
	std::vector<OutUnit> outputEle;
};

#define div_sign "********************************"
#define LOC_DATABASE	_T("conf\\database.xml")
#define LOC_LOGIN		_T("conf\\itest.xml")
#define LOC_IOLOG		_T("conf\\IOLog.xml")
#define LOC_FILTERLOG	_T("conf\\Filter.xml")

//com组件清理与检测
#define CHK_HR_RETURN(stmt)        if (FAILED(stmt)) return; 
#define CHK_HR_RETURNERR(stmt)        if (FAILED(stmt)) return E_FAIL; 
#define CHK_HR(stmt)        do { hr=(stmt); if (FAILED(hr)) goto CleanUp; } while(0)
#define SAFE_RELEASE(p)     do { if ((p)) { (p)->Release(); (p) = NULL; } } while(0)