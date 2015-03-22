// ImageWCSDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CameraMeasurement.h"
#include "ImageWCSDlg.h"
#include "afxdialogex.h"
#include "HalconHeader.h"

// CImageWCSDlg dialog

IMPLEMENT_DYNAMIC(CImageWCSDlg, CDialog)

	CImageWCSDlg::CImageWCSDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CImageWCSDlg::IDD, pParent)
	, m_CamParamFile(_T(""))
	, m_CamPoseFile(_T(""))
	, m_ImgRow(0)
	, m_ImgColumn(0)
	, m_WCSX(0)
	, m_WCSY(0)
	, m_Message(_T(""))
{
	InitSet();
}

void CImageWCSDlg::InitSet()
{
	CString exePath = GetExePath();

	m_CamParamFile = exePath + _T("Files\\camera_parameters.dat");
	m_CamPoseFile = exePath + _T("Files\\camera_pose.dat");

}

CString CImageWCSDlg::GetExePath()
{
	TCHAR exeFullPath[MAX_PATH]; // MAX_PATH在WINDEF.h中定义了，等于260  
	memset(exeFullPath,0,MAX_PATH);  

	GetModuleFileName(NULL,exeFullPath,MAX_PATH);  
	(_tcsrchr(exeFullPath, _T('\\')))[1] = 0;
	return exeFullPath;
}

CImageWCSDlg::~CImageWCSDlg()
{
}

void CImageWCSDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CAM_PARAM_FILE_EDIT_1, m_CamParamFile);
	DDX_Text(pDX, IDC_CAM_POSE_FILE_EDIT_1, m_CamPoseFile);
	DDX_Text(pDX, IDC_IMG_ROW_EDIT, m_ImgRow);
	DDX_Text(pDX, IDC_IMG_COL_EDIT, m_ImgColumn);
	DDX_Text(pDX, IDC_WCS_X_EDIT, m_WCSX);
	DDX_Text(pDX, IDC_WCS_Y_EDIT, m_WCSY);
	DDX_Text(pDX, IDC_IMG_WCS_MSG, m_Message);
}


BEGIN_MESSAGE_MAP(CImageWCSDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CImageWCSDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CAL_BTN, &CImageWCSDlg::OnBnClickedCalBtn)
END_MESSAGE_MAP()


// CImageWCSDlg message handlers


void CImageWCSDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//CDialog::OnOK();
}


void CImageWCSDlg::OnBnClickedCalBtn()
{
	UpdateData(true);
	m_Message = "Start Mesauring!";
	UpdateData(false);
	ExcuteCalculate();
	m_Message = "End Mesauring!";
	UpdateData(false);
}

void CImageWCSDlg::ExcuteCalculate()
{
	HTuple hv_Exception;

	HTuple hv_CamParam;
	HTuple hv_CamParamFile;
	hv_CamParamFile = (char*)LPCTSTR(m_CamParamFile);

	//Read the internal camera parameters from file
	try
	{
		ReadCamPar(hv_CamParamFile, &hv_CamParam);
	}
	// catch (Exception) 
	catch (HalconCpp::HException &HDevExpDefaultException)
	{
		HDevExpDefaultException.ToHTuple(&hv_Exception);
		//run 'camera_calibration_internal.hdev' first to generate camera
		//parameter file 'camera_parameters.dat'
		// stop(); only in hdevelop
	}

	HTuple hv_CamPose;
	HTuple hv_CamPoseFile = (char*)LPCTSTR(m_CamPoseFile);
	try
	{
		ReadPose(hv_CamPoseFile, &hv_CamPose);
	}
	// catch (Exception) 
	catch (HalconCpp::HException &HDevExpDefaultException)
	{
		HDevExpDefaultException.ToHTuple(&hv_Exception);
		//run 'camera_calibration_internal.hdev' first to generate camera
		//parameter file 'camera_parameters.dat'
		// stop(); only in hdevelop
	}

	HTuple hv_X, hv_Y;
	ImagePointsToWorldPlane(hv_CamParam, hv_CamPose, m_ImgRow, m_ImgColumn, 1, &hv_X, 
		&hv_Y);

	m_WCSX = (float)hv_X;
	m_WCSY = (float)hv_Y;
}
