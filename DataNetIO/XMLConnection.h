#pragma once
using namespace MSXML2;
class CXMLConnection
{
public:
	CXMLConnection();
	CXMLConnection(CString filePath);
	~CXMLConnection(void);
public:
	HRESULT initXML();
	HRESULT initXML(CString filePath);
	HRESULT getRoot(MSXML2::IXMLDOMNode* pRoot,CString rootName);
	void exitXML();
private:
	CString m_filePath;
	MSXML2::IXMLDOMDocument *pXMLDoc;	
};

