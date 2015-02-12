// StaffMana.cpp : 实现文件
//

#include "stdafx.h"
#include "DataNetIO.h"
#include "StaffMana.h"
#include "afxdialogex.h"
#include "XMLConnection.h"
#include "StructDef.h"
#include "AddStaff.h"

// CStaffMana 对话框

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


// CStaffMana 消息处理程序


void CStaffMana::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//CDialogEx::OnOK();
	CAddStaff dlg;
	dlg.DoModal();
	getConfigFromXML();
	showList();
}


void CStaffMana::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}


BOOL CStaffMana::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  在此添加额外的初始化
	DWORD dwExStyle=LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_HEADERDRAGDROP|LVS_EX_ONECLICKACTIVATE|LVS_AUTOARRANGE;
	m_staffList.ModifyStyle(0,LVS_REPORT|LVS_SINGLESEL|LVS_SHOWSELALWAYS);
	m_staffList.SetExtendedStyle(dwExStyle);
	m_staffList.InsertColumn(0,_T("序号"),LVCFMT_CENTER,38,0);
	m_staffList.InsertColumn(1,_T("用户名"),LVCFMT_CENTER,140,0);
	m_staffList.InsertColumn(2,_T("用户身份"),LVCFMT_CENTER,140,0);

	getConfigFromXML();
	showList();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
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
			m_staffList.SetItemText(i,2,_T("管理员"));
		} else{
			m_staffList.SetItemText(i,2,_T("普通用户"));
		}	
	}
}

void CStaffMana::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码   删除一个用户	
	int i = -1;

	BOOL isSelected = FALSE;
	for(i=0; i<m_staffList.GetItemCount(); i++) { 
		if( m_staffList.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED ) { 
			isSelected = TRUE;
			break;
		} 
	} 
	if (!isSelected){
		AfxMessageBox(_T("没选择"));
		return;
	}
	TCHAR delName[1024]; 
	LVITEM lvi; 
	lvi.iItem = i; //行
	lvi.iSubItem = 1; //列
	lvi.mask = LVIF_TEXT; 
	lvi.pszText = delName; 
	lvi.cchTextMax = 1024; 
	m_staffList.GetItem(&lvi); 

	CString delStr;
	delStr.Format(_T("%s"),delName);

	if (IDNO == MessageBoxW(_T("确定要删除")+delStr+_T("？"),_T("删除用户"),MB_YESNO)){
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
