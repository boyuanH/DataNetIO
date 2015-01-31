#include "stdafx.h"
#include "XMLConnection.h"
#include "StructDef.h"

using namespace MSXML2;
CXMLConnection::CXMLConnection(){
	
}

CXMLConnection::CXMLConnection(CString filePath){
	m_filePath = filePath;
}

CXMLConnection::~CXMLConnection(void){

}

HRESULT CXMLConnection::initXML(CString filePath){
	m_filePath = filePath;
	return initXML();
}

HRESULT CXMLConnection::initXML(){
	HRESULT hr;
	if(m_filePath.IsEmpty())
		return E_INVALIDARG;
	try{
		CoInitialize(NULL);
		hr = CoCreateInstance(__uuidof(DOMDocument40),
								NULL,
								CLSCTX_INPROC_SERVER,
								__uuidof(MSXML2::IXMLDOMDocument),
								(void**)&pXMLDoc);
		CHK_HR_RETURNERR(hr);				
		VARIANT_BOOL status;
		VARIANT var;
		VariantInit(&var);
		V_BSTR(&var) = m_filePath.AllocSysString();
		V_VT(&var) = VT_BSTR;
		pXMLDoc->load(var, &status);
		if (VARIANT_TRUE != status){			  
			if (pXMLDoc) pXMLDoc->Release();
		}
		VariantClear(&var);		
		hr = S_OK;
	}
	catch(_com_error e){
		CString str;
		str.Format(_T("%s"),e.ErrorMessage());
		AfxMessageBox(str);
		hr = E_FAIL;
	}
	return hr;
}

HRESULT CXMLConnection::getRoot(MSXML2::IXMLDOMNode* &pRoot,CString rootName){
	MSXML2::IXMLDOMNode* pnode = NULL;
	if (NULL == pXMLDoc)
		initXML();
	BSTR bstr;
	bstr = rootName.AllocSysString();
	HRESULT hr = pXMLDoc->selectSingleNode(bstr,&pnode);
	SysFreeString(bstr);
	pRoot = pnode;
//	pnode->Release();
	if (pRoot!=NULL){
		return S_OK;
	}else{
		return E_FAIL;
	}	
}

void CXMLConnection::exitXML(){
	if (pXMLDoc != NULL)
		pXMLDoc->Release();
	CoUninitialize();	
}

MSXML2::IXMLDOMDocument* CXMLConnection::getXMLDOC(){
	return pXMLDoc;
}