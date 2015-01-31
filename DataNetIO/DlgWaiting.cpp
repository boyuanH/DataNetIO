// DlgWaiting.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DataNetIO.h"
#include "DlgWaiting.h"
#include "afxdialogex.h"
#include "ADOConnection.h"


// CDlgWaiting �Ի���

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


// CDlgWaiting ��Ϣ�������


void CDlgWaiting::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int response = MessageBoxW(_T("���Ҫ�ж�"),_T("����"),MB_OKCANCEL);
	if (IDOK == response)
	{
		MessageBoxW(_T("������������ж��쳣"));		
		//�����߳�

		//���Ŀ¼�����ļ���ɾ���Ѿ��������ļ�
		deleteDirectory(m_outInfo.folderPath);
		//�˳�
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

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_outInfo.hWnd = m_hWnd;
	m_pThread = AfxBeginThread(ThreadFunc,&m_outInfo);
	if (m_pThread ==NULL){
		MessageBoxW(_T("����ʧ��!"));
		return FALSE;
	}
	if(m_pThread){

	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CDlgWaiting::deleteDirectory(CString strDir)
{
	if(strDir.IsEmpty()){ 
		RemoveDirectory(strDir); 
		return; 
	} 
	//����ɾ���ļ������ļ��� 
	CFileFind   ff; 
	BOOL bFound = ff.FindFile(strDir+ _T("\\*"),0); 
	while(bFound) { 
		bFound = ff.FindNextFile(); 
		if(ff.GetFileName()== _T(".")||ff.GetFileName()== _T("..")) 
			continue; 
		//ȥ���ļ�(��)ֻ�������� 
		SetFileAttributes(ff.GetFilePath(),FILE_ATTRIBUTE_NORMAL); 
		if(ff.IsDirectory()) {   
			//�ݹ�ɾ�����ļ��� 
			deleteDirectory(ff.GetFilePath()); 
			RemoveDirectory(ff.GetFilePath()); 
		} else{ 
			DeleteFile(ff.GetFilePath());   //ɾ���ļ� 
		} 
	} 
	ff.Close(); 
	//Ȼ��ɾ�����ļ��� 
	RemoveDirectory(strDir); 
}

HRESULT CDlgWaiting::OnClose(WPARAM wParam,LPARAM lParam)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (wParam == 0){
		//ʧ��
		deleteDirectory(m_outInfo.folderPath);
		//�˳�
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
	//�ļ�·����Ψһ�Լ��
	CFileFind   ff; 
	BOOL bFound = ff.FindFile(pInfo->folderPath,0); 
	if (bFound)
	{
		//MessageBoxW(_T("��·�������ظ��ļ��У�����"));
		AfxMessageBox(_T("����ʧ��!"));
		//�����߳�
		::SendMessage(pInfo->hWnd,WM_CLOSE,0,0);
		return E_FAIL;
	}
	else
	{
		//����path�����µ��ļ���
		if (!CreateDirectory(pInfo->folderPath,NULL)){
			return E_FAIL;
		}		
	}
	//�������ݿ�

	CString procName=_T("[dbo].[sql2csv]");
	//�������� [caxastat].

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
					//����������	
					//@filepath		varchar(256)	--FILEPATHȡֵΪ../../../���س���/
					//,@outputType	int			--	1,2,3�ֱ�ΪDeviceOperationAnalysis,DeviceAlarm,DeviceFILETRANLOG
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
						//LOG�ļ�����Ӽ�¼
						hr = S_OK;
					}
					else
					{
						//ERRORLOG��Ӽ�¼
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