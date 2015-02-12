// StaffMana.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DataNetIO.h"
#include "StaffMana.h"
#include "afxdialogex.h"
#include "XMLConnection.h"
#include "StructDef.h"
#include "AddStaff.h"

// CStaffMana �Ի���

IMPLEMENT_DYNAMIC(CStaffMana, CDialogEx)

CStaffMana::CStaffMana(CWnd* pParent /*=NULL*/)
	: CDialogEx(CStaffMana::IDD, pParent)
{

}

CStaffMana::~CStaffMana()
{
}

void CStaffMana::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_staffList);
}


BEGIN_MESSAGE_MAP(CStaffMana, CDialogEx)
	ON_BN_CLICKED(IDOK, &CStaffMana::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CStaffMana::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, &CStaffMana::OnBnClickedButton1)
END_MESSAGE_MAP()


// CStaffMana ��Ϣ�������


void CStaffMana::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//CDialogEx::OnOK();
	CAddStaff dlg;
	dlg.DoModal();
	getConfigFromXML();
	showList();
}


void CStaffMana::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnCancel();
}


BOOL CStaffMana::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	DWORD dwExStyle=LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_HEADERDRAGDROP|LVS_EX_ONECLICKACTIVATE|LVS_AUTOARRANGE;
	m_staffList.ModifyStyle(0,LVS_REPORT|LVS_SINGLESEL|LVS_SHOWSELALWAYS);
	m_staffList.SetExtendedStyle(dwExStyle);
	m_staffList.InsertColumn(0,_T("���"),LVCFMT_CENTER,38,0);
	m_staffList.InsertColumn(1,_T("�û���"),LVCFMT_CENTER,140,0);
	m_staffList.InsertColumn(2,_T("�û����"),LVCFMT_CENTER,140,0);

	getConfigFromXML();
	showList();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

HRESULT CStaffMana::getConfigFromXML(){
	users.clear();
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
		MSXML2::IXMLDOMElement *pElement = NULL;
		pNodeUser->QueryInterface(__uuidof(MSXML2::IXMLDOMElement), (void **)&pElement);		
		bstt = _T("LoginType");
		CHK_HR_RETURNERR(pElement->getAttribute(bstt,&var))
		bstt = _T("Admin");
		if ((_bstr_t)(bstt) == (_bstr_t)(var.bstrVal)){
			unit.isAdmin = TRUE;
		}else{
			unit.isAdmin = FALSE;
		}
		users.push_back(unit);
	}		
	
	//::SysFreeString(bstt);
	VariantClear(&var);
	xmlConn.exitXML();
	return S_OK;  // return TRUE unless you set the focus to a control
}

void CStaffMana::showList(){
	m_staffList.DeleteAllItems();
	if (users.size() == 0){
		return ;
	}
	
	for (int i = 0; i<users.size();i++){
		CString str;
		str.Format(_T("%d"),i);
		m_staffList.InsertItem(i,str);
		m_staffList.SetItemText(i,1,users[i].userName);
		if (users[i].isAdmin){
			m_staffList.SetItemText(i,2,_T("����Ա"));
		} else{
			m_staffList.SetItemText(i,2,_T("��ͨ�û�"));
		}	
	}
}

void CStaffMana::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������   ɾ��һ���û�	
	int i = -1;

	BOOL isSelected = FALSE;
	for(i=0; i<m_staffList.GetItemCount(); i++) { 
		if( m_staffList.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED ) { 
			isSelected = TRUE;
			break;
		} 
	} 
	if (!isSelected){
		AfxMessageBox(_T("ûѡ��"));
		return;
	}
	TCHAR delName[1024]; 
	LVITEM lvi; 
	lvi.iItem = i; //��
	lvi.iSubItem = 1; //��
	lvi.mask = LVIF_TEXT; 
	lvi.pszText = delName; 
	lvi.cchTextMax = 1024; 
	m_staffList.GetItem(&lvi); 

	CString delStr;
	delStr.Format(_T("%s"),delName);

	if (IDNO == MessageBoxW(_T("ȷ��Ҫɾ��")+delStr+_T("��"),_T("ɾ���û�"),MB_YESNO)){
		return;
	}

	CXMLConnection xmlConn;
	xmlConn.initXML(LOC_LOGIN);
	MSXML2::IXMLDOMNode* pRoot = NULL;
	MSXML2::IXMLDOMNodeList* pNodeUsers = NULL;
	CHK_HR_RETURN(xmlConn.getRoot(pRoot,_T("Persons")))
	CHK_HR_RETURN(pRoot->selectNodes(_T("User"),&pNodeUsers))
	MSXML2::IXMLDOMNode* pNodeUser = NULL;
	MSXML2::IXMLDOMNode* pNode = NULL;
	HRESULT hr;
	BSTR bstt=::SysAllocString(_T(""));;
	while(pNodeUsers->nextNode(&pNodeUser) == S_OK){
		CHK_HR_RETURN(pNodeUser->selectSingleNode(_T("LoginName"),&pNode))
		CHK_HR_RETURN(pNode->get_text(&bstt))
		if ((_bstr_t)bstt == (_bstr_t)delStr){
			hr = pRoot->removeChild(pNodeUser,NULL);
		}
	}
	VARIANT var;VariantInit(&var);var.vt = VT_BSTR;
	BSTR b = SysAllocString(LOC_LOGIN);
	var.bstrVal = b;	
	CHK_HR_RETURN(xmlConn.getXMLDOC()->save(var))
	VariantClear(&var);	
	getConfigFromXML();
	showList();
}
