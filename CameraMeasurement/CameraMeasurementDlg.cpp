
// CameraMeasurementDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CameraMeasurement.h"
#include "CameraMeasurementDlg.h"
#include "afxdialogex.h"
#include "CamCalibDlg.h"
#include "ImageWCSDlg.h"
#include "HalconHeader.h"
#include "ImageProcessDlg.h"

using namespace HalconCpp;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCameraMeasurementDlg dialog




CCameraMeasurementDlg::CCameraMeasurementDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCameraMeasurementDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_CamCalibDlg = NULL;
	m_ImageWCSDlg = NULL;
	m_ImageProcessDlg = NULL; 

}

CCameraMeasurementDlg::~CCameraMeasurementDlg()
{
	if(m_CamCalibDlg != NULL)
	{
		delete m_CamCalibDlg;
		m_CamCalibDlg = NULL;
	}
	if(m_ImageWCSDlg != NULL)
	{
		delete m_ImageWCSDlg;
		m_ImageWCSDlg = NULL;
	}
	if(m_ImageProcessDlg != NULL)
	{
		delete m_ImageProcessDlg;
		m_ImageProcessDlg = NULL;
	}
}

void CCameraMeasurementDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCameraMeasurementDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CCameraMeasurementDlg::OnBnClickedOk)
	
	ON_BN_CLICKED(IDC_CAM_CALIB_BTN, &CCameraMeasurementDlg::OnBnClickedCamCalibBtn)
	ON_BN_CLICKED(IDC_IMAGE_WCS_BTN, &CCameraMeasurementDlg::OnBnClickedImageWcsBtn)
	ON_BN_CLICKED(IDC_OPEN_IMG_BTN, &CCameraMeasurementDlg::OnBnClickedOpenImgBtn)
	ON_BN_CLICKED(IDC_PROCESS_IMG_BTN, &CCameraMeasurementDlg::OnBnClickedProcessImgBtn)
	ON_STN_CLICKED(IDC_SHOW_IMAGE, &CCameraMeasurementDlg::OnStnClickedShowImage)
END_MESSAGE_MAP()


// CCameraMeasurementDlg message handlers

BOOL CCameraMeasurementDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	OpenHalconWind();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCameraMeasurementDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCameraMeasurementDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCameraMeasurementDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CCameraMeasurementDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//CDialogEx::OnOK();
}




void CCameraMeasurementDlg::OnBnClickedOpenImgBtn()
{
	CFileDialog dlg(TRUE);

	INT_PTR retval = dlg.DoModal();
	if(retval==IDCANCEL)
	{
		return;
	}

	HTuple hv_Exception;
	try
	{
		HalconCpp::ReadImage(&hv_image, (char*)LPCTSTR(dlg.GetPathName()));
	}
	// catch (Exception) 
	catch (HalconCpp::HException &HDevExpDefaultException)
	{
		HDevExpDefaultException.ToHTuple(&hv_Exception);
	}


	DisplayImage(hv_image, true);
}

void CCameraMeasurementDlg::OpenHalconWind()
{
	CRect rtWindow;

	HWND hImgWnd = GetDlgItem( IDC_SHOW_IMAGE)->m_hWnd;

	GetDlgItem( IDC_SHOW_IMAGE)->GetClientRect(&rtWindow);

	HalconCpp::SetWindowAttr("background_color","black");

	HalconCpp::OpenWindow(rtWindow.left,rtWindow.top, rtWindow.Width(),rtWindow.Height(),(Hlong)hImgWnd,"","",&hv_WindowID);

	HalconCpp::SetPart(hv_WindowID, 0, 0, rtWindow.Height() -1, rtWindow.Width() - 1);

	HDevWindowStack::Push(hv_WindowID);
}


void CCameraMeasurementDlg::DisplayImage(HObject image, bool resize)
{
	if(resize && HDevWindowStack::IsOpen())
	{
		HTuple hv_width,hv_height;
		HalconCpp::GetImageSize(hv_image, &hv_width, &hv_height);
		HalconCpp::SetPart(HDevWindowStack::GetActive(), 0, 0, hv_height -1, hv_width - 1);
	}

	if (HDevWindowStack::IsOpen())
	{
		HalconCpp::DispObj(hv_image, HDevWindowStack::GetActive());
	}
}

void CCameraMeasurementDlg::OnBnClickedProcessImgBtn()
{
	if(m_ImageProcessDlg == NULL)
	{
		m_ImageProcessDlg = new CImageProcessDlg();
		m_ImageProcessDlg->m_CamMeasureDlg = this;
		m_ImageProcessDlg->Create(CImageProcessDlg::IDD, this);
	}
	m_ImageProcessDlg->ShowWindow(SW_SHOW);
}

void CCameraMeasurementDlg::OnBnClickedCamCalibBtn()
{
	if(m_CamCalibDlg == NULL)
	{
		m_CamCalibDlg = new CCamCalibDlg();
		m_CamCalibDlg->Create(CCamCalibDlg::IDD, this);
	}
	m_CamCalibDlg->ShowWindow(SW_SHOW);
}


void CCameraMeasurementDlg::OnBnClickedImageWcsBtn()
{
	if(m_ImageWCSDlg == NULL)
	{
		m_ImageWCSDlg = new CImageWCSDlg();
		m_ImageWCSDlg->Create(CImageWCSDlg::IDD, this);
	}
	m_ImageWCSDlg->ShowWindow(SW_SHOW);
}

void CCameraMeasurementDlg::OnStnClickedShowImage()
{
	// TODO: Add your control notification handler code here
}
