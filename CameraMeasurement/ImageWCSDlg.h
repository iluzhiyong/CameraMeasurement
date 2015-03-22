#pragma once


// CImageWCSDlg dialog

class CImageWCSDlg : public CDialog
{
	DECLARE_DYNAMIC(CImageWCSDlg)

public:
	CImageWCSDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CImageWCSDlg();

// Dialog Data
	enum { IDD = IDD_IMAGE_WCS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CString m_CamParamFile;
	CString m_CamPoseFile;
	int m_ImgRow;
	int m_ImgColumn;
	float m_WCSX;
	float m_WCSY;
	afx_msg void OnBnClickedCalBtn();
	void ExcuteCalculate();
	CString m_Msg;
	CString m_Message;
	
public:
	CString GetExePath();
	void InitSet();
};
