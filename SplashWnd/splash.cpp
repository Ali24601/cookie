// CG: This file was added by the Splash Screen component.
// Splash.cpp : implementation file
//

#include "stdafx.h"  // e. g. stdafx.h
#include "resource.h"  // e.g. resource.h

#include "Splash.h"  // e.g. splash.h
#include "interFHead\version.h"
#include <fstream>
#include "interFHead/MainFrm.h"
#include "NetApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//   Splash Screen class

BOOL CSplashWnd::c_bShowSplashWnd;
CSplashWnd* CSplashWnd::c_pSplashWnd;
CSplashWnd::CSplashWnd()
{
}

CSplashWnd::~CSplashWnd()
{
	// Clear the static window pointer.
	ASSERT(c_pSplashWnd == this);
	c_pSplashWnd = NULL;
}

BEGIN_MESSAGE_MAP(CSplashWnd, CWnd)
	//{{AFX_MSG_MAP(CSplashWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSplashWnd::EnableSplashScreen(BOOL bEnable /*= TRUE*/)
{
	c_bShowSplashWnd = bEnable;
}

void CSplashWnd::ShowSplashScreen(CWnd* pParentWnd /*= NULL*/)
{
	if (!c_bShowSplashWnd || c_pSplashWnd != NULL)
		return;

	// Allocate a new splash screen, and create the window.
	c_pSplashWnd = new CSplashWnd;
	if (!c_pSplashWnd->Create(pParentWnd))
		delete c_pSplashWnd;
	else
		c_pSplashWnd->UpdateWindow();
}

BOOL CSplashWnd::PreTranslateAppMessage(MSG* pMsg)
{
	if (c_pSplashWnd == NULL)
		return FALSE;

	// If we get a keyboard or mouse message, hide the splash screen.
	if (pMsg->message == WM_KEYDOWN ||
	    pMsg->message == WM_SYSKEYDOWN ||
	    pMsg->message == WM_LBUTTONDOWN ||
	    pMsg->message == WM_RBUTTONDOWN ||
	    pMsg->message == WM_MBUTTONDOWN ||
	    pMsg->message == WM_NCLBUTTONDOWN ||
	    pMsg->message == WM_NCRBUTTONDOWN ||
	    pMsg->message == WM_NCMBUTTONDOWN)
	{
		c_pSplashWnd->HideSplashScreen();
		return TRUE;	// message handled here
	}

	return FALSE;	// message not handled
}

BOOL CSplashWnd::Create(CWnd* pParentWnd /*= NULL*/)
{
	//sll 2016.1.11
	///获得相对路径
	CString exePath;
	GetModuleFileName(NULL,exePath.GetBufferSetLength(MAX_PATH+1),MAX_PATH);   
	exePath.ReleaseBuffer();
	exePath = exePath.Left(exePath.ReverseFind('\\'));
	exePath.MakeLower();
	CString CustomizedDir(exePath+_T("\\images\\Customized\\"));
	CreateBMPFromBCK(CustomizedDir+_T("Welcome.bck"));

	BITMAP bm;
	HBITMAP hbmp;//位图句柄 
	//sll 2015.11.13
	hbmp= (HBITMAP)LoadImage( AfxGetInstanceHandle(),_T("images\\Customized\\Welcome.bmp"),IMAGE_BITMAP,0,0, 
		LR_LOADFROMFILE);

	//sll 2016.1.11
	DeleteBMPFile(CustomizedDir+_T("Welcome.bmp"));

	m_bitmap.DeleteObject();
	m_bitmap.Attach(hbmp);//绑定 
	m_bitmap.GetObject(sizeof(bm),&bm);//返回的宽度、高度和颜色格式信息的位图结构体

	if (!hbmp)
		return FALSE;
	return CreateEx(0,
		AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)),
		NULL, WS_POPUP | WS_VISIBLE, 0, 0, bm.bmWidth, bm.bmHeight, pParentWnd->GetSafeHwnd(), NULL);
}

void CSplashWnd::HideSplashScreen()
{
	// Destroy the window, and update the mainframe.
	DestroyWindow();
	AfxGetMainWnd()->ShowWindow(SW_SHOW);
	AfxGetMainWnd()->UpdateWindow();
}

void CSplashWnd::PostNcDestroy()
{
	// Free the C++ class.
	delete this;
}

int CSplashWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Center the window.
	CenterWindow();

	// Set a timer to destroy the splash screen.
	SetTimer(1, 2750, NULL);

	return 0;
}

void CSplashWnd::OnPaint()
{
	CPaintDC dc(this);

	CDC dcImage;
	if (!dcImage.CreateCompatibleDC(&dc))
		return;

	BITMAP bm;
	m_bitmap.GetBitmap(&bm);

	// Paint the image.
	CBitmap* pOldBitmap = dcImage.SelectObject(&m_bitmap);
	dc.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &dcImage, 0, 0, SRCCOPY);
	dcImage.SelectObject(pOldBitmap);
}

void CSplashWnd::OnTimer(UINT nIDEvent)
{
	// Destroy the splash screen window.
	HideSplashScreen();
}

bool CSplashWnd::CreateBMPFromBCK( CString fileName )
{
	setlocale(LC_ALL,"Chinese-simplified");//设置中文环境
	std::fstream bckFile(fileName, std::ios_base::in | std::ios_base::out | std::ios::binary );///| ios::nocreate);
	CString newFile(fileName);
	newFile.Replace(_T(".bck"),_T(".bmp"));
	std::fstream bmpFile;
	bmpFile.open(newFile,  std::ios_base::out | std::ios::binary );
	setlocale(LC_ALL,"C");//还原
	if(bckFile.good())
	{
		char buf[2] = {'3','v'};
		buf[0] = ~buf[0];
		buf[1] = ~buf[1];
		bckFile.seekg(0);
		char *str1=new char[2];
		bckFile.get(str1[0]);
		bckFile.get(str1[1]);
		if (buf[0]==str1[0]&&buf[1]==str1[1])
		{
			char bmphead[2] = {'B','M'};
			bmpFile << bmphead[0];
			bmpFile << bmphead[1];
			char ch;
			while (bckFile.get(ch)) 
				bmpFile << ch;
		}
	}
	else
	{
		return NULL;
	}
	bckFile.close();
	bmpFile.close();
	return true;
}

bool CSplashWnd::DeleteBMPFile( CString fileName )
{
	int result=remove(NetApp::WS2S(fileName).c_str());
	if (result==0)
	{
		return true;
	}
	else if (result==-1)
	{
		return false;
	}
	else
		return false;
}
