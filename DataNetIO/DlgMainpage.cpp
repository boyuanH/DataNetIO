// DlgMainpage.cpp : 实现文件
//

#include "stdafx.h"
#include "DataNetIO.h"
#include "DlgMainpage.h"
#include "afxdialogex.h"
#include "TxtFileIO.h"
#include "DbConf.h"
#include "StructDef.h"
#include "ADOConnection.h"
#include "DlgWaiting.h"
#include <iostream>
#include <fstream>
#include <string>
#include "time.h"
#include "StaffMana.h"
#include "DataNetIODlg.h"

using namespace std;
using std::ifstream;
// CDlgMainpage 对话框

IMPLEMENT_DYNAMIC(CDlgMainpage, CDialogEx)

CDlgMainpage::CDlgMainpage(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgMainpage::IDD, pParent)
{

}

CDlgMainpage::CDlgMainpage(BOOL isAdmin,CWnd* pParent)
	: CDialogEx(CDlgMainpage::IDD, pParent)
{
	m_isAdmin = isAdmin;
	AfxGetApp()->m_pMainWnd = this;
	pParent->ShowWindow(SW_HIDE);
}

CDlgMainpage::~CDlgMainpage()
{
}

void CDlgMainpage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DEPARTMENT, m_listboxDepartment);
	DDX_Control(pDX, IDC_LIST_FILELIST, m_listboxOutputFileType);
	DDX_Control(pDX, IDC_DATETIMEPICKER_START, m_DateStart);
	DDX_Control(pDX, IDC_DATETIMEPICKER_END, m_dateEnd);
}

BOOL CDlgMainpage::OnInitDialog(){
	CDialogEx::OnInitDialog();
	// TODO:  在此添加额外的初始化
	pMenu  = new CMenu();
	if (m_isAdmin == TRUE){
		pMenu->LoadMenuW(IDR_MENU_ADMIN);
	}else{
		pMenu->LoadMenuW(IDR_MENU_NORM);
	}
	SetMenu(pMenu);
	ModifyStyleEx(0,WS_EX_APPWINDOW);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgMainpage::OnStart(){
	//连接数据库，如果成功则将所需的信息处理后显示在界面中，用以保证数据库的连接正常
	outputEle.clear();
	if (S_OK != getConfigFromXML()){
		MessageBoxW(_T("Failed on get Config File From XML"));
		return ;
	}	
	//重新从数据库中获取数据
	//从数据库中获取可选时间范围
	CADOConnection m_adoConnection(m_dbinfo);
	if (S_OK != m_adoConnection.OnInitAdo()){
		MessageBoxW(_T("连接数据库失败"));
		return ;
	}
	_RecordsetPtr m_pRecordset;
	//从数据库初始化数据
	CString	empSQL=_T("Select * from [caxastat].[dbo].[SYSCLASSFICATION]");
	m_pRecordset=m_adoConnection.GetRecordset(empSQL,adCmdText);
	//读取数据库来填充车间，选择类型
	while (!m_pRecordset->TheAdoEOF){
		OutUnit ele;
		_variant_t temp = (m_pRecordset->GetCollect("classname"));
		_variant_t tempID = (m_pRecordset->GetCollect("objectid"));
		if (temp.vt != VT_NULL){
			//添加车间
			ele.departName = temp.bstrVal;
			ele.departID.Format(_T("%d"),tempID.intVal);
			ele.isDepartment= TRUE;
			//添加文件
			FileUint fu;
			fu.fileName =_T("操作文件");fu.isCheck = TRUE; ele.fileEle.push_back(fu);
			fu.fileName =_T("报警文件");fu.isCheck = TRUE; ele.fileEle.push_back(fu);
			fu.fileName =_T("传输文件");fu.isCheck = TRUE; ele.fileEle.push_back(fu);		
			outputEle.push_back(ele);
		}
		m_pRecordset->MoveNext();
	}
	if (outputEle.empty()){
		return ;
	}
	COleDateTime* pStartDate = &(COleDateTime::GetCurrentTime());
	COleDateTime* pEndDate = &(COleDateTime::GetCurrentTime());
	VARIANT varDate;
	VariantInit(&varDate);
	V_VT(&varDate) = VT_DATE;
	empSQL=_T("SELECT Top 1 * FROM [caxastat].[dbo].[DeviceOperationAnalysis] order by starttime ");
	m_pRecordset=m_adoConnection.GetRecordset(empSQL,adCmdText);
	while(!m_pRecordset->TheAdoEOF){
		varDate = (m_pRecordset->GetCollect("starttime"));
		if (varDate.date != NULL){
			pStartDate = new COleDateTime(varDate);
		}
		m_pRecordset->MoveNext();
	}

	empSQL=_T("SELECT Top 1 * FROM [caxastat].[dbo].[DeviceOperationAnalysis] order by starttime DESC");
	m_pRecordset=m_adoConnection.GetRecordset(empSQL,adCmdText);
	while(!m_pRecordset->TheAdoEOF){
		varDate = (m_pRecordset->GetCollect("starttime"));
		if (varDate.date != NULL){
			pEndDate = new COleDateTime(varDate);
		}
		m_pRecordset->MoveNext();
	}
	m_adoConnection.ExitConnect();
	m_DateStart.SetRange(pStartDate,pEndDate);
	m_dateEnd.SetRange(pStartDate,pEndDate);
	delete pStartDate;
	delete pEndDate;
	showList();
	m_listboxDepartment.SetCurSel(0);
	m_listboxOutputFileType.SetCurSel(0);
}

void CDlgMainpage::OnDataInput(){
	//csv2sql
	if (S_OK != getConfigFromXML()){
		MessageBoxW(_T("Failed on get Config File From XML"));
		return ;
	}	
	//0 获取路径
	TCHAR	szFolderPath[MAX_PATH] = {0};
	CString strFolderPath;	
	BROWSEINFO      sInfo;  
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));  
	sInfo.pidlRoot   = 0;  
	sInfo.lpszTitle   = _T("请选择一个文件夹：");  
	sInfo.ulFlags   = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX;  
	sInfo.lpfn     = NULL;  
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);   
	if (lpidlBrowse != NULL){  
		// 取得文件夹名  
		if (::SHGetPathFromIDList(lpidlBrowse,szFolderPath)){  
			strFolderPath = szFolderPath;  
		}  
	}  
	if(lpidlBrowse != NULL){  
		::CoTaskMemFree(lpidlBrowse);  
	}  
	//如果取得的文件名不是日期，错误
	if (strFolderPath.IsEmpty()){
		return;
	}
	//1对文件夹进行遍历，建立待导入文件列表
	std::vector<Inputs> _inp;
	findAllFile(strFolderPath,_inp);	
	if (_inp.size()>0){
		if (inputFileCheck(_inp)){
			//文件过滤，打开文件后，依次读取每一行，如果这一行有中文字符，写入到过滤文件中去，
			//果没有就写入到文件名（1）中，最后删除原文件，把该文件改名
			for (int i = 0;i<_inp.size();i++){
				clock_t start, finish;
				start = clock();
				double  duration;
// 				ifstream ifnor;
// 				ifnor.open(LOC_FILTERNOR+_inp[i].theFileName,ios::binary|ios::in|ios::out);
// 				ifstream iffil;
// 				iffil.open(_inp[i].theFilePath,ios::binary|ios::in|ios::out);
				CTxtFileIO cioNor(LOC_FILTERNOR);
				//temp.txt内容长度置零				
				CreateFile(LOC_FILTERNOR,GENERIC_READ| GENERIC_WRITE, 0,NULL,  
					TRUNCATE_EXISTING,NULL,NULL);
				CTxtFileIO cioFil(LOC_FILTERLOG);
				cioFil.writeALine(_T("*******************************"));
				cioFil.writeALine(_inp[i].theFileName);
 				ifstream ifs;
				ifs.open(_inp[i].theFilePath,ios::binary|ios::in|ios::out);
				string s;
				int temp = 0;
				int temple = 0x80808080;
				while (getline(ifs,s)){
					int len = s.length();		
					char a[1024] ={0x00};
					s.copy(a,s.length()-1);
					int ia[1024] = {0x00};
					memcpy(ia,a,len-1);
					int coun = 0;BOOL flag = TRUE;
					CString tmpCstr(s.c_str());
					while (ia[coun] != 0x00){		
						temp = ia[coun] & temple;
						coun++;
						if ((temp) != 0x00){
							// 中文字符,输出，退出该句循环
							flag = FALSE;
							cioFil.writeALine(tmpCstr);
							break;
						}						
					}
					if (TRUE == flag){
						cioNor.writeALine(tmpCstr);
					}
				}

				finish = clock();
				CString strrrr;
				strrrr.Format(_T("%d"),(finish - start) );
				MessageBoxW(strrrr);

				//写入数据库
				CString procName=_T("[dbo].[csv2sql]");
				int inputType = 0;
				if (_inp[i].theFileName == _T("operation.csv")){
					inputType = 1;
				} 
				else if (_inp[i].theFileName == _T("alarm.csv")){
					inputType = 2;
				} 
				else{
					inputType = 3;
				}

				CADOConnection m_adoConnection(m_dbinfo);	
				CHK_HR_RETURN(m_adoConnection.OnInitAdo())
					//CHK_HR_RETURN(m_adoConnection.addParas(_T("filepath"),adVarWChar,adParamInput,_inp[i].theFilePath.GetLength()+1,(_variant_t)(_inp[i].theFilePath)))
					CHK_HR_RETURN(m_adoConnection.addParas(_T("filepath"),adVarWChar,adParamInput,sizeof(LOC_FILTERNOR)/sizeof(TCHAR),(_variant_t)(LOC_FILTERNOR)))
					CHK_HR_RETURN(m_adoConnection.addParas(_T("inputType"),adInteger,adParamInput,sizeof(int),(_variant_t)(inputType)))
					CHK_HR_RETURN(m_adoConnection.addParas(_T("departmentID"),adInteger,adParamInput,sizeof(int),(_variant_t)(_ttoi(_inp[i].theFolderName))))
					CHK_HR_RETURN(m_adoConnection.addParas(_T("UID"),adVarWChar,adParamInput,m_dbinfo.username.GetLength()+1,(_variant_t)(m_dbinfo.username)))
					CHK_HR_RETURN(m_adoConnection.addParas(_T("UPSW"),adVarWChar,adParamInput,m_dbinfo.password.GetLength()+1,(_variant_t)(m_dbinfo.password)))
					//		if (m_adoConnection.ExecuteProc(procName)){
					//LOG文件中添加记录		
					//				}
					m_adoConnection.ExitConnect();

			}
		}else{
			MessageBoxW(_T("文件夹内容错误"));
			return ;
		}

		MessageBoxW(_T("结束输入"));
	}
}

void CDlgMainpage::OnDataOutput(){
	//sql2csv
	//检测std::vector<OutputElement> outputElem内容的逻辑合理性
	int temp = listFilter();
	if (4 != temp){
		CString strTemp;
		strTemp.Format(_T("%d Error"),temp);
		MessageBoxW(strTemp);
		return ;
	}
	//检测日期合法性
	//end的时间大于等于start时间
	COleDateTime t1,t2,tempdate;
	m_DateStart.GetTime(t1);
	m_dateEnd.GetTime(t2);
	m_DateStart.GetTime(tempdate);
	if (t1>t2){
		MessageBoxW(_T("日期选择不正确"));
		return;
	}
	t1.SetDateTime(tempdate.GetYear(),tempdate.GetMonth(),tempdate.GetDay(),0,0,0);
	m_dateEnd.GetTime(tempdate);
	t2.SetDateTime(tempdate.GetYear(),tempdate.GetMonth(),tempdate.GetDay(),23,59,59);
	//获取路径
	TCHAR	szFolderPath[MAX_PATH] = {0};
	CString strFolderPath;	
	BROWSEINFO      sInfo;  
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));  
	sInfo.pidlRoot   = 0;  
	sInfo.lpszTitle   = _T("请选择一个文件夹：");  
	sInfo.ulFlags   = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX;  
	sInfo.lpfn     = NULL;  
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);   
	if (lpidlBrowse != NULL){  
		// 取得文件夹名  
		if (::SHGetPathFromIDList(lpidlBrowse,szFolderPath)){  
			strFolderPath = szFolderPath;  
		}  
	}  
	if(lpidlBrowse != NULL){  
		::CoTaskMemFree(lpidlBrowse);  
	}  
	//对文件夹的非法字符检查 空格，下划线，破折号都是非法字符
	OutputInfo outInfo;
	outInfo.info = m_dbinfo;
	outInfo.startDateTime = t1;
	outInfo.endDateTime = t2;
	outInfo.folderPath = strFolderPath;
	outInfo.hWnd = this->m_hWnd;
	outInfo.outputEle = outputEle;
	CDlgWaiting dlgWaiting(outInfo);
	dlgWaiting.DoModal();
}

void CDlgMainpage::OnReLogin(){
	//该用户注销，重新进入登录界面
	CDataNetIODlg dlg(this);
	dlg.DoModal();
}

void CDlgMainpage::OnExit(){
	//退出程序
	CWnd::DestroyWindow();
}

void CDlgMainpage::OnDBConfig(){
	//数据库信息配置
	CDbConf dbdlg;
	dbdlg.DoModal();
}

void CDlgMainpage::OnStaffMana(){
	//软件使用人员管理
	CStaffMana staffmanDlg;
	staffmanDlg.DoModal();
}

void CDlgMainpage::OnAbout(){
	//关于对话框
}

int CDlgMainpage::listFilter(){
	//0.禁止outputEle内容为空时输出			提示0
	//1.没有任何选择						提示1
	//2.禁止车间选择为是的项，文件选择为空	提示2
	//3.禁止车间选择为否的项，文件有选择	提示3
	//4.合法的输出							提示4

	if (outputEle.empty()){
		return 0;
	}	
	BOOL flagA = FALSE;
	for (size_t i = 0;i < outputEle.size();i++){
		if (outputEle[i].isDepartment){
			BOOL flag = FALSE; flagA = TRUE;
			for (size_t j = 0; j < outputEle[i].fileEle.size();j++){
				if (outputEle[i].fileEle[j].isCheck){
					flag = TRUE;
				}
			}
			if (!flag){
				return 2;
			}
		} 
		else{
			for (size_t j = 0; j < outputEle[i].fileEle.size();j++){
				if (outputEle[i].fileEle[j].isCheck){
					return 3;
				}
			}			
		}
	}
	if (!flagA){
		return 1;
	}
	return 4;	
}

void CDlgMainpage::showList(){
	int temp1 = m_listboxDepartment.GetCurSel();
	int temp2 = m_listboxOutputFileType.GetCurSel();
	showList(temp1,temp2);
}

void CDlgMainpage::showList(int selDepartment,int selFile){
	if (outputEle.empty()){
		return;
	}
	for (int i = m_listboxDepartment.GetCount();i>=0;i--){
		m_listboxDepartment.DeleteString(i);
	}
	for (int i = m_listboxOutputFileType.GetCount();i>=0;i--){
		m_listboxOutputFileType.DeleteString(i);
	}
	//显示Department部分
	for (size_t i = 0;i < outputEle.size();i++){
		m_listboxDepartment.AddString(outputEle[i].departName);
		if (outputEle[i].isDepartment){
			m_listboxDepartment.SetCheck(i,BST_CHECKED);
		}else{
			m_listboxDepartment.SetCheck(i,BST_UNCHECKED);
		}
	}
	//显示OutputFiletype部分
	int temp = 0;
	if (-1 != selDepartment){
		temp = selDepartment;
	}
	size_t j = 0;
	for (;j < outputEle[temp].fileEle.size(); j++){
		m_listboxOutputFileType.AddString(outputEle[temp].fileEle[j].fileName);
		if (outputEle[temp].fileEle[j].isCheck){
			m_listboxOutputFileType.SetCheck(j,BST_CHECKED);
		}else{
			m_listboxOutputFileType.SetCheck(j,BST_UNCHECKED);
		}
	}
	//设置选中状态
	m_listboxDepartment.SetCurSel(selDepartment);
	m_listboxOutputFileType.SetCurSel(selFile);
}

HRESULT CDlgMainpage::getConfigFromXML(){
	CXMLConnection xmlConn;
	xmlConn.initXML(LOC_DATABASE);
	HRESULT hr = S_OK;
	MSXML2::IXMLDOMNode* pRoot = NULL;	
	MSXML2::IXMLDOMNodeList* pinfos = NULL;
	CHK_HR_RETURNERR(xmlConn.getRoot(pRoot,_T("DataInfo")))
	CHK_HR_RETURNERR(pRoot->selectNodes(_T("Info"),&pinfos))
	MSXML2::IXMLDOMNode* pinfo = NULL;
	MSXML2::IXMLDOMNode* pinfoNode = NULL;
	BSTR bst;
	while(S_OK ==( pinfos->nextNode(&pinfo))){
		CHK_HR_RETURNERR(pinfo->selectSingleNode(_T("Server"),&pinfoNode))
		CHK_HR_RETURNERR(pinfoNode->get_text(&bst))
		m_dbinfo.server.Empty();m_dbinfo.server = bst;		
		CHK_HR_RETURNERR(pinfo->selectSingleNode(_T("Catalog"),&pinfoNode))
		CHK_HR_RETURNERR(pinfoNode->get_text(&bst))
		m_dbinfo.database.Empty();m_dbinfo.database = bst;	
		CHK_HR_RETURNERR(pinfo->selectSingleNode(_T("UserID"),&pinfoNode))
		CHK_HR_RETURNERR(pinfoNode->get_text(&bst))
		m_dbinfo.username.Empty();m_dbinfo.username = bst;	
		CHK_HR_RETURNERR(pinfo->selectSingleNode(_T("Password"),&pinfoNode))
		CHK_HR_RETURNERR(pinfoNode->get_text(&bst))
		m_dbinfo.password.Empty();m_dbinfo.password = bst;	
	}	
	xmlConn.exitXML();
	return S_OK;  // return TRUE unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CDlgMainpage, CDialogEx)
	ON_COMMAND(ID_START,&CDlgMainpage::OnStart)
	ON_COMMAND(ID_INPUTDATA,&CDlgMainpage::OnDataInput)
	ON_COMMAND(ID_OUTPUTDATA,&CDlgMainpage::OnDataOutput)
	ON_COMMAND(ID_RELOGIN,&CDlgMainpage::OnReLogin)
	ON_COMMAND(ID_EXIT,&CDlgMainpage::OnExit)
	ON_COMMAND(ID_DATABASECONFIG,&CDlgMainpage::OnDBConfig)
	ON_COMMAND(ID_STAFFMANA,&CDlgMainpage::OnStaffMana)
	ON_COMMAND(ID_ABOUT,&CDlgMainpage::OnAbout)
	ON_LBN_SELCHANGE(IDC_LIST_DEPARTMENT, &CDlgMainpage::OnLbnSelchangeListDepartment)
	ON_LBN_SELCHANGE(IDC_LIST_FILELIST, &CDlgMainpage::OnLbnSelchangeListFilelist)
	
	ON_BN_CLICKED(IDC_BUTTON_CONFIRM, &CDlgMainpage::OnBnClickedButtonConfirm)
END_MESSAGE_MAP()

void CDlgMainpage::OnLbnSelchangeListDepartment()
{
	// TODO: 在此添加控件通知处理程序代码
	int curSel = m_listboxDepartment.GetCurSel();
	if (curSel != -1){
		for (int i = 0 ;i<m_listboxDepartment.GetCount();i++){
			if (1 == m_listboxDepartment.GetCheck(i)){
				outputEle[i].isDepartment = TRUE;
			} else{
				outputEle[i].isDepartment = FALSE;
			}
		}
	}
	showList();
}

void CDlgMainpage::OnLbnSelchangeListFilelist()
{
	// TODO: 在此添加控件通知处理程序代码
	int curSel = m_listboxDepartment.GetCurSel();
	//int temp = 0;
	if (curSel != -1){
		for (size_t i = 0 ;i < outputEle[curSel].fileEle.size();i++){
			if (1 == m_listboxOutputFileType.GetCheck(i)){
				outputEle[curSel].fileEle[i].isCheck = TRUE;
			} else{
				outputEle[curSel].fileEle[i].isCheck = FALSE;
			}
		}
	}else{
		MessageBoxW(_T("未选择车间号"));
		return;
	}
}

void CDlgMainpage::OnBnClickedButtonConfirm()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CDlgMainpage::findAllFile(const CString& foldPath,std::vector<Inputs>& inp)
{
	CFileFind   ff; 
	BOOL bFound = ff.FindFile(foldPath + _T("\\*"),0); 
	while(bFound) { 
		bFound = ff.FindNextFile(); 
		if(ff.GetFileName()== _T(".")||ff.GetFileName()== _T("..")) 
			continue; 
		if(ff.IsDirectory()) {   
			//递归删除子文件夹 
			findAllFile(ff.GetFilePath(),inp);
		} else{ 
			Inputs _inp;
			CString _tmp(ff.GetFileName());
			if ((_tmp == _T("alarm.csv")) || (_tmp == _T("operation.csv")) ||(_tmp == _T("trans.csv"))){
				_inp.theFileName = _tmp;
				CString _cstrTmp = foldPath;
				_cstrTmp.MakeReverse();
				int npos = _cstrTmp.Find(_T("\\"));
				_inp.theFolderName = _cstrTmp.Left(npos);
				_inp.theFolderName.MakeReverse();
				_inp.theFilePath = ff.GetFilePath();
				inp.push_back(_inp);
			}
		} 
	} 
	ff.Close(); 
}

BOOL CDlgMainpage::inputFileCheck(const std::vector<Inputs>& inp){

	for (int i = 0;i<inp.size();i++){
		CString tmpA = inp[i].theFolderName;
		if (inp[i].theFolderName.SpanIncluding(_T("1234567890")) != inp[i].theFolderName){			
			return FALSE;
		}
		for (int j=0;j<inp.size();j++){
			if ((tmpA == inp[j].theFolderName) && (i != j)){
				if (inp[i].theFileName == inp[j].theFileName){
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}