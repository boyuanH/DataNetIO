// DlgMainpage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DataNetIO.h"
#include "DlgMainpage.h"
#include "afxdialogex.h"
#include "DbConf.h"
#include "ADOConnection.h"
#include "DlgWaiting.h"

// CDlgMainpage �Ի���

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
	// TODO:  �ڴ����Ӷ���ĳ�ʼ��
	pMenu  = new CMenu();
	if (m_isAdmin == TRUE){
		pMenu->LoadMenuW(IDR_MENU_ADMIN);
	}else{
		pMenu->LoadMenuW(IDR_MENU_NORM);
	}
	SetMenu(pMenu);
	ModifyStyleEx(0,WS_EX_APPWINDOW);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CDlgMainpage::OnStart(){
	//�������ݿ⣬����ɹ����������Ϣ��������ʾ�ڽ����У����Ա�֤���ݿ����������
	outputEle.clear();
	if (S_OK != getConfigFromXML()){
		MessageBoxW(_T("Failed on get Config File From XML"));
		return ;
	}	
	//���´����ݿ��л�ȡ����
	//�����ݿ��л�ȡ��ѡʱ�䷶Χ
	CADOConnection m_adoConnection(m_dbinfo);
	if (S_OK != m_adoConnection.OnInitAdo()){
		MessageBoxW(_T("�������ݿ�ʧ��"));
		return ;
	}
	_RecordsetPtr m_pRecordset;
	//�����ݿ��ʼ������
	CString	empSQL=_T("Select * from [caxastat].[dbo].[SYSCLASSFICATION]");
	m_pRecordset=m_adoConnection.GetRecordset(empSQL,adCmdText);
	//��ȡ���ݿ�����䳵�䣬ѡ������
	while (!m_pRecordset->TheAdoEOF){
		OutUnit ele;
		_variant_t temp = (m_pRecordset->GetCollect("classname"));
		_variant_t tempID = (m_pRecordset->GetCollect("objectid"));
		if (temp.vt != VT_NULL){
			//���ӳ���
			ele.departName = temp.bstrVal;
			ele.departID.Format(_T("%d"),tempID.intVal);
			ele.isDepartment= TRUE;
			//�����ļ�
			FileUint fu;
			fu.fileName =_T("�����ļ�");fu.isCheck = TRUE; ele.fileEle.push_back(fu);
			fu.fileName =_T("�����ļ�");fu.isCheck = TRUE; ele.fileEle.push_back(fu);
			fu.fileName =_T("�����ļ�");fu.isCheck = TRUE; ele.fileEle.push_back(fu);		
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
	//0 ��ȡ·��
	TCHAR	szFolderPath[MAX_PATH] = {0};
	CString strFolderPath;	
	BROWSEINFO      sInfo;  
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));  
	sInfo.pidlRoot   = 0;  
	sInfo.lpszTitle   = _T("��ѡ��һ���ļ��У�");  
	sInfo.ulFlags   = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX;  
	sInfo.lpfn     = NULL;  
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);   
	if (lpidlBrowse != NULL){  
		// ȡ���ļ�����  
		if (::SHGetPathFromIDList(lpidlBrowse,szFolderPath)){  
			strFolderPath = szFolderPath;  
		}  
	}  
	if(lpidlBrowse != NULL){  
		::CoTaskMemFree(lpidlBrowse);  
	}  
	//���ȡ�õ��ļ����������ڣ�����

	//1���ļ��н��б����������������ļ��б�
	std::vector<Inputs> _inp;
	findAllFile(strFolderPath,_inp);

	//2��ÿ���ļ����й��ˣ�����3������
	if (_inp.size()>0){
		if (inputFileCheck(_inp)){
			int t = _inp.size();
		}else{
			MessageBoxW(_T("�ļ������ݴ���"));
			return ;
		}		
	}

	//���δ��ļ��������ļ�����
	for (int i = 0; i < _inp.size(); i++){
		
	}

	//3ѭ�������������ݿ�

}

void CDlgMainpage::OnDataOutput(){
	//sql2csv
	//���std::vector<OutputElement> outputElem���ݵ��߼�������
	int temp = listFilter();
	if (4 != temp){
		CString strTemp;
		strTemp.Format(_T("%d Error"),temp);
		MessageBoxW(strTemp);
		return ;
	}
	//������ںϷ���
	//end��ʱ����ڵ���startʱ��
	COleDateTime t1,t2,tempdate;
	m_DateStart.GetTime(t1);
	m_dateEnd.GetTime(t2);
	m_DateStart.GetTime(tempdate);
	if (t1>t2){
		MessageBoxW(_T("����ѡ����ȷ"));
		return;
	}
	t1.SetDateTime(tempdate.GetYear(),tempdate.GetMonth(),tempdate.GetDay(),0,0,0);
	m_dateEnd.GetTime(tempdate);
	t2.SetDateTime(tempdate.GetYear(),tempdate.GetMonth(),tempdate.GetDay(),23,59,59);
	//��ȡ·��
	TCHAR	szFolderPath[MAX_PATH] = {0};
	CString strFolderPath;	
	BROWSEINFO      sInfo;  
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));  
	sInfo.pidlRoot   = 0;  
	sInfo.lpszTitle   = _T("��ѡ��һ���ļ��У�");  
	sInfo.ulFlags   = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX;  
	sInfo.lpfn     = NULL;  
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);   
	if (lpidlBrowse != NULL){  
		// ȡ���ļ�����  
		if (::SHGetPathFromIDList(lpidlBrowse,szFolderPath)){  
			strFolderPath = szFolderPath;  
		}  
	}  
	if(lpidlBrowse != NULL){  
		::CoTaskMemFree(lpidlBrowse);  
	}  
	//���ļ��еķǷ��ַ���� �ո��»��ߣ����ۺŶ��ǷǷ��ַ�

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
	//���û�ע�������½����¼����
}

void CDlgMainpage::OnExit(){
	//�˳�����
	CWnd::DestroyWindow();
}

void CDlgMainpage::OnDBConfig(){
	//���ݿ���Ϣ����
	CDbConf dbdlg;
	dbdlg.DoModal();
}

void CDlgMainpage::OnStaffMana(){
	//����ʹ����Ա����
}

void CDlgMainpage::OnAbout(){
	//���ڶԻ���
}

int CDlgMainpage::listFilter(){
	//0.��ֹoutputEle����Ϊ��ʱ���			��ʾ0
	//1.û���κ�ѡ��						��ʾ1
	//2.��ֹ����ѡ��Ϊ�ǵ���ļ�ѡ��Ϊ��	��ʾ2
	//3.��ֹ����ѡ��Ϊ�����ļ���ѡ��	��ʾ3
	//4.�Ϸ������							��ʾ4

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
	//��ʾDepartment����
	for (size_t i = 0;i < outputEle.size();i++){
		m_listboxDepartment.AddString(outputEle[i].departName);
		if (outputEle[i].isDepartment){
			m_listboxDepartment.SetCheck(i,BST_CHECKED);
		}else{
			m_listboxDepartment.SetCheck(i,BST_UNCHECKED);
		}
	}
	//��ʾOutputFiletype����
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
	//����ѡ��״̬
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
		m_dbinfo.server = bst;		
		CHK_HR_RETURNERR(pinfo->selectSingleNode(_T("Catalog"),&pinfoNode))
		CHK_HR_RETURNERR(pinfoNode->get_text(&bst))
		m_dbinfo.database = bst;
		CHK_HR_RETURNERR(pinfo->selectSingleNode(_T("UserID"),&pinfoNode))
		CHK_HR_RETURNERR(pinfoNode->get_text(&bst))
		m_dbinfo.username = bst;
		CHK_HR_RETURNERR(pinfo->selectSingleNode(_T("Password"),&pinfoNode))
		CHK_HR_RETURNERR(pinfoNode->get_text(&bst))
		m_dbinfo.password = bst;
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
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
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
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
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
		MessageBoxW(_T("δѡ�񳵼��"));
		return;
	}
}

void CDlgMainpage::OnBnClickedButtonConfirm()
{
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
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
			//�ݹ�ɾ�����ļ��� 
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
				inp.push_back(_inp);
			}
		} 
	} 
	ff.Close(); 
}

BOOL CDlgMainpage::inputFileCheck(const std::vector<Inputs>& inp){
	for (int i = 0;i<inp.size();i++){
		CString tmpA = inp[i].theFolderName;
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