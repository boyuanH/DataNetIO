#pragma once
#include "StructDef.h"
#include "XMLConnection.h"
#include <vector>
#include "MD5.h"
#include "afxwin.h"

// CAddStaff �Ի���

class CAddStaff : public CDialogEx
{
	DECLARE_DYNAMIC(CAddStaff)

public:
	CAddStaff(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAddStaff();

// �Ի�������
	enum { IDD = IDD_DIALOG_ADDSTAFF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
private:
	HRESULT getConfigFromXML();
	std::vector<userUnit> users;
public:
	virtual BOOL OnInitDialog();
	CButton m_isAdmin;
};
