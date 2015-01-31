#pragma once


// CDbConf 对话框
#include "StructDef.h"
#include "XMLConnection.h"
class CDbConf : public CDialogEx
{
	DECLARE_DYNAMIC(CDbConf)

public:
	CDbConf(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDbConf();
	DatabaseInfo getDbConf(){return m_dbinfo;}
// 对话框数据
	enum { IDD = IDD_DIALOG_DATABASE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
private:
	DatabaseInfo m_dbinfo;
	CXMLConnection xmlConn;
	DECLARE_MESSAGE_MAP()
	BOOL ipAddressCheck(const CString& ipCstr);
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonContest();
	afx_msg void OnBnClickedOk();
};
