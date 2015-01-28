
#if !defined(AFX_ADOCONN_H__INCLUDED_)
#define AFX_ADOCONN_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma once
#include <vector>
#include "StructDef.h"
class CADOConnection
{
public:
	CADOConnection(void);
	CADOConnection(DatabaseInfo info);
	virtual ~CADOConnection(void);
	HRESULT OnInitAdo();
	_RecordsetPtr& GetRecordset(CString bstrSQL,CommandTypeEnum commandType);
	_RecordsetPtr& GetRecordset();
	_CommandPtr& GetCommand();
	_ConnectionPtr& GetConnection();
	HRESULT ExecuteSQL(CString strSQL);
	HRESULT ExecuteSQL(CString bstrSQL,_RecordsetPtr pRecordset);
	HRESULT ExecuteProc(CString procName);
	HRESULT addParas(CString name, DataTypeEnum type, ParameterDirectionEnum direction, long tsize, const _variant_t &tvalue);
	void ExitConnect();

private:
	_RecordsetPtr m_pRecordset;//��¼��ָ��
	_ConnectionPtr m_pConnection;//���ݿ�����ָ��
	_CommandPtr	m_pCommand;//����ָ��
	std::vector<_ParameterPtr> m_Paras;//�洢���̵Ĳ�����

private:
	DatabaseInfo m_Databaseinfo;
};

#endif