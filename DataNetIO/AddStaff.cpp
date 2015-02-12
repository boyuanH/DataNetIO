// AddStaff.cpp : 实现文件
//

#include "stdafx.h"
#include "DataNetIO.h"
#include "AddStaff.h"
#include "afxdialogex.h"


// CAddStaff 对话框

IMPLEMENT_DYNAMIC(CAddStaff, CDialogEx)

CAddStaff::CAddStaff(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAddStaff::IDD, pParent)
{

}

CAddStaff::~CAddStaff()
{
}

void CAddStaff::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_ADDISADMIN, m_isAdmin);
}


BEGIN_MESSAGE_MAP(CAddStaff, CDialogEx)
	ON_BN_CLICKED(IDOK, &CAddStaff::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CAddStaff::OnBnClickedCancel)
END_MESSAGE_MAP()


// CAddStaff 消息处理程序


void CAddStaff::OnBnClickedOk()
{
	HRESULT hr;
	// TODO: 在此添加控件通知处理程序代码
	CString uName,uPsw1,uPsw2; 
	GetDlgItem(IDC_EDIT_ADDUSER)->GetWindowTextW(uName);
	GetDlgItem(IDC_EDIT_ADDPSW1)->GetWindowTextW(uPsw1);
	GetDlgItem(IDC_EDIT_ADDPSW2)->GetWindowTextW(uPsw2);
	
	if ((uName.GetLength() == 0)){
		MessageBoxW(_T("填入信息不完整，请重新输入"));
		GetDlgItem(IDC_EDIT_ADDUSER)->SetWindowText(_T(""));
		GetDlgItem(IDC_EDIT_ADDPSW1)->SetWindowText(_T(""));
		GetDlgItem(IDC_EDIT_ADDPSW2)->SetWindowText(_T(""));
		m_isAdmin.SetCheck(0);
		return;
	}
	if (uPsw1 != uPsw2){
		MessageBoxW(_T("密码不一致，请重新输入"));
		GetDlgItem(IDC_EDIT_ADDPSW1)->SetWindowText(_T(""));
		GetDlgItem(IDC_EDIT_ADDPSW2)->SetWindowText(_T(""));
		m_isAdmin.SetCheck(0);
		return;
	}

	for (int i = 0;i<users.size();i++){
		if (users[i].userName == uName){
			MessageBoxW(_T("用户名重复，请重新输入"));
			GetDlgItem(IDC_EDIT_ADDUSER)->SetWindowText(_T(""));
			if (uPsw1 != uPsw2){
				GetDlgItem(IDC_EDIT_ADDPSW1)->SetWindowText(_T(""));
				GetDlgItem(IDC_EDIT_ADDPSW2)->SetWindowText(_T(""));
				m_isAdmin.SetCheck(0);
			}
			return;
		}
	}

	//write A Node
	CXMLConnection xmlConn;
	xmlConn.initXML(LOC_LOGIN);
	MSXML2::IXMLDOMNode* pRoot = NULL;
	CHK_HR_RETURN(xmlConn.getRoot(pRoot,_T("Persons")))
	
	MSXML2::IXMLDOMElementPtr pNode = NULL;
	xmlConn.getXMLDOC()->createElement((_bstr_t)("User"),&pNode);
	VARIANT var;VariantInit(&var);var.vt = VT_BSTR;
	if (m_isAdmin.GetCheck() == 1){
		BSTR bt = SysAllocString(_T("Admin"));
		var.bstrVal = bt;
		CHK_HR_RETURN(pNode->setAttribute((_bstr_t)("LoginType"),var))
		SysFreeString(bt);
	}else{
		BSTR bt = SysAllocString(_T("Normal"));
		var.bstrVal = bt;
		CHK_HR_RETURN(pNode->setAttribute((_bstr_t)("LoginType"),var))
		SysFreeString(bt);
	}
	CHK_HR_RETURN(pRoot->appendChild(pNode,NULL))
	MSXML2::IXMLDOMElementPtr pUNode = NULL;
	MSXML2::IXMLDOMElementPtr pPNode = NULL;
	CHK_HR_RETURN(xmlConn.getXMLDOC()->createElement((_bstr_t)("LoginName"),&pUNode))
	CHK_HR_RETURN(xmlConn.getXMLDOC()->createElement((_bstr_t)("Password"),&pPNode))
	CHK_HR_RETURN(pUNode->put_text((_bstr_t)(uName)))
	CMD5 md5;
	md5.update(uPsw1);
	CString Md5psw = md5.toCString();
	CHK_HR_RETURN(pPNode->put_text((_bstr_t)(Md5psw)))
	CHK_HR_RETURN(pNode->appendChild(pUNode,NULL))
	CHK_HR_RETURN(pNode->appendChild(pPNode,NULL))
	
	
	BSTR b = SysAllocString(LOC_LOGIN);
	var.bstrVal = b;
	
	//hr = xmlConn.getXMLDOC()->save(var);
	CHK_HR_RETURN(xmlConn.getXMLDOC()->save(var))
	xmlConn.exitXML();
	VariantClear(&var);
	SysFreeString(b);
	MessageBoxW(_T("添加完成"));
	CDialogEx::OnOK();
}


void CAddStaff::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}

HRESULT CAddStaff::getConfigFromXML(){
	CXMLConnection xmlConn;
	xmlConn.initXML(LOC_LOGIN);
	MSXML2::IXMLDOMNode* pRoot = NULL;
	MSXML2::IXMLDOMNodeList* pNodeUsers = NULL;
	CHK_HR_RETURNERR(xmlConn.getRoot(pRoot,_T("Persons")))
	CHK_HR_RETURNERR(pRoot->selectNodes(_T("User"),&pNodeUsers))
	MSXML2::IXMLDOMNode* pNodeUser = NULL;
	MSXML2::IXMLDOMNode* pNode = NULL;
	BSTR bstt=::SysAllocString(_T(""));;
	VARIANT var;var.vt = VT_BSTR;VariantInit(&var);
	while(pNodeUsers->nextNode(&pNodeUser) == S_OK){
		CHK_HR_RETURNERR(pNodeUser->selectSingleNode(_T("LoginName"),&pNode))
		CHK_HR_RETURNERR(pNode->get_text(&bstt))
		userUnit unit;
		unit.userName = (COLE2T)bstt;
		unit.isAdmin = FALSE;
		users.push_back(unit);
	}	

//	::SysFreeString(bstt);
	VariantClear(&var);
	return S_OK;  // return TRUE unless you set the focus to a control
}

BOOL CAddStaff::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	getConfigFromXML();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
