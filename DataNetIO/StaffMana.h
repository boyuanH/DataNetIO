#pragma once
#include "afxcmn.h"
#include "StructDef.h"
#include <vector>

// CStaffMana 对话框

class CStaffMana : public CDialogEx
{
	DECLARE_DYNAMIC(CStaffMana)

public:
	CStaffMana(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CStaffMana();

// 对话框数据
	enum { IDD = IDD_DIALOG_STAFFMANA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
