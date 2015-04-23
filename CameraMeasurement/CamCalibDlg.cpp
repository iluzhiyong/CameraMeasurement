// CamCalibDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CameraMeasurement.h"
#include "AutoHListBox.h"
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
	, m_AccuracyImage(_T(""))
	, m_ModelRow(7)
	, m_ModelColumn(7)
	, m_InitCamParam(_T(""))
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
	m_AccuracyImage = exePath + _T("Images\\AccuracyImges\\calib_12");
	m_InitCamParam = exePath + _T("Files\\InitCameraParamFile\\camera_parameters.dat");
	

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
	DDX_Text(pDX, IDC_CAM_PARAM_FILE_EDIT, m_CamParamFile);
	DDX_Text(pDX, IDC_CAM_POSE_FILE_EDIT, m_CamPoseFile);
	DDX_Text(pDX, IDC_CAM_POSE_IMAG_EDIT, m_CamPoseImage);
	DDX_Text(pDX, IDC_SHOWN_MSG, m_Msg);
	DDX_Text(pDX, IDC_CAM_ACCURACY_IMAG_EDIT, m_AccuracyImage);
	DDX_Text(pDX, IDC_CALIB_MODEL_ROW_EDIT, m_ModelRow);
	DDX_Text(pDX, IDC_CALIB_MODEL_COLUMN_EDIT, m_ModelColumn);
	DDX_Control(pDX, IDC_LIST1, m_ImageList);
	DDX_Text(pDX, IDC_INIT_CAM_PARAM_FILE_EDIT, m_InitCamParam);
}


BEGIN_MESSAGE_MAP(CCamCalibDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CCamCalibDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CALC_CAM_PARAM_BTN, &CCamCalibDlg::OnBnClickedCalcCamParamBtn)
	ON_BN_CLICKED(IDC_CAL_CAM_POSE_BTN, &CCamCalibDlg::OnBnClickedCalCamPoseBtn)
	ON_BN_CLICKED(IDC_CAL_ACCURACY_BTN, &CCamCalibDlg::OnBnClickedCalAccuracyBtn)
	ON_BN_CLICKED(IDC_SELECT_CALIB_IMAGE_BTN, &CCamCalibDlg::OnBnClickedSelectCalibImageBtn)
	ON_BN_CLICKED(IDC_SELECT_CALIB_MODEL_BTN, &CCamCalibDlg::OnBnClickedSelectCalibModelBtn)
	ON_BN_CLICKED(IDC_SELECT_CAM_PARAM_BTN, &CCamCalibDlg::OnBnClickedSelectCamParamBtn)
	ON_BN_CLICKED(IDC_SELECT_CAM_POSE_IMAGE_BTN, &CCamCalibDlg::OnBnClickedSelectCamPoseImageBtn)
	ON_BN_CLICKED(IDC_SELECT_CAM_POSE_FILE_BTN, &CCamCalibDlg::OnBnClickedSelectCamPoseFileBtn)
	ON_BN_CLICKED(IDC_SELECT_CAL_ACCURACY_IMAGE_BTN, &CCamCalibDlg::OnBnClickedSelectCalAccuracyImageBtn)
	ON_BN_CLICKED(IDC_SELECT_INIT_CAM_PARAM_BTN, &CCamCalibDlg::OnBnClickedSelectInitCamParamBtn)
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
	HTuple  hv_StartCamPar, hv_CalibDataID, hv_NumImages;
	HTuple  hv_Error, hv_CamParam, hv_Message;
	HTuple  hv_CalibModel;
	HTuple  hv_CamParamFile;
	HTuple  hv_ImageName;

	CString msg;

	hv_ImgPath = (char*)LPCTSTR(m_ImageDir);
	hv_ImageName = (char*)LPCTSTR(m_ImageName);
	hv_CalibModel = (char*)LPCTSTR(m_CalibModel);

	//Read Camera param
	hv_CamParamFile = (char*)LPCTSTR(m_InitCamParam);
	try
	{
		ReadCamPar(hv_CamParamFile, &hv_StartCamPar);
	}
	catch (...)
	{
		AfxMessageBox("Failed to read init camera parameter file ");
		return;
	}

	if(m_ImageList.GetCount() <= 0)
	{
		AfxMessageBox("No images. Please select calibration image first.");
		return;
	}

	CString imageFile;
	m_ImageList.GetText(0, imageFile);

	if (HDevWindowStack::IsOpen())
	{
		try
		{
			ReadImage(&ho_Image, (char*)LPCTSTR(imageFile));
		}
		catch(...)
		{
			msg.Format(_T("Failed to load image ""%s"""), imageFile); 
			AfxMessageBox(msg);
			return;
		}

		HTuple hv_width, hv_height;
		HalconCpp::GetImageSize(ho_Image, &hv_width, &hv_height);
		HalconCpp::SetPart(HDevWindowStack::GetActive(), 0, 0, hv_height -1,  hv_width - 1);
	}
	

	CreateCalibData("calibration_object", 1, 1, &hv_CalibDataID);

	SetCalibDataCamParam(hv_CalibDataID, 0, "area_scan_division", hv_StartCamPar);

	SetCalibDataCalibObject(hv_CalibDataID, 0, hv_CalibModel);

	int imageCount = m_ImageList.GetCount();
	{
		for (int i = 1; i < imageCount; i++)
		{
			m_ImageList.GetText(i, imageFile);
			try
			{
				ReadImage(&ho_Image, (char*)LPCTSTR(imageFile));
			}
			catch(...)
			{
				msg.Format(_T("Failed to load image ""%s"""), imageFile); 
				AfxMessageBox(msg);
				continue;
			}

			if (HDevWindowStack::IsOpen())
			{
				HalconCpp::DispObj(ho_Image, HDevWindowStack::GetActive());
			}

			try
			{
				FindCalibObject(ho_Image, hv_CalibDataID, 0, 0, i, HTuple(), HTuple());
			}
			catch(...)
			{
				msg.Format(_T("Can not find calibraion object in image ""%s"""), imageFile); 
				AfxMessageBox(msg);
				continue;
			}

			GetCalibDataObservContours(&ho_Caltab, hv_CalibDataID, "caltab", 0, 0, i);

			if (HDevWindowStack::IsOpen())
			{
				HalconCpp::SetColor(HDevWindowStack::GetActive(),"green");
				HalconCpp::DispObj(ho_Caltab, HDevWindowStack::GetActive());
			}
		}
	}

	try
	{
		CalibrateCameras(hv_CalibDataID, &hv_Error);
		GetCalibData(hv_CalibDataID, "camera", 0, "params", &hv_CamParam);
	}
	catch(...)
	{

		return ;
	}

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

	HObject ho_Cross;

	// Local control variables
	HTuple  hv_ImgPath, hv_WindowHandle, hv_CamParam;
	HTuple  hv_Exception, hv_CaltabName, hv_CalibDataID, hv_RCoord;
	HTuple  hv_CCoord, hv_Index, hv_PoseForCalibrationPlate;
	HTuple  hv_finalPose,  hv_Button;

	HTuple  hv_X1, hv_Y1, hv_Distance;
    HTuple  hv_MeanDistance, hv_DeviationDistance;


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

	GenCrossContourXld(&ho_Cross, hv_RCoord, hv_CCoord, 6, 0.785398);

	if (HDevWindowStack::IsOpen())
	{
		SetColor(HDevWindowStack::GetActive(),"green");
	}
	if (HDevWindowStack::IsOpen())
	{
		DispObj(ho_Caltab, HDevWindowStack::GetActive());
	}
	if (HDevWindowStack::IsOpen())
	{
		SetColor(HDevWindowStack::GetActive(),"pink");
	}
	if (HDevWindowStack::IsOpen())
	{
		DispCaltab(HDevWindowStack::GetActive(), hv_CaltabName, hv_CamParam, hv_PoseForCalibrationPlate, 1);
	}

	// Cross
	if (HDevWindowStack::IsOpen())
	{
		SetColor(HDevWindowStack::GetActive(),"red");
		DispObj(ho_Cross, HDevWindowStack::GetActive());
	}

	// Write pose to file
	HTuple hv_CamPoseFile = (char*)LPCTSTR(m_CamPoseFile);
	WritePose(hv_PoseForCalibrationPlate, hv_CamPoseFile);



	ClearCalibData(hv_CalibDataID);
}

void CCamCalibDlg::OnBnClickedCalAccuracyBtn()
{
	UpdateData(true);

	// Local iconic variables
	HObject  ho_Image, ho_Caltab;

	HObject ho_Cross;

	// Local control variables
	HTuple  hv_ImgPath, hv_WindowHandle, hv_CamParam;
	HTuple  hv_Exception, hv_CaltabName, hv_CalibDataID, hv_RCoord;
	HTuple  hv_CCoord, hv_Index, hv_PoseForCalibrationPlate;
	HTuple  hv_finalPose,  hv_Button;

	HTuple  hv_X1, hv_Y1, hv_Distance;
    HTuple  hv_MeanDistance, hv_DeviationDistance;


	HTuple ho_PoseFilePath;

	//Read Image
	hv_ImgPath = (char*)LPCTSTR(m_AccuracyImage);
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

	//Read Camera Pose
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


	//calibration plate is positioned directly on the measurement plane
	hv_CaltabName = (char*)LPCTSTR(m_CalibModel);

	CreateCalibData("calibration_object", 1, 1, &hv_CalibDataID);

	SetCalibDataCamParam(hv_CalibDataID, 0, "area_scan_division", hv_CamParam);

	SetCalibDataCalibObject(hv_CalibDataID, 0, hv_CaltabName);

	FindCalibObject(ho_Image, hv_CalibDataID, 0, 0, 1, HTuple(), HTuple());

	GetCalibDataObservContours(&ho_Caltab, hv_CalibDataID, "caltab", 0, 0, 1);

	GetCalibDataObservPoints(hv_CalibDataID, 0, 0, 1, &hv_RCoord, &hv_CCoord, &hv_Index, &hv_PoseForCalibrationPlate);

	GenCrossContourXld(&ho_Cross, hv_RCoord, hv_CCoord, 6, 0.785398);

	// Cross
	if (HDevWindowStack::IsOpen())
	{
		SetColor(HDevWindowStack::GetActive(),"red");
		DispObj(ho_Cross, HDevWindowStack::GetActive());
	}
	
	// Cal Accuracy
	ImagePointsToWorldPlane(hv_CamParam, hv_PoseForCalibrationPlate, hv_RCoord, hv_CCoord, "mm", &hv_X1, &hv_Y1);

	hv_Distance = HTuple();
	for(int r = 0; r < m_ModelRow; r++)
	{
		HTuple distance;

		int i = r * m_ModelColumn;

		int j = i + m_ModelColumn - 2;

		 DistancePp(hv_X1.TupleSelectRange( i, j), hv_Y1.TupleSelectRange(i, j), 
			 hv_X1.TupleSelectRange(i + 1, j + 1), hv_Y1.TupleSelectRange(i + 1, j + 1), &distance);


		 hv_Distance.Append(distance);
	}
   
	TupleMean(hv_Distance, &hv_MeanDistance);   

	TupleDeviation(hv_Distance, &hv_DeviationDistance);

	CString msg;

	msg.Format(_T("Mean distance: %.3f mm +/- %.3f mm"),  (float)hv_MeanDistance, (float)hv_DeviationDistance);

	MessageBox(msg);

	ClearCalibData(hv_CalibDataID);
	
}

void CCamCalibDlg::OnBnClickedSelectCalibImageBtn()
{
	CFileDialog openDlg(true, NULL, NULL, OFN_ALLOWMULTISELECT);
	const int nMaxFiles = 50;
	const int nMaxPathBuffer = (nMaxFiles * (MAX_PATH + 1)) + 1;  
	LPSTR pc = (LPSTR)malloc(nMaxPathBuffer * sizeof(char));
	if(pc) 
	{
		openDlg.GetOFN().lpstrFile = pc; 
		openDlg.GetOFN().lpstrFile[0] = NULL; 
		openDlg.GetOFN().nMaxFile = nMaxPathBuffer; 
		if( openDlg.DoModal() == IDOK )  
		{  
			m_ImageList.ResetContent();
			/*while(m_ImageList.GetCount() > 0)
			{
				m_ImageList.DeleteString( 0 ); 
			}*/
			POSITION posStart = openDlg.GetStartPosition();  
			while( posStart )  
			{  
				CString fileName = openDlg.GetNextPathName(posStart);  
				m_ImageList.AddString(fileName);
			}
		} 
		free(pc); 
	}
}


void CCamCalibDlg::OnBnClickedSelectCalibModelBtn()
{
	CFileDialog openDlg(true, NULL, NULL);
	if(openDlg.DoModal() == IDOK)
	{
		m_CalibModel = openDlg.GetPathName();
		UpdateData(false);
	}
}


void CCamCalibDlg::OnBnClickedSelectCamParamBtn()
{
	CFileDialog openDlg(true, NULL, NULL);
	if(openDlg.DoModal() == IDOK)
	{
		m_CamParamFile = openDlg.GetPathName();
		UpdateData(false);
	}
}


void CCamCalibDlg::OnBnClickedSelectCamPoseImageBtn()
{
	CFileDialog openDlg(true, NULL, NULL);
	if(openDlg.DoModal() == IDOK)
	{
		m_CamPoseImage = openDlg.GetPathName();
		UpdateData(false);
	}
}


void CCamCalibDlg::OnBnClickedSelectCamPoseFileBtn()
{
	CFileDialog openDlg(true, NULL, NULL);
	if(openDlg.DoModal() == IDOK)
	{
		m_CamPoseFile = openDlg.GetPathName();
		UpdateData(false);		
	}
}


void CCamCalibDlg::OnBnClickedSelectCalAccuracyImageBtn()
{
		CFileDialog openDlg(true, NULL, NULL);
	if(openDlg.DoModal() == IDOK)
	{
		m_AccuracyImage = openDlg.GetPathName();
		UpdateData(false);
	}
}


void CCamCalibDlg::OnBnClickedSelectInitCamParamBtn()
{
	// TODO: Add your control notification handler code here
}
