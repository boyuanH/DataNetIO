// AddStaff.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DataNetIO.h"
#include "AddStaff.h"
#include "afxdialogex.h"


// CAddStaff �Ի���

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


// CAddStaff ��Ϣ�������


void CAddStaff::OnBnClickedOk()
{
	HRESULT hr;
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString uName,uPsw1,uPsw2; 
	GetDlgItem(IDC_EDIT_ADDUSER)->GetWindowTextW(uName);
	GetDlgItem(IDC_EDIT_ADDPSW1)->GetWindowTextW(uPsw1);
	GetDlgItem(IDC_EDIT_ADDPSW2)->GetWindowTextW(uPsw2);
	
	if ((uName.GetLength() == 0)){
		MessageBoxW(_T("������Ϣ������������������"));
		GetDlgItem(IDC_EDIT_ADDUSER)->SetWindowText(_T(""));
		GetDlgItem(IDC_EDIT_ADDPSW1)->SetWindowText(_T(""));
		GetDlgItem(IDC_EDIT_ADDPSW2)->SetWindowText(_T(""));
		m_isAdmin.SetCheck(0);
		return;
	}
	if (uPsw1 != uPsw2){
		MessageBoxW(_T("���벻һ�£�����������"));
		GetDlgItem(IDC_EDIT_ADDPSW1)->SetWindowText(_T(""));
		GetDlgItem(IDC_EDIT_ADDPSW2)->SetWindowText(_T(""));
		m_isAdmin.SetCheck(0);
		return;
	}

	for (int i = 0;i<users.size();i++){
		if (users[i].userName == uName){
			MessageBoxW(_T("�û����ظ�������������"));
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
	MessageBoxW(_T("������"));
	CDialogEx::OnOK();
}


void CAddStaff::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	getConfigFromXML();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
