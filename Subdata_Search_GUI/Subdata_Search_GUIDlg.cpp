
// Subdata_Search_GUIDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "Subdata_Search_GUI.h"
#include "Subdata_Search_GUIDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSubdataSearchGUIDlg 对话框

CSubdataSearchGUIDlg::CSubdataSearchGUIDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SUBDATA_SEARCH_GUI_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSubdataSearchGUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, Result);
	DDX_Control(pDX, IDC_MFCEDITBROWSE1, DirectoryURL);
	DDX_Control(pDX, IDC_BUTTON1, buttonStartSearch);
	DDX_Control(pDX, IDC_PROGRESS1, progressCtrl);
	DDX_Control(pDX, IDC_RADIO1, radioText);
	DDX_Control(pDX, IDC_RADIO2, radioHex);
	DDX_Control(pDX, IDC_EDIT1, editData);
	DDX_Control(pDX, IDC_EDIT2, editTips);
	DDX_Control(pDX, IDC_EDIT3, editProcessNum);
	DDX_Control(pDX, IDC_SPIN1, spinProcessNum);
}

BEGIN_MESSAGE_MAP(CSubdataSearchGUIDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CSubdataSearchGUIDlg::StartSearch)
	ON_EN_UPDATE(IDC_EDIT3, &CSubdataSearchGUIDlg::OnEnUpdateEdit3)
END_MESSAGE_MAP()


// CSubdataSearchGUIDlg 消息处理程序

BOOL CSubdataSearchGUIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CString cstrColumn[] = { TEXT("文件名"),TEXT("偏移量") };
	for (int i = 0; i < 2; i++)
		Result.InsertColumn(i, cstrColumn[i], LVCFMT_LEFT, 200);

	spinProcessNum.SetRange(1, THREAD_NUM_MAX);
	spinProcessNum.SetBuddy(&editProcessNum);

	editProcessNum.SetWindowText(TEXT("8"));
	radioText.SetCheck(TRUE);
	progressCtrl.SetRange(0, PROGRESS_CTRL_LEN);
	Result.SetExtendedStyle(Result.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSubdataSearchGUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSubdataSearchGUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSubdataSearchGUIDlg::StartSearch()
{
	// TODO: 在此添加控件通知处理程序代码
	CString cstr;
	int byteDataSize;
	char* byteData = NULL;
	static _ExecuteByteData_t byteData_t;

	try
	{
		DirectoryURL.GetWindowText(cstr);
		if (!cstr.Compare(TEXT("")))
			ErrorTips(TEXT("路径为空"));

		editData.GetWindowText(cstr);
		if (!cstr.Compare(TEXT("")))
			ErrorTips(TEXT("内容为空"));

		if (radioText.GetCheck())
		{
			byteData = new char[cstr.GetLength()];
			byteDataSize = cstr.GetLength();
			for (int i = 0; i < byteDataSize; i++)
				byteData[i] = cstr.operator LPCTSTR()[i];
		}
		else
		{
			if (cstr.GetLength() % 2 != 0)
				ErrorTips(TEXT("不能接受的二进制数据"));

			CString Hex("0123456789ABCDEF");
			char hexData_1;
			char hexData_2;

			byteData = new char[cstr.GetLength() / 2];
			byteDataSize = cstr.GetLength() / 2;
			for (int i = 0; i < byteDataSize; i++)
			{
				hexData_1 = Hex.Find(cstr[i * 2]) * 16;
				hexData_2 = Hex.Find(cstr[i * 2 + 1]);
				if (hexData_1 < 0 || hexData_2 < 0)
					ErrorTips(TEXT("不能接受的二进制数据"));

				byteData[i] = hexData_1 + hexData_2;
			}
		}
	}
	catch (const TCHAR* cstrErrorTips)
	{
		editTips.SetWindowText(cstrErrorTips);
		UpdateWindow();
		return;
	}

	if (hThread != NULL)
		CloseHandle(hThread);
	CString cstrProcessNum;
	editProcessNum.GetWindowText(cstrProcessNum); // 线程数

	// 这是传入子线程的结构 不应该被销毁
	byteData_t = { byteDataSize,byteData,atoi(CStringA(cstrProcessNum)),this };
	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Execute, (LPVOID)&byteData_t, 0, NULL);
}

DWORD WINAPI CSubdataSearchGUIDlg::Execute(LPVOID lpParameter)
{
	_ExecuteByteData_t* pByteData_t = (_ExecuteByteData_t*)lpParameter;

	std::ifstream finFileData;
	std::vector<_SearchList_t> vsearchList;
	std::vector<CString> vcstrFiles;
	CString cstr;

	int byteDataSize = pByteData_t->byteDataSize;
	char* byteData = pByteData_t->byteData;
	int iProcessNum = pByteData_t->iProcessNum;
	CSubdataSearchGUIDlg* pDlg = pByteData_t->pDlg;

	pDlg->buttonStartSearch.EnableWindow(FALSE);
	pDlg->Result.DeleteAllItems();

	pDlg->editTips.SetWindowText(TEXT("正在预处理..."));
	pDlg->UpdateWindow();

	// 构建next数组
	Kmp kmp(byteData, byteDataSize);
	// 查找所有文件
	pDlg->DirectoryURL.GetWindowText(cstr);
	pDlg->fileFinder.GetFiles(cstr, vcstrFiles);

	pDlg->editTips.SetWindowText(TEXT("正在处理...(1/2)"));
	pDlg->UpdateWindow();

	pDlg->progressCtrl.SetPos(0);
	/*************************************************************************/
	HANDLE* hThread = new HANDLE[iProcessNum];
	HANDLE hMutex_1 = CreateMutex(NULL, FALSE, NULL);
	HANDLE hMutex_2 = CreateMutex(NULL, FALSE, NULL);

	_ProcessUnitData_t processUnitData_t = { 0,vcstrFiles,vsearchList,kmp,hMutex_1,hMutex_2,pDlg };
	for (int i = 0; i < iProcessNum; i++)
		hThread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ProcessUnit, (LPVOID)&processUnitData_t, 0, NULL);
	// 等待处理单元
	WaitForMultipleObjects(iProcessNum, hThread, TRUE, INFINITE);

	CloseHandle(hMutex_1);
	CloseHandle(hMutex_2);
	delete[] hThread;
	/*************************************************************************/
	pDlg->progressCtrl.SetPos(PROGRESS_CTRL_LEN);

	pDlg->editTips.SetWindowText(TEXT("正在处理...(2/2)"));
	pDlg->UpdateWindow();

	pDlg->progressCtrl.SetPos(0); // 显示结果
	int iSearchListSize = vsearchList.size();
	for (int i = 0; i < iSearchListSize; i++)
	{
		for (auto pos : vsearchList[i].posList)
		{
			cstr.Format(TEXT("%d"), pos);
			pDlg->Result.InsertItem(0, vsearchList[i].cstrFilename);
			pDlg->Result.SetItemText(0, 1, cstr);
		}

		pDlg->progressCtrl.SetPos((int)((float)i / iSearchListSize * PROGRESS_CTRL_LEN));
	}
	pDlg->progressCtrl.SetPos(PROGRESS_CTRL_LEN);

	cstr.Format(TEXT("完成 找到%d个结果"), pDlg->Result.GetItemCount());
	pDlg->editTips.SetWindowText(cstr);
	pDlg->UpdateWindow();

	pDlg->buttonStartSearch.EnableWindow(TRUE);
	return 0;
}

DWORD WINAPI CSubdataSearchGUIDlg::ProcessUnit(LPVOID lpParameter)
{
	_ProcessUnitData_t* pProcessUnitData_t = (_ProcessUnitData_t*)lpParameter;

	std::ifstream finFileData;
	CString cstrFilename;

	std::vector<CString>& vcstrFiles = pProcessUnitData_t->vcstrFiles;
	std::vector<_SearchList_t>& vsearchList = pProcessUnitData_t->vsearchList;
	HANDLE hMutex_1 = pProcessUnitData_t->hMutex_1;
	HANDLE hMutex_2 = pProcessUnitData_t->hMutex_2;
	CSubdataSearchGUIDlg* pDlg = pProcessUnitData_t->pDlg;

	int iFilesListSize = vcstrFiles.size();
	while (TRUE)
	{	
		// 获取互斥元 访问文件名列表
		WaitForSingleObject(hMutex_1, INFINITE);
		if (pProcessUnitData_t->index >= iFilesListSize)
		{
			ReleaseMutex(hMutex_1);
			break;
		}
		cstrFilename = vcstrFiles[pProcessUnitData_t->index++];	
		ReleaseMutex(hMutex_1);
	
		Kmp kmp(pProcessUnitData_t->pKmp);
		finFileData.open(cstrFilename, std::ios_base::in | std::ios_base::binary);
		std::vector<int> vPosList;
		int index = kmp.frun(&finFileData);

		bool searchFlag = false;
		while (index != -1)
		{
			searchFlag = true;
			vPosList.push_back(index);
			index = kmp.frun();
		}
		finFileData.close();
		kmp.~Kmp();

		if (searchFlag)
		{
			// 获取互斥元 访问搜索结果列表
			WaitForSingleObject(hMutex_2, INFINITE);
			vsearchList.push_back(_SearchList_t{ cstrFilename, vPosList });
			pDlg->progressCtrl.SetPos((int)((float)pProcessUnitData_t->index / iFilesListSize * PROGRESS_CTRL_LEN));
			ReleaseMutex(hMutex_2);
		}	
	}
	return 0;
}

void CSubdataSearchGUIDlg::OnEnUpdateEdit3()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数，以将 EM_SETEVENTMASK 消息发送到该控件，
	// 同时将 ENM_UPDATE 标志“或”运算到 lParam 掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString cstrProcessNum;
	editProcessNum.GetWindowText(cstrProcessNum);
	int iProcessNum = atoi(CStringA(cstrProcessNum));



	if (iProcessNum > THREAD_NUM_MAX)
	{
		cstrProcessNum.Format(TEXT("%d"), THREAD_NUM_MAX);
		editProcessNum.SetWindowText(cstrProcessNum);
	}
	else if (iProcessNum < 1)
		editProcessNum.SetWindowText(TEXT("1"));
}