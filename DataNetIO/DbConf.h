#pragma once


// CDbConf �Ի���
#include "StructDef.h"
#include "XMLConnection.h"
class CDbConf : public CDialogEx
{
	DECLARE_DYNAMIC(CDbConf)

public:
	CDbConf(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDbConf();
	DatabaseInfo getDbConf(){return m_dbinfo;}
// �Ի�������
	enum { IDD = IDD_DIALOG_DATABASE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
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
