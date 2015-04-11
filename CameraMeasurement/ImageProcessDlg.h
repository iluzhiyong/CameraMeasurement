#pragma once
#include "HalconHeader.h"

// CImageProcessDlg dialog
class CCameraMeasurementDlg;
class CImageProcessDlg : public CDialog
{
	DECLARE_DYNAMIC(CImageProcessDlg)

public:
	CImageProcessDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CImageProcessDlg();

// Dialog Data
	enum { IDD = IDD_PROCESS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_CamParamFile;
	CString m_CamPoseFile;

public:
	CString GetExePath();
	void InitSet();
	afx_msg void OnBnClickedImgCneterBtn();
	afx_msg void OnBnClickedCircleCenterBtn();
	float m_ImgRow;
	float m_ImgCol;
	float m_ImgCX;
	float m_ImgCY;
	float m_CRow;
	float m_CCol;
	float m_CCX;
	float m_CCY;

public:
	CCameraMeasurementDlg* m_CamMeasureDlg;

	HTuple hv_CamParam;
	HTuple hv_CamPose;

public:
	void LoadCamParamPoseFile();
};
