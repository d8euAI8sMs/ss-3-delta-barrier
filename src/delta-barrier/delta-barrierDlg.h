// delta-barrierDlg.h : header file
//

#pragma once

#include <functional>

#include <util/common/plot/PlotStatic.h>

// CDeltaBarrierDlg dialog
class CDeltaBarrierDlg : public CDialogEx
{
// Construction
public:
	CDeltaBarrierDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_DELTABARRIER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
    CWinThread * m_pWorkerThread;
    volatile BOOL m_bWorking;
    void Invoke(const std::function < void () > & fn);
    void StartSimulationThread();
    void StopSimulationThread();

public:

    afx_msg LRESULT OnInvoke(WPARAM wParam, LPARAM lParam);

    friend UINT SimulationThreadProc(LPVOID pParam);

protected:

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL DestroyWindow();
    afx_msg void OnBnClickedButton1();
    afx_msg void OnBnClickedButton2();
    PlotStatic m_cBarrier;
    PlotStatic m_cWaveFunc;
    PlotStatic m_cTransmission;
};
