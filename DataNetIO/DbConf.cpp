// DbConf.cpp : 实现文件
//

#include "stdafx.h"
#include "DataNetIO.h"
#include "DbConf.h"
#include "afxdialogex.h"
#include "XMLConnection.h"
#include "ADOConnection.h"


// CDbConf 对话框

IMPLEMENT_DYNAMIC(CDbConf, CDialogEx)

CDbConf::CDbConf(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDbConf::IDD, pParent)
{

}

CDbConf::~CDbConf()
{
}

void CDbConf::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDbConf, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_CONTEST, &CDbConf::OnBnClickedButtonContest)
	ON_BN_CLICKED(IDOK, &CDbConf::OnBnClickedOk)
END_MESSAGE_MAP()


// CDbConf 消息处理程序


BOOL CDbConf::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//连接XML文档，取得文档中的信息，如果信息不合规，则提示需要数据库文件中不正确，需要重新配置，可以进行一次当前数据联通测试
	xmlConn.initXML(LOC_DATABASE);
	BOOL isRight = FALSE;
	MSXML2::IXMLDOMNode* pRoot = NULL;	
	MSXML2::IXMLDOMNodeList* pinfos = NULL;
	CHK_HR_RETURNFALSE(xmlConn.getRoot(pRoot,_T("DataInfo")))
	CHK_HR_RETURNFALSE(pRoot->selectNodes(_T("Info"),&pinfos))
	MSXML2::IXMLDOMNode* pinfo = NULL;
	MSXML2::IXMLDOMNode* pinfoNode = NULL;
	BSTR bst;
	while(S_OK ==( pinfos->nextNode(&pinfo))){
		CHK_HR_RETURNFALSE(pinfo->selectSingleNode(_T("Server"),&pinfoNode))
		CHK_HR_RETURNFALSE(pinfoNode->get_text(&bst))
		m_dbinfo.server = bst;		
		CHK_HR_RETURNFALSE(pinfo->selectSingleNode(_T("Catalog"),&pinfoNode))
		CHK_HR_RETURNFALSE(pinfoNode->get_text(&bst))
		m_dbinfo.database = bst;
		CHK_HR_RETURNFALSE(pinfo->selectSingleNode(_T("UserID"),&pinfoNode))
		CHK_HR_RETURNFALSE(pinfoNode->get_text(&bst))
		m_dbinfo.username = bst;
		CHK_HR_RETURNFALSE(pinfo->selectSingleNode(_T("Password"),&pinfoNode))
		CHK_HR_RETURNFALSE(pinfoNode->get_text(&bst))
		m_dbinfo.password = bst;
	}	
	GetDlgItem(IDC_EDIT_DBSERVER)->SetWindowTextW(m_dbinfo.server);
	GetDlgItem(IDC_EDIT_DBCATALOG)->SetWindowTextW(m_dbinfo.database);
	GetDlgItem(IDC_EDIT_DBUSER)->SetWindowTextW(m_dbinfo.username);
	GetDlgItem(IDC_EDIT_DBPSW)->SetWindowTextW(m_dbinfo.password);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CDbConf::OnBnClickedButtonContest()
{
	// TODO: 在此添加控件通知处理程序代码
	DatabaseInfo dbinfo;
	GetDlgItem(IDC_EDIT_DBSERVER)->GetWindowTextW(dbinfo.server);
	GetDlgItem(IDC_EDIT_DBCATALOG)->GetWindowTextW(dbinfo.database);
	GetDlgItem(IDC_EDIT_DBUSER)->GetWindowTextW(dbinfo.username);
	GetDlgItem(IDC_EDIT_DBPSW)->GetWindowTextW(dbinfo.password);
	if (!ipAddressCheck(dbinfo.server)){
		MessageBoxW(_T("IP地址不合理"));
	}
	if ((!dbinfo.database.GetLength())||(!dbinfo.username.GetLength())){
		MessageBoxW(_T("重新输入信息"));
	}
	CADOConnection m_adoConnection(dbinfo);
	if (SUCCEEDED(m_adoConnection.OnInitAdo())){
		MessageBoxW(_T("连接成功"));
		m_adoConnection.ExitConnect();	
	}else{
		MessageBoxW(_T("连接不成功"));
	}	
}


void CDbConf::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//将对话框的信息取出，复制到databaseXML中
	DatabaseInfo dbinfo;
	GetDlgItem(IDC_EDIT_DBSERVER)->GetWindowTextW(dbinfo.server);
	GetDlgItem(IDC_EDIT_DBCATALOG)->GetWindowTextW(dbinfo.database);
	GetDlgItem(IDC_EDIT_DBUSER)->GetWindowTextW(dbinfo.username);
	GetDlgItem(IDC_EDIT_DBPSW)->GetWindowTextW(dbinfo.password);
	if (!ipAddressCheck(dbinfo.server)){
		MessageBoxW(_T("IP地址不合理"));
	}
	if ((!dbinfo.database.GetLength())||(!dbinfo.username.GetLength())){
		MessageBoxW(_T("重新输入信息"));
	}

	MSXML2::IXMLDOMNode* pRoot = NULL;	
	MSXML2::IXMLDOMNodeList* pinfos = NULL;
	CHK_HR_RETURN(xmlConn.getRoot(pRoot,_T("DataInfo")))
	CHK_HR_RETURN(pRoot->selectNodes(_T("Info"),&pinfos))
	MSXML2::IXMLDOMNode* pinfo = NULL;
	MSXML2::IXMLDOMNode* pinfoNode = NULL;
	BSTR bst =::SysAllocString(_T(""));
	while(S_OK ==( pinfos->nextNode(&pinfo))){
		CHK_HR_RETURN(pinfo->selectSingleNode(_T("Server"),&pinfoNode))
		bst = dbinfo.server.AllocSysString();
		CHK_HR_RETURN(pinfoNode->put_text(bst))			
		CHK_HR_RETURN(pinfo->selectSingleNode(_T("Catalog"),&pinfoNode))
		bst = dbinfo.database.AllocSysString();
		CHK_HR_RETURN(pinfoNode->put_text(bst))		
		CHK_HR_RETURN(pinfo->selectSingleNode(_T("UserID"),&pinfoNode))
		bst = dbinfo.username.AllocSysString();
		CHK_HR_RETURN(pinfoNode->put_text(bst))		
		CHK_HR_RETURN(pinfo->selectSingleNode(_T("Password"),&pinfoNode))
		bst = dbinfo.password.AllocSysString();
		CHK_HR_RETURN(pinfoNode->put_text(bst))	
	}	
	VARIANT varPath;
	VariantInit(&varPath);
	V_VT(&varPath) = VT_BSTR;
	V_BSTR(&varPath) = SysAllocString(LOC_DATABASE);
	CHK_HR_RETURN(xmlConn.getXMLDOC()->save(varPath));
	VariantClear(&varPath);
	SysFreeString(bst);
	CDialogEx::OnOK();
}

BOOL CDbConf::ipAddressCheck(const CString& ipCstr){
	BOOL isOK = TRUE;
	if (ipCstr.GetLength()>15){
		return FALSE;
	}
	int ipadd[4] = {-1,-1,-1,-1};	int j = 0;
	CString strDot(_T("."));CString tempStr;
	for (int i = 0;i < ipCstr.GetLength();i++){		
		if (ipCstr[i] == strDot){
			if (!tempStr.GetLength()){
				return FALSE;
			}
			ipadd[j] = _ttoi(tempStr);
			if (ipadd[j]>255 || ipadd[j]<0){
				return FALSE;
			}
			j++;
			tempStr.Empty();
		}
		else{
			if (ipCstr[i]>57 || ipCstr[i]<48){
				return FALSE;
			}
			tempStr.AppendChar(ipCstr[i]);
		}
	}

	ipadd[j] = _ttoi(tempStr);
	if (ipadd[j]>255 || ipadd[j]<0){
		return FALSE;
	}

	if (3 != j){
		return FALSE;
	}
	if (0 == ipadd[0]){
		return FALSE;
	}
	for (int i = 0;i<4;i++){
		if (ipadd[i]>255 || ipadd[i]<0){
			return FALSE;
		}
	}
	return isOK;
}