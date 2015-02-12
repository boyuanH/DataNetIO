#pragma once
#include "afxcmn.h"
#include "StructDef.h"
#include <vector>

// CStaffMana �Ի���

class CStaffMana : public CDialogEx
{
	DECLARE_DYNAMIC(CStaffMana)

public:
	CStaffMana(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CStaffMana();

// �Ի�������
	enum { IDD = IDD_DIALOG_STAFFMANA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CListCtrl m_staffList;
	virtual BOOL OnInitDialog();
private:
	HRESULT getConfigFromXML();
	void showList();
	std::vector<userUnit> users;
public:
	afx_msg void OnBnClickedButton1();
};
