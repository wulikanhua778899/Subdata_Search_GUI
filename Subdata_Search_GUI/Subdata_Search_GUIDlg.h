
// Subdata_Search_GUIDlg.h: 头文件
//
#include "FileFinder.h"
#include "Kmp.h"
#include <fstream>

#pragma once


// CSubdataSearchGUIDlg 对话框
class CSubdataSearchGUIDlg : public CDialogEx
{
// 构造
public:
	CSubdataSearchGUIDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SUBDATA_SEARCH_GUI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	virtual void OnOK() {};
	virtual void OnClear() {};
private:
	struct _SearchList_t
	{
		CString cstrFilename;
		std::vector<int> posList;
	};
	struct _ExecuteByteData_t
	{
		int byteDataSize;
		char* byteData;
		int iProcessNum;
		CSubdataSearchGUIDlg* pDlg;
	};
	struct _ProcessUnitData_t
	{
		int index;

		std::vector<CString>& vcstrFiles;
		std::vector<_SearchList_t>& vsearchList;
		Kmp& pKmp;
		HANDLE hMutex_1;
		HANDLE hMutex_2;
		CSubdataSearchGUIDlg* pDlg;
	};

#define PROGRESS_CTRL_LEN 100
#define THREAD_NUM_MAX 256
	FileFinder fileFinder;
	HANDLE hThread;

	CListCtrl Result;
	CMFCEditBrowseCtrl DirectoryURL;	
	CProgressCtrl progressCtrl;
	CButton buttonStartSearch;
	CButton radioText;
	CButton radioHex;
	CEdit editData;
	CEdit editTips;
	CEdit editProcessNum;
	CSpinButtonCtrl spinProcessNum;

	inline void ErrorTips(TCHAR* cstrInfo) { throw(cstrInfo); };
	static DWORD WINAPI Execute(LPVOID lpParameter);
	static DWORD WINAPI ProcessUnit(LPVOID lpParameter);

	afx_msg void StartSearch();	
	afx_msg void OnEnUpdateEdit3();
};
