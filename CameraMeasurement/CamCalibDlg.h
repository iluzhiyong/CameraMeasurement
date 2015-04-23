#pragma once
#include "afxwin.h"
#include "AutoHListBox.h"

// CCamCalibDlg dialog

class CCamCalibDlg : public CDialog
{
	DECLARE_DYNAMIC(CCamCalibDlg)

public:
	CCamCalibDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCamCalibDlg();

// Dialog Data
	enum { IDD = IDD_CAM_CALIB_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CString m_CalibModel;
	CString m_ImageDir;
	afx_msg void OnBnClickedCalcCamParamBtn();
	CString m_ImageName;
	int m_ImageNums;
	CString m_CamParamFile;
	CString m_CamPoseFile;
	afx_msg void OnBnClickedButton();
	CString m_CamPoseImage;
	afx_msg void OnBnClickedCalCamPoseBtn();

public:
	CString GetExePath();	
	void InitSet();

	void CalcCamParam();
	void CalCamPose();

	CString m_Msg;
	BOOL m_CalcParam;
	CString m_AccuracyImage;
	afx_msg void OnBnClickedCalAccuracyBtn();
	int m_ModelRow;
	int m_ModelColumn;
	CAutoHListBox m_ImageList;
	afx_msg void OnBnClickedSelectCalibImageBtn();
	afx_msg void OnBnClickedSelectCalibModelBtn();
	afx_msg void OnBnClickedSelectCamParamBtn();
	afx_msg void OnBnClickedSelectCamPoseImageBtn();
	afx_msg void OnBnClickedSelectCamPoseFileBtn();
	afx_msg void OnBnClickedSelectCalAccuracyImageBtn();
	CString m_InitCamParam;
	afx_msg void OnBnClickedSelectInitCamParamBtn();
};
