#include "stdafx.h"
#include "TxtFileIO.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
using std::ifstream;

CTxtFileIO::CTxtFileIO(const CString& file){
	m_file = file;
	CString tempStr = file;
	tempStr.MakeReverse();
	int npos = tempStr.Find(_T("\\"));
	m_fileName = tempStr.Left(npos);
	m_fileName.MakeReverse();
	m_filePath = tempStr.Right(tempStr.GetLength()-npos);
	m_filePath.MakeReverse();	
	m_fs.open(file,ios::in|ios::out|ios::app);
}

CTxtFileIO::~CTxtFileIO(void){
	if (m_fs.is_open()){
		m_fs.close();
	}
}

BOOL CTxtFileIO::readNextLine(CString& deCstr){
	string s;
	if (getline(m_fs,s)){
		deCstr = s.c_str();
		return TRUE;
	} else{
		return FALSE;
	}	
}

BOOL CTxtFileIO::readNextLog(CString& deCstr){
	string s;
	CString tems;
	BOOL flag = FALSE;
	while (getline(m_fs,s)){
		if ((s == ("********************************"))){
			if (FALSE == flag){
				flag = TRUE;
			} 
			else{
				return TRUE;
			}
		}else{
			CString tems(s.c_str());
			deCstr = deCstr + tems;
		}
	}
	return flag;
}

BOOL CTxtFileIO::writeALog(const CString& cstrDe){
	CString cstr(_T("********************************"));
	cstr = cstr +cstrDe;
	m_fs<<cstr;
	return TRUE;
}

BOOL CTxtFileIO::writeALine(const CString& cstrDe){
	
	CString m_cstr = cstrDe;
	char *chr=new char[m_cstr.GetLength()+1];
	memset(chr,0x00,m_cstr.GetLength()+1);
	char charC[1024] = {0x00}; 
	WideCharToMultiByte(CP_ACP,0,m_cstr.GetBuffer(),-1,chr,m_cstr.GetLength()+1,NULL,NULL);	
	WideCharToMultiByte(CP_ACP,0,m_cstr.GetBuffer(),-1,charC,m_cstr.GetLength()+1,NULL,NULL);	
	if (!cstrDe.IsEmpty()){
		m_fs<<endl;
	}
	//m_fs<<chr;
	m_fs<<charC;
	delete chr;
	return TRUE;
}

void CTxtFileIO::closeIO(){
	if (m_fs.is_open()){
		m_fs.close();
	}
}