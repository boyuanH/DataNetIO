#pragma once


// CDlgWaiting 对话框
#include "StructDef.h"
class CDlgWaiting : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgWaiting)

public:
	CDlgWaiting(CWnd* pParent = NULL);   // 标准构造函数
	CDlgWaiting(OutputInfo outInfo);
	virtual ~CDlgWaiting();

// 对话框数据
	enum { IDD = IDD_DIALOG_WAITING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
private:
	OutputInfo m_outInfo;
private:
	void deleteDirectory(CString strDir);
	HRESULT dataOperation();
private:
	CWinThread* m_pThread;
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg HRESULT OnClose(WPARAM wParam,LPARAM lParam);
};

UINT ThreadFunc(LPVOID pParm);