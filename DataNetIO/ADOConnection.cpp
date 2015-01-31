#include "stdafx.h"
#include "ADOConnection.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CADOConnection::CADOConnection(){

}

CADOConnection::CADOConnection(DatabaseInfo info){
	m_Databaseinfo = info;
}

CADOConnection:: ~CADOConnection(){
	//ExitConnect();
}

HRESULT CADOConnection::OnInitAdo(){
	HRESULT hr = S_OK;
	::CoInitialize(NULL);
	try{
		m_pConnection.CreateInstance("ADODB.Connection");		
		_bstr_t strConnect;
		strConnect = _T("Provider=SQLOLEDB.1;Persist Security Info=False");
		strConnect = strConnect +_T(";Data Source=") + m_Databaseinfo.server 
								+_T(";Initial Catalog=") + m_Databaseinfo.database 
								+_T(";User ID=") + m_Databaseinfo.username 
								+_T(";Password=") + m_Databaseinfo.password;
		CHK_HR_RETURNERR(m_pConnection->Open(strConnect,"","",adModeUnknown))		
	}
	catch(_com_error e){
		CString str;
		str.Format(_T("%s"),e.ErrorMessage());
		AfxMessageBox(str);
		::CoUninitialize();
		hr = E_FAIL;
	}
	return hr;
}

_RecordsetPtr& CADOConnection::GetRecordset(CString bstrSQL,CommandTypeEnum commandType){	
	try{
		if(m_pConnection==NULL)
			OnInitAdo();
		m_pRecordset.CreateInstance(__uuidof(Recordset));
		switch (commandType){
		case adCmdText:
			m_pRecordset->Open((_bstr_t)bstrSQL,m_pConnection.GetInterfacePtr(),adOpenDynamic,adLockOptimistic,adCmdText);
			break;
		case adCmdStoredProc:
			m_pRecordset->Open((_bstr_t)bstrSQL,m_pConnection.GetInterfacePtr(),adOpenDynamic,adLockOptimistic,adCmdStoredProc);
			break;
		default:
			AfxMessageBox(_T("TypeError"));
			break;
		}
	}
	catch(_com_error e){
		CString str;
		str.Format(_T("%s"),e.ErrorMessage());
		AfxMessageBox(str);
	}
	return m_pRecordset;
}

_RecordsetPtr& CADOConnection::GetRecordset(){
	try{
		if(m_pConnection==NULL)
			OnInitAdo();
		m_pRecordset.CreateInstance(__uuidof(Recordset));
	}
	catch (_com_error e){
		CString str;
		str.Format(_T("%s"),e.ErrorMessage());
		AfxMessageBox(str);
	}
	return m_pRecordset;
}

_CommandPtr& CADOConnection::GetCommand(){
	try{
		if(m_pConnection==NULL)
			OnInitAdo();
		m_pCommand.CreateInstance( __uuidof(Command));
		m_pCommand->ActiveConnection = m_pConnection;
	}
	catch (_com_error e){
		CString str;
		str.Format(_T("%s"),e.ErrorMessage());
		AfxMessageBox(str);
	}
	return m_pCommand;
}

_ConnectionPtr& CADOConnection::GetConnection(){
	try{
		if(m_pConnection==NULL)
		OnInitAdo();
	}
	catch (_com_error e){
		CString str;
		str.Format(_T("%s"),e.ErrorMessage());
		AfxMessageBox(str);
	}
	return m_pConnection;
}

HRESULT CADOConnection::ExecuteSQL(CString strSQL){
	HRESULT hr;
	try{
		if(m_pConnection==NULL)
			OnInitAdo();
		m_pConnection->Execute((_bstr_t)strSQL,NULL,adCmdText);
		hr = S_OK;
	}
	catch(_com_error e){
		CString str;
		str.Format(_T("%s"),e.ErrorMessage());
		AfxMessageBox(str);
		hr = S_FALSE;
	}
	return hr;
}

HRESULT CADOConnection::ExecuteSQL(CString bstrSQL,_RecordsetPtr pRecordset){
	HRESULT hr;
	try{
		if(m_pConnection==NULL)
			OnInitAdo();
		m_pRecordset.CreateInstance(__uuidof(Recordset));
		m_pRecordset->Open((_bstr_t)bstrSQL,m_pConnection.GetInterfacePtr(),adOpenDynamic,adLockOptimistic,adCmdText);
		pRecordset = m_pRecordset;
		hr = S_OK;
	}	
	catch(_com_error e){
		CString str;
		str.Format(_T("%s"),e.ErrorMessage());
		AfxMessageBox(str);
		hr = E_FAIL;
	}
	return hr;
}

HRESULT CADOConnection::ExecuteProc(CString procName){
	HRESULT hr;
	try{
		if(m_pConnection==NULL)
			OnInitAdo();
		m_pCommand->CommandType = adCmdStoredProc;  
		m_pCommand->CommandText  = (_bstr_t)(procName); 
		for (size_t i = 0;i < m_Paras.size();i++)
		{
			m_pCommand->Parameters->Append(m_Paras[i]);
		}

		m_pCommand->Execute(NULL,NULL,adCmdStoredProc);
		hr = S_OK;
	}
	catch(_com_error e){
		CString str;
		str.Format(_T("%s"),e.ErrorMessage());
		AfxMessageBox(str);
		hr = E_FAIL;
	}
	return hr;
}

HRESULT CADOConnection::addParas(CString name, DataTypeEnum type, ParameterDirectionEnum direction, long tsize, const _variant_t &tvalue){
	HRESULT hr = S_OK;
	try{
		if ( NULL == m_pCommand ){
			if(m_pConnection==NULL)
				OnInitAdo();
			m_pCommand.CreateInstance( __uuidof(Command));
			m_pCommand->ActiveConnection = m_pConnection;
		}		
		para.CreateInstance(_uuidof(Parameter));
		para = m_pCommand->CreateParameter((_bstr_t)name,type,direction,tsize,tvalue);
		m_Paras.push_back(para);
	}
	catch(_com_error e){
		CString str;
		str.Format(_T("%s"),e.ErrorMessage());
		AfxMessageBox(str);
		hr = E_FAIL;
	}
	return hr;
}

void CADOConnection::ExitConnect(){
	m_Paras.clear();
	if(m_pRecordset!= NULL)
		m_pRecordset->Close();
	if (m_pCommand != NULL){
		m_pCommand->Cancel();
	}
	m_pConnection->Close();
	::CoUninitialize();
}