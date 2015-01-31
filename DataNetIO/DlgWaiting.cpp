// DlgWaiting.cpp : 实现文件
//

#include "stdafx.h"
#include "DataNetIO.h"
#include "DlgWaiting.h"
#include "afxdialogex.h"
#include "ADOConnection.h"


// CDlgWaiting 对话框

IMPLEMENT_DYNAMIC(CDlgWaiting, CDialogEx)

CDlgWaiting::CDlgWaiting(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgWaiting::IDD, pParent)
{

}

CDlgWaiting::CDlgWaiting(OutputInfo outInfo)
	: CDialogEx(CDlgWaiting::IDD, FromHandle(outInfo.hWnd))
{
	m_outInfo = outInfo;
	CString strFileFolder,tempCstr;
	COleDateTime dateTimeCur = COleDateTime::GetCurrentTime();
	strFileFolder.Format(_T("\\%d"),dateTimeCur.GetYear());
	if (dateTimeCur.GetMonth()<10){
		tempCstr.Format(_T("0%d"),dateTimeCur.GetMonth());
	} 
	else{
		tempCstr.Format(_T("%d"),dateTimeCur.GetMonth());
	}
	strFileFolder = strFileFolder+tempCstr;	
	if (dateTimeCur.GetDay()<10){
		tempCstr.Format(_T("0%d"),dateTimeCur.GetDay());
	} 
	else{
		tempCstr.Format(_T("%d"),dateTimeCur.GetDay());
	}
	strFileFolder = strFileFolder+tempCstr;
	
	if (dateTimeCur.GetHour()<10){
		tempCstr.Format(_T("0%d"),dateTimeCur.GetHour());
	} 
	else{
		tempCstr.Format(_T("%d"),dateTimeCur.GetHour());
	}
	strFileFolder = strFileFolder+tempCstr;
	
	if (dateTimeCur.GetMinute()<10){
		tempCstr.Format(_T("0%d"),dateTimeCur.GetMinute());
	} 
	else{
		tempCstr.Format(_T("%d"),dateTimeCur.GetMinute());
	}
	strFileFolder = strFileFolder+tempCstr;
	//strFileFolder.Format(_T("\\%d%d%d%d%d"),dateTimeCur.GetYear(),dateTimeCur.GetMonth(),dateTimeCur.GetDay(),dateTimeCur.GetHour(),dateTimeCur.GetMinute());
	m_outInfo.folderPath = outInfo.folderPath+strFileFolder;
}

CDlgWaiting::~CDlgWaiting()
{
}

void CDlgWaiting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgWaiting, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CDlgWaiting::OnBnClickedCancel)
	ON_MESSAGE(WM_CLOSE,CDlgWaiting::OnClose)
END_MESSAGE_MAP()


// CDlgWaiting 消息处理程序


void CDlgWaiting::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	int response = MessageBoxW(_T("真的要中断"),_T("警告"),MB_OKCANCEL);
	if (IDOK == response)
	{
		MessageBoxW(_T("数据输出被迫中断异常"));		
		//结束线程

		//如果目录下有文件，删除已经建立的文件
		deleteDirectory(m_outInfo.folderPath);
		//退出
		CDialogEx::OnCancel();
	}
	if (IDCANCEL == response)
	{
		return ;
	}
}


BOOL CDlgWaiting::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_outInfo.hWnd = m_hWnd;
	m_pThread = AfxBeginThread(ThreadFunc,&m_outInfo);
	if (m_pThread ==NULL){
		MessageBoxW(_T("启动失败!"));
		return FALSE;
	}
	if(m_pThread){

	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgWaiting::deleteDirectory(CString strDir)
{
	if(strDir.IsEmpty()){ 
		RemoveDirectory(strDir); 
		return; 
	} 
	//首先删除文件及子文件夹 
	CFileFind   ff; 
	BOOL bFound = ff.FindFile(strDir+ _T("\\*"),0); 
	while(bFound) { 
		bFound = ff.FindNextFile(); 
		if(ff.GetFileName()== _T(".")||ff.GetFileName()== _T("..")) 
			continue; 
		//去掉文件(夹)只读等属性 
		SetFileAttributes(ff.GetFilePath(),FILE_ATTRIBUTE_NORMAL); 
		if(ff.IsDirectory()) {   
			//递归删除子文件夹 
			deleteDirectory(ff.GetFilePath()); 
			RemoveDirectory(ff.GetFilePath()); 
		} else{ 
			DeleteFile(ff.GetFilePath());   //删除文件 
		} 
	} 
	ff.Close(); 
	//然后删除该文件夹 
	RemoveDirectory(strDir); 
}

HRESULT CDlgWaiting::OnClose(WPARAM wParam,LPARAM lParam)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (wParam == 0){
		//失败
		deleteDirectory(m_outInfo.folderPath);
		//退出
		CDialogEx::OnCancel();		
	} else{
		CDialogEx::OnOK();
	}
	return E_FAIL;	
}

UINT ThreadFunc(LPVOID pParm)
{
	OutputInfo *pInfo = (OutputInfo *)pParm;
	HRESULT hr;
	//文件路径的唯一性检测
	CFileFind   ff; 
	BOOL bFound = ff.FindFile(pInfo->folderPath,0); 
	if (bFound)
	{
		//MessageBoxW(_T("该路径下有重复文件夹，请变更"));
		AfxMessageBox(_T("启动失败!"));
		//结束线程
		::SendMessage(pInfo->hWnd,WM_CLOSE,0,0);
		return E_FAIL;
	}
	else
	{
		//按照path建立新的文件夹
		if (!CreateDirectory(pInfo->folderPath,NULL)){
			return E_FAIL;
		}		
	}
	//连接数据库

	CString procName=_T("[dbo].[sql2csv]");
	//遍历操作 [caxastat].

	size_t sz = 0;
	std::vector<_ParameterPtr> paras;
	for (;sz < pInfo->outputEle.size();sz++)
	{

		if (pInfo->outputEle[sz].isDepartment)
		{
			pInfo->folderPath = pInfo->folderPath +_T("\\") + pInfo->outputEle[sz].departID;			
			if (!CreateDirectory(pInfo->folderPath,NULL)){
				return E_FAIL;
			}
			for (size_t szt = 0;szt < pInfo->outputEle[sz].fileEle.size();szt++){
				if (pInfo->outputEle[sz].fileEle[szt].isCheck){
					//创建参数集	
					//@filepath		varchar(256)	--FILEPATH取值为../../../数控车间/
					//,@outputType	int			--	1,2,3分别为DeviceOperationAnalysis,DeviceAlarm,DeviceFILETRANLOG
					//	,@department	varchar(50)	
					//	,@startDate		datetime		--2014-12-04 00:00:00
					//	,@endDate		datetime
					//	,@UID			varchar(128)
					//	,@UPSW			varchar(256)

					CADOConnection m_adoConnection(pInfo->info);					
					CHK_HR_RETURNERR(m_adoConnection.OnInitAdo())
					CHK_HR_RETURNERR(m_adoConnection.addParas(_T("filepath"),adVarWChar,adParamInput,pInfo->folderPath.GetLength()+1,(_variant_t)(pInfo->folderPath)))
					CHK_HR_RETURNERR(m_adoConnection.addParas(_T("outputType"),adInteger,adParamInput,sizeof(int),(_variant_t)(szt+1)))
					CHK_HR_RETURNERR(m_adoConnection.addParas(_T("department"),adVarWChar,adParamInput,pInfo->outputEle[sz].departName.GetLength()+1,(_variant_t)(pInfo->outputEle[sz].departName)))
					_variant_t  vtDate;
					vtDate.vt = VT_DATE;
					vtDate = pInfo->startDateTime.Format(_T("%Y-%m-%d %H:%M:%S"));
					CHK_HR_RETURNERR(m_adoConnection.addParas(_T("startDate"),adDBTimeStamp,adParamInput,135,(_variant_t)(vtDate)))
					vtDate = pInfo->endDateTime.Format(_T("%Y-%m-%d %H:%M:%S"));
					CHK_HR_RETURNERR(m_adoConnection.addParas(_T("endDate"),adDBTimeStamp,adParamInput,135,(_variant_t)(vtDate)))
					CHK_HR_RETURNERR(m_adoConnection.addParas(_T("UID"),adVarWChar,adParamInput,pInfo->info.username.GetLength()+1,(_variant_t)(pInfo->info.username)))
					CHK_HR_RETURNERR(m_adoConnection.addParas(_T("UPSW"),adVarWChar,adParamInput,pInfo->info.password.GetLength()+1,(_variant_t)(pInfo->info.password)))

					
					/*
					_ParameterPtr paraTemp;
					paraTemp.CreateInstance(_uuidof(Parameter));
					paraTemp = m_adoConnection.GetCommand()->CreateParameter("filepath",adVarWChar,adParamInput,pInfo->folderPath.GetLength()+1,(_variant_t)(pInfo->folderPath));
					paras.push_back(paraTemp);
					paraTemp.CreateInstance(_uuidof(Parameter));
					paraTemp = m_adoConnection.GetCommand()->CreateParameter("outputType",adInteger,adParamInput,sizeof(int),(_variant_t)(szt+1));
					paras.push_back(paraTemp);
					paraTemp.CreateInstance(_uuidof(Parameter));
					paraTemp = m_adoConnection.GetCommand()->CreateParameter("department",adVarWChar,adParamInput,pInfo->outputEle[sz].departName.GetLength()+1,(_variant_t)(pInfo->outputEle[sz].departName));
					paras.push_back(paraTemp);
					paraTemp.CreateInstance(_uuidof(Parameter));
					_variant_t  vtDate;
					vtDate.vt = VT_DATE;
					vtDate = pInfo->startDateTime.Format(_T("%Y-%m-%d %H:%M:%S"));
					paraTemp = m_adoConnection.GetCommand()->CreateParameter("startDate",adDBTimeStamp,adParamInput,135,vtDate);
					paras.push_back(paraTemp);
					paraTemp.CreateInstance(_uuidof(Parameter));
					vtDate = pInfo->endDateTime.Format(_T("%Y-%m-%d %H:%M:%S"));
					paraTemp = m_adoConnection.GetCommand()->CreateParameter("endDate",adDBTimeStamp,adParamInput,135,vtDate);
					paras.push_back(paraTemp);
					paraTemp.CreateInstance(_uuidof(Parameter));
					paraTemp = m_adoConnection.GetCommand()->CreateParameter("UID",adVarWChar,adParamInput,pInfo->info.username.GetLength()+1,(_variant_t)(pInfo->info.username));
					paras.push_back(paraTemp);
					paraTemp.CreateInstance(_uuidof(Parameter));
					paraTemp = m_adoConnection.GetCommand()->CreateParameter("UPSW",adVarWChar,adParamInput,pInfo->info.password.GetLength()+1,(_variant_t)(pInfo->info.password));
					paras.push_back(paraTemp);
					*/
					if (m_adoConnection.ExecuteProc(procName))
					{
						//LOG文件中添加记录
						hr = S_OK;
					}
					else
					{
						//ERRORLOG添加记录
					}
					paras.clear();
					m_adoConnection.ExitConnect();					
				}
			}
		}
	}

	::SendMessage(pInfo->hWnd,WM_CLOSE,1,0);
	return 0;
}