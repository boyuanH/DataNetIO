#pragma once
#include <fstream>
class CTxtFileIO
{
public:
	CTxtFileIO(const CString& file);
	~CTxtFileIO(void);
	//readAline
	BOOL readNextLine(CString& deCstr);
	BOOL writeALine(const CString& cstrDe);
	//read a log ÿ��log�������±�־��ʼ
	//********************************��32��* 
	BOOL readNextLog(CString& deCstr);
	//write a log
	BOOL writeALog(const CString& cstrDe);
	void closeIO();
private:
	CString m_file;
	CString m_filePath;
	CString m_fileName;
	std::fstream m_fs;

};

