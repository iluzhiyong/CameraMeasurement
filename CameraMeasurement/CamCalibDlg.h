#pragma once


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
};