
// DataNetIODlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DataNetIO.h"
#include "DataNetIODlg.h"
#include "afxdialogex.h"
#include "TxtFileIO.h"
#include "StructDef.h"
#include "XMLConnection.h"
#include "MD5.h"
#include <iostream>
#include <fstream>
#include <string>
#include "DlgMainpage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace std;
using std::ifstream;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD){
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX){
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDataNetIODlg 对话框



CDataNetIODlg::CDataNetIODlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDataNetIODlg::IDD, pParent){
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDataNetIODlg::DoDataExchange(CDataExchange* pDX){
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDataNetIODlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OK, &CDataNetIODlg::OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCLE, &CDataNetIODlg::OnBnClickedButtonCancle)
END_MESSAGE_MAP()


// CDataNetIODlg 消息处理程序

BOOL CDataNetIODlg::OnInitDialog(){
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	ModifyStyleEx(WS_EX_TOOLWINDOW, WS_EX_APPWINDOW);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	xmlConn.initXML(LOC_LOGIN);
/*	CTxtFileIO cio(_T("E:\\bb\\as.txt"));
	CString textA(_T("BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, void* lpReserved){"));
	CString textB;
	int iT = 0;
	cio.writeALine(textA);
	cio.closeIO();
*/
/*	CString str(_T("E:\\bb\\as.txt"));
	
	CString mystr;
	int te = 0;
	while (cio.readNextLine(mystr))
	{
		te++;
	}

	ifstream ifs;
	ifs.open(str,ios::binary|ios::in|ios::out);
	string s;
	int temp = 0;
	int temple = 0x80808080;
	int pTemple[1] = {0x80808080};
	char cTemple[4]={0x80,0x80,0x80,0x80};
	char res[4] = {0xcc,0xcc,0xcc,0xcc};
	while (getline(ifs,s))
	{		
		int len = s.length();		
		char a[1024] ={0x00};
		s.copy(a,s.length()-1);
		int ia[1024] = {0x00};
		memcpy(ia,a,len-1);
		temp = ia[0] & temple;				
		temp++;
	}
	
*/

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDataNetIODlg::OnSysCommand(UINT nID, LPARAM lParam){
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDataNetIODlg::OnPaint(){
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDataNetIODlg::OnQueryDragIcon(){
	return static_cast<HCURSOR>(m_hIcon);
}

void CDataNetIODlg::OnBnClickedButtonOk(){
	// TODO: 在此添加控件通知处理程序代码

	//HRESULT hr;
	CString inputUsername,inputPassword;
	GetDlgItem(IDC_EDIT_LOGINUSER)->SetWindowTextW(_T("system"));
	GetDlgItem(IDC_EDIT_LOGINPSW)->SetWindowTextW(_T("system"));

	GetDlgItem(IDC_EDIT_LOGINUSER)->GetWindowTextW(inputUsername);
	GetDlgItem(IDC_EDIT_LOGINPSW)->GetWindowTextW(inputPassword);
	if (inputUsername.IsEmpty()){
		MessageBoxW(_T("请输入用户名"));
		GetDlgItem(IDC_EDIT_LOGINUSER)->SetWindowTextW(_T(""));
		GetDlgItem(IDC_EDIT_LOGINPSW)->SetWindowTextW(_T(""));
		return;
	}
	CMD5 md5;
	md5.update(inputPassword);
	CString psw = md5.toCString();
	//check username
	BOOL isUser = FALSE;BOOL isAdmin= FALSE;
	MSXML2::IXMLDOMNode* pRoot = NULL;
	MSXML2::IXMLDOMNodeList* pNodeUsers = NULL;
	CHK_HR_RETURN(xmlConn.getRoot(pRoot,_T("Persons")))
	CHK_HR_RETURN(pRoot->selectNodes(_T("User"),&pNodeUsers))
	MSXML2::IXMLDOMNode* pNodeUser = NULL;
	MSXML2::IXMLDOMNode* pNode = NULL;
	BSTR bst;
	while(pNodeUsers->nextNode(&pNodeUser) == S_OK){
		CHK_HR_RETURN(pNodeUser->selectSingleNode(_T("LoginName"),&pNode))
		CHK_HR_RETURN(pNode->get_text(&bst))
		if (bst == inputUsername){
			isUser = TRUE;
			CHK_HR_RETURN(pNode->get_nextSibling(&pNode))
			CHK_HR_RETURN(pNode->get_text(&bst))
			if (psw == bst){
				MSXML2::IXMLDOMElement *pElement = NULL;
				pNodeUser->QueryInterface(__uuidof(MSXML2::IXMLDOMElement), (void **)&pElement);
				VARIANT var;var.vt = VT_BSTR;VariantInit(&var);
				bst = _T("LoginType");
				CHK_HR_RETURN(pElement->getAttribute(bst,&var))
				bst = _T("Admin");
				if ((_bstr_t)(bst) == (_bstr_t)(var.bstrVal)){
					isAdmin = TRUE;
				}
				VariantClear(&var);
			} 
			else{
				MessageBoxW(_T("密码错误，请重新输入"));
				GetDlgItem(IDC_EDIT_LOGINPSW)->SetWindowTextW(_T(""));	
				return;
			}
		}
	}
	if (!isUser){
		MessageBoxW(_T("用户不存在,请重新输入"));
		GetDlgItem(IDC_EDIT_LOGINUSER)->SetWindowTextW(_T(""));
		GetDlgItem(IDC_EDIT_LOGINPSW)->SetWindowTextW(_T(""));
		return;
	}
	//SysFreeString(bst);bst = NULL;
	CDlgMainpage mdlg(isAdmin,this);
	mdlg.DoModal();
	CDialogEx::OnOK();
}

void CDataNetIODlg::OnBnClickedButtonCancle(){
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}
