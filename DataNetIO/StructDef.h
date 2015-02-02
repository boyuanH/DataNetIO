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
	CString departID;
	BOOL isDepartment;	
	std::vector<FileUint> fileEle;
};

struct userUnit{
	CString userName;
	BOOL isAdmin;
};

struct OutputInfo{
	HWND hWnd;
	DatabaseInfo info;
	CString folderPath;COleDateTime startDateTime;COleDateTime endDateTime;
	std::vector<OutUnit> outputEle;
};

struct Inputs{
	CString theFilePath;
	CString theFolderName;
	CString theFileName;
};

#define div_sign "********************************"
#define LOC_DATABASE	_T("E:\\confs\\database.xml")
#define LOC_LOGIN		_T("E:\\confs\\user.xml")
#define LOC_IOLOG		_T("E:\\confs\\IOLog.xml")
#define LOC_FILTERLOG	_T("E:\\confs\\Filter.csv")
#define LOC_FILTERNOR	_T("E:\\confs\\temp.csv")
#define LOC_FILEERR		_T("E:\\confs\\Error.txt")

//com组件清理与检测
#define CHK_HR_RETURN(stmt)        if (FAILED(stmt)) return; 
#define CHK_HR_RETURNFALSE(stmt)        if (FAILED(stmt)) return FALSE; 
#define CHK_HR_RETURNERR(stmt)        if (FAILED(stmt)) return E_FAIL; 
#define CHK_HR(stmt)        do { hr=(stmt); if (FAILED(hr)) goto CleanUp; } while(0)
#define SAFE_RELEASE(p)     do { if ((p)) { (p)->Release(); (p) = NULL; } } while(0)