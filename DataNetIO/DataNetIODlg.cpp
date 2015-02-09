
// DataNetIODlg.cpp : ʵ���ļ�
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
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CDataNetIODlg �Ի���



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


// CDataNetIODlg ��Ϣ�������

BOOL CDataNetIODlg::OnInitDialog(){
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

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

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CDataNetIODlg::OnPaint(){
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CDataNetIODlg::OnQueryDragIcon(){
	return static_cast<HCURSOR>(m_hIcon);
}

void CDataNetIODlg::OnBnClickedButtonOk(){
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	//HRESULT hr;
	CString inputUsername,inputPassword;
	GetDlgItem(IDC_EDIT_LOGINUSER)->SetWindowTextW(_T("system"));
	GetDlgItem(IDC_EDIT_LOGINPSW)->SetWindowTextW(_T("system"));

	GetDlgItem(IDC_EDIT_LOGINUSER)->GetWindowTextW(inputUsername);
	GetDlgItem(IDC_EDIT_LOGINPSW)->GetWindowTextW(inputPassword);
	if (inputUsername.IsEmpty()){
		MessageBoxW(_T("�������û���"));
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
				MessageBoxW(_T("�����������������"));
				GetDlgItem(IDC_EDIT_LOGINPSW)->SetWindowTextW(_T(""));	
				return;
			}
		}
	}
	if (!isUser){
		MessageBoxW(_T("�û�������,����������"));
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnCancel();
}
