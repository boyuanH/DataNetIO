#pragma once
#include "afxwin.h"
#include "afxdtctl.h"
#include "StructDef.h"
#include <vector>

// CDlgMainpage �Ի���

class CDlgMainpage : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgMainpage)

public:
	CDlgMainpage(CWnd* pParent = NULL);   // ��׼���캯��
	CDlgMainpage(BOOL isAdmin,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgMainpage();

// �Ի�������
	enum { IDD = IDD_DIALOG_MAINPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
private:
	CMenu* pMenu;
	BOOL m_isAdmin;
	BOOL m_InOut;//0 is in 1 is out
	std::vector<OutUnit> outputEle;
	int listFilter();
	void showList();
	void showList(int selDepartment ,int selFile );//��������outputElementVector��ʾ�ڿؼ���
	HRESULT getConfigFromXML();
	void findAllFile(const CString& foldPath,std::vector<Inputs>& inp);
	BOOL inputFileCheck(const std::vector<Inputs>& inp);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnStart();
	afx_msg void OnDataInput();
	afx_msg void OnDataOutput();
	afx_msg void OnReLogin();
	afx_msg void OnExit();
	afx_msg void OnDBConfig();
	afx_msg void OnStaffMana();
	afx_msg void OnAbout();
	CCheckListBox m_listboxDepartment;
	CCheckListBox m_listboxOutputFileType;
	CDateTimeCtrl m_DateStart;
	CDateTimeCtrl m_dateEnd;
	DatabaseInfo m_dbinfo;
	afx_msg void OnLbnSelchangeListDepartment();
	afx_msg void OnLbnSelchangeListFilelist();
	
	afx_msg void OnBnClickedButtonConfirm();
};
