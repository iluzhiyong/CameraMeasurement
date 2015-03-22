// CamCalibDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CameraMeasurement.h"
#include "CamCalibDlg.h"
#include "afxdialogex.h"
#include "HalconHeader.h"
#include "HalconCpp.h"

using namespace HalconCpp;
// CCamCalibDlg dialog

IMPLEMENT_DYNAMIC(CCamCalibDlg, CDialog)

	CCamCalibDlg::CCamCalibDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCamCalibDlg::IDD, pParent)
	, m_CalibModel(_T(""))
	, m_ImageDir(_T(""))
	, m_ImageName(_T(""))
	, m_ImageNums(0)
	, m_CamParamFile(_T(""))
	, m_CamPoseFile(_T(""))
	, m_CamPoseImage(_T(""))
	, m_Msg(_T(""))
{
	m_CalcParam = false;
	InitSet();
}

void CCamCalibDlg::InitSet()
{
	CString exePath = GetExePath();

	m_CalibModel = _T("caltab_30mm.descr");
	m_ImageDir = exePath + _T("Images\\CalibImages\\");
	m_ImageName = _T("calib_");
	m_ImageNums = 10;
	m_CamParamFile = exePath + _T("Files\\camera_parameters.dat");
	m_CamPoseFile = exePath + _T("Files\\camera_pose.dat");
	m_CamPoseImage = exePath + _T("Images\\CamPoseImges\\calib_11");

}

CString CCamCalibDlg::GetExePath()
{
	TCHAR exeFullPath[MAX_PATH]; // MAX_PATH在WINDEF.h中定义了，等于260  
	memset(exeFullPath,0,MAX_PATH);  

	GetModuleFileName(NULL,exeFullPath,MAX_PATH);  
	(_tcsrchr(exeFullPath, _T('\\')))[1] = 0;
	return exeFullPath;
}

CCamCalibDlg::~CCamCalibDlg()
{

}

void CCamCalibDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CALIB_MODEL_EDIT, m_CalibModel);
	DDX_Text(pDX, IDC_IMG_DIR_EDIT, m_ImageDir);
	DDX_Text(pDX, IDC_IMG_NAME_EDIT, m_ImageName);
	DDX_Text(pDX, IDC_IMG_NUM_EDIT, m_ImageNums);
	DDX_Text(pDX, IDC_CAM_PARAM_FILE_EDIT, m_CamParamFile);
	DDX_Text(pDX, IDC_CAM_POSE_FILE_EDIT, m_CamPoseFile);
	DDX_Text(pDX, IDC_CAM_POSE_IMAG_EDIT, m_CamPoseImage);
	DDX_Text(pDX, IDC_SHOWN_MSG, m_Msg);
}


BEGIN_MESSAGE_MAP(CCamCalibDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CCamCalibDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CALC_CAM_PARAM_BTN, &CCamCalibDlg::OnBnClickedCalcCamParamBtn)
	ON_BN_CLICKED(IDC_CAL_CAM_POSE_BTN, &CCamCalibDlg::OnBnClickedCalCamPoseBtn)
END_MESSAGE_MAP()


// CCamCalibDlg message handlers


void CCamCalibDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//CDialog::OnOK();
}


void CCamCalibDlg::OnBnClickedCalcCamParamBtn()
{
	UpdateData(true);
	m_Msg = _T("Start calculating camera parameter!");
	UpdateData(false);
	CalcCamParam();
	m_Msg = _T("End calculating camera parameter!");
	UpdateData(false);

	m_CalcParam = true;
}

void CCamCalibDlg::CalcCamParam()
{
	// Local iconic variables
	HObject  ho_Image, ho_Caltab;
	// Local control variables
	HTuple  hv_ImgPath;
	HTuple  hv_StartCamPar, hv_CalibDataID, hv_NumImages, hv_I;
	HTuple  hv_Error, hv_CamParam, hv_Message;
	HTuple  hv_CalibModel;
	HTuple  hv_CamParamFile;
	HTuple  hv_ImageName;

	hv_ImgPath = (char*)LPCTSTR(m_ImageDir);
	hv_ImageName = (char*)LPCTSTR(m_ImageName);
	hv_CalibModel = (char*)LPCTSTR(m_CalibModel);

	//Calibrate the camera.
	hv_StartCamPar.Clear();
	hv_StartCamPar[0] = 0.016;
	hv_StartCamPar[1] = 0;
	hv_StartCamPar[2] = 0.0000074;
	hv_StartCamPar[3] = 0.0000074;
	hv_StartCamPar[4] = 326;
	hv_StartCamPar[5] = 247;
	hv_StartCamPar[6] = 652;
	hv_StartCamPar[7] = 494;


	if (HDevWindowStack::IsOpen())
	{
		ReadImage(&ho_Image, (hv_ImgPath + hv_ImageName)+("01"));
		HTuple hv_width, hv_height;
		HalconCpp::GetImageSize(ho_Image, &hv_width, &hv_height);
		HalconCpp::SetPart(HDevWindowStack::GetActive(), 0, 0, hv_width - 1, hv_height -1);
	}
	

	CreateCalibData("calibration_object", 1, 1, &hv_CalibDataID);

	SetCalibDataCamParam(hv_CalibDataID, 0, "area_scan_division", hv_StartCamPar);

	SetCalibDataCalibObject(hv_CalibDataID, 0, hv_CalibModel);

	hv_NumImages = m_ImageNums;
	//Note, we do not use the image from which the pose of the measurement plane can be derived
	{
		HTuple end_val17 = hv_NumImages;
		HTuple step_val17 = 1;
		for (hv_I=1; hv_I.Continue(end_val17, step_val17); hv_I += step_val17)
		{
			ReadImage(&ho_Image, (hv_ImgPath + hv_ImageName)+(hv_I.TupleString("02d")));
			if (HDevWindowStack::IsOpen())
			{
				HalconCpp::DispObj(ho_Image, HDevWindowStack::GetActive());
			}
			FindCalibObject(ho_Image, hv_CalibDataID, 0, 0, hv_I, HTuple(), HTuple());
			GetCalibDataObservContours(&ho_Caltab, hv_CalibDataID, "caltab", 0, 0, hv_I);
			if (HDevWindowStack::IsOpen())
			{
				HalconCpp::SetColor(HDevWindowStack::GetActive(),"green");
			}
			if (HDevWindowStack::IsOpen())
			{
				HalconCpp::DispObj(ho_Caltab, HDevWindowStack::GetActive());
			}
		}
	}
	CalibrateCameras(hv_CalibDataID, &hv_Error);

	GetCalibData(hv_CalibDataID, "camera", 0, "params", &hv_CamParam);

	//Write the internal camera parameters to a file
	hv_CamParamFile = (char*)LPCTSTR(m_CamParamFile);
	WriteCamPar(hv_CamParam, hv_CamParamFile);

	ClearCalibData(hv_CalibDataID);
}


void CCamCalibDlg::OnBnClickedCalCamPoseBtn()
{
	if(m_CalcParam == false)
	{
		m_Msg = _T("Do not calculate camera parameter. Please calculate camera parameter first!");
		UpdateData(false);
	}
	else
	{
		UpdateData(true);
		m_Msg = _T("Start calculating camera pose!");
		UpdateData(false);
		CalCamPose();
		m_Msg = _T("End calculating camera pose!");
		UpdateData(false);
		m_CalcParam = false;
	}
}

void CCamCalibDlg::CalCamPose()
{
	UpdateData(true);

	// Local iconic variables
	HObject  ho_Image, ho_Caltab;

	// Local control variables
	HTuple  hv_ImgPath, hv_WindowHandle, hv_CamParam;
	HTuple  hv_Exception, hv_CaltabName, hv_CalibDataID, hv_RCoord;
	HTuple  hv_CCoord, hv_Index, hv_PoseForCalibrationPlate;
	HTuple  hv_finalPose, hv_Row, hv_Column, hv_Button, hv_X1;
	HTuple  hv_Y1;

	HTuple ho_PoseFilePath;

	//Read Image
	hv_ImgPath = (char*)LPCTSTR(m_CamPoseImage);
	ReadImage(&ho_Image, hv_ImgPath);

	if (HDevWindowStack::IsOpen())
	{
		HalconCpp::DispObj(ho_Image, HDevWindowStack::GetActive());
	}

	//Read Camera param
	HTuple hv_CamParamFile;
	hv_CamParamFile = (char*)LPCTSTR(m_CamParamFile);
	try
	{
		ReadCamPar(hv_CamParamFile, &hv_CamParam);
	}
	catch (HalconCpp::HException &HDevExpDefaultException)
	{
		HDevExpDefaultException.ToHTuple(&hv_Exception);
	}

	//calibration plate is positioned directly on the measurement plane
	hv_CaltabName = (char*)LPCTSTR(m_CalibModel);

	CreateCalibData("calibration_object", 1, 1, &hv_CalibDataID);

	SetCalibDataCamParam(hv_CalibDataID, 0, "area_scan_division", hv_CamParam);

	SetCalibDataCalibObject(hv_CalibDataID, 0, hv_CaltabName);

	FindCalibObject(ho_Image, hv_CalibDataID, 0, 0, 1, HTuple(), HTuple());

	GetCalibDataObservContours(&ho_Caltab, hv_CalibDataID, "caltab", 0, 0, 1);

	GetCalibDataObservPoints(hv_CalibDataID, 0, 0, 1, &hv_RCoord, &hv_CCoord, &hv_Index, &hv_PoseForCalibrationPlate);

	if (HDevWindowStack::IsOpen())
	{
		HalconCpp::DispObj(ho_Caltab, HDevWindowStack::GetActive());
	}

	//Write pose to file
	HTuple hv_CamPoseFile = (char*)LPCTSTR(m_CamPoseFile);
	WritePose(hv_PoseForCalibrationPlate, hv_CamPoseFile);

	ClearCalibData(hv_CalibDataID);
}
