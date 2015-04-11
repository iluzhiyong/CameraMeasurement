// ImageProcessDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CameraMeasurement.h"
#include "ImageProcessDlg.h"
#include "afxdialogex.h"
#include "CameraMeasurementDlg.h"


// CImageProcessDlg dialog

IMPLEMENT_DYNAMIC(CImageProcessDlg, CDialog)

CImageProcessDlg::CImageProcessDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CImageProcessDlg::IDD, pParent)
	, m_CamParamFile(_T(""))
	, m_CamPoseFile(_T(""))
	, m_ImgRow(0)
	, m_ImgCol(0)
	, m_ImgCX(0)
	, m_ImgCY(0)
	, m_CRow(0)
	, m_CCol(0)
	, m_CCX(0)
	, m_CCY(0)
{
	m_CamMeasureDlg = NULL;
	InitSet();
}

void CImageProcessDlg::InitSet()
{
	CString exePath = GetExePath();

	m_CamParamFile = exePath + _T("Files\\camera_parameters.dat");
	m_CamPoseFile = exePath + _T("Files\\camera_pose.dat");

}


CString CImageProcessDlg::GetExePath()
{
	TCHAR exeFullPath[MAX_PATH]; // MAX_PATH在WINDEF.h中定义了，等于260  
	memset(exeFullPath,0,MAX_PATH);  

	GetModuleFileName(NULL,exeFullPath,MAX_PATH);  
	(_tcsrchr(exeFullPath, _T('\\')))[1] = 0;
	return exeFullPath;
}

CImageProcessDlg::~CImageProcessDlg()
{
}

void CImageProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CAM_PARAM_FILE_EDIT_P, m_CamParamFile);
	DDX_Text(pDX, IDC_CAM_POSE_FILE_EDIT_P, m_CamPoseFile);
	DDX_Text(pDX, IDC_IMG_C_R_EDIT, m_ImgRow);
	DDX_Text(pDX, IDC_IMG_C_CL_EDIT, m_ImgCol);
	DDX_Text(pDX, IDC_IMG_C_X_EDIT, m_ImgCX);
	DDX_Text(pDX, IDC_IMG_C_Y_EDIT, m_ImgCY);
	DDX_Text(pDX, IDC_CIRCLE_C_R_EDIT, m_CRow);
	DDX_Text(pDX, IDC_CIRCLE_C_CL_EDIT, m_CCol);
	DDX_Text(pDX, IDC_CIRCLE_C_X_EDIT, m_CCX);
	DDX_Text(pDX, IDC_CIRCLE_C_Y_EDIT, m_CCY);
}


BEGIN_MESSAGE_MAP(CImageProcessDlg, CDialog)
	ON_BN_CLICKED(IDC_IMG_CNETER_BTN, &CImageProcessDlg::OnBnClickedImgCneterBtn)
	ON_BN_CLICKED(IDC_CIRCLE_CENTER_BTN, &CImageProcessDlg::OnBnClickedCircleCenterBtn)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CImageProcessDlg message handlers


void CImageProcessDlg::OnBnClickedImgCneterBtn()
{
	LoadCamParamPoseFile();

	if(m_CamMeasureDlg != NULL)
	{
		if (HDevWindowStack::IsOpen())
		{
			DispObj(m_CamMeasureDlg->hv_image, HDevWindowStack::GetActive());

			HTuple hv_width,hv_height;
			HalconCpp::GetImageSize(m_CamMeasureDlg->hv_image, &hv_width, &hv_height);

			m_ImgRow = hv_height / 2;
			m_ImgCol = hv_width / 2;

			SetColor(HDevWindowStack::GetActive(),"green");

		    DispCross(HDevWindowStack::GetActive(), m_ImgRow, m_ImgCol, 10, 0);

			HTuple hv_X, hv_Y;
			ImagePointsToWorldPlane(hv_CamParam, hv_CamPose, m_ImgRow, m_ImgCol, 1, &hv_X, 
				&hv_Y);

			m_ImgCX = hv_X;
			m_ImgCY = hv_Y;

			UpdateData(FALSE);
		}
	}
}



void CImageProcessDlg::OnBnClickedCircleCenterBtn()
{
	LoadCamParamPoseFile();
	if(m_CamMeasureDlg != NULL)
	{
		// Local iconic variables
		HObject  ho_Edges, ho_Holes, ho_Hole;

		HTuple  hv_Row, hv_Column, hv_Radius, hv_Number;

		EdgesSubPix(m_CamMeasureDlg->hv_image, &ho_Edges, "canny", 4, 20, 40);
		SelectShapeXld(ho_Edges, &ho_Holes, "circularity", "and", 0.7, 1.0);
		SortContoursXld(ho_Holes, &ho_Holes, "upper_left", "true", "row");

		if (HDevWindowStack::IsOpen())
		{
			SetColor(HDevWindowStack::GetActive(),"yellow");
		}

		//Determine the midpoints
		SmallestCircleXld(ho_Holes, &hv_Row, &hv_Column, &hv_Radius);
		CountObj(ho_Holes, &hv_Number);

		//For all boles ...
		{
			HTuple end_val20 = hv_Number;
			HTuple step_val20 = 1;

			HTuple hv_i;

			for (hv_i=1; hv_i.Continue(end_val20, step_val20); hv_i += step_val20)
			{
				SelectObj(ho_Holes, &ho_Hole, hv_i);

				m_CRow = hv_Row[hv_i-1];
				m_CCol = hv_Column[hv_i-1];


				HTuple hv_X, hv_Y;
				ImagePointsToWorldPlane(hv_CamParam, hv_CamPose, m_CRow, m_CCol, 1, &hv_X, &hv_Y);

				m_CCX = hv_X;
				m_CCY = hv_Y;

				if (HDevWindowStack::IsOpen())
				{
					DispObj(ho_Hole, HDevWindowStack::GetActive());
					DispCross(HDevWindowStack::GetActive(), m_CRow, m_CCol, 10, 0);
				}
				UpdateData(FALSE);
				break;  //TODO: 
			}
		}
	}
}

void CImageProcessDlg::LoadCamParamPoseFile()
{
	HTuple hv_Exception;

	HTuple hv_CamParamFile = (char*)LPCTSTR(m_CamParamFile);

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
}

