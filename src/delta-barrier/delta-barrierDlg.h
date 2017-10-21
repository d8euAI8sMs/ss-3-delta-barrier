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
    template < typename _container_t >
    void SetupPlot(PlotStatic & targetPlot, plot::simple_list_plot < _container_t > & layer);
    void DrawBarrier();

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
    PlotStatic m_cWaveFuncRe;
    PlotStatic m_cWaveFuncIm;
    PlotStatic m_cTransmission;
    double m_dL;
    double m_dV0;
    int m_nN;
    double m_dS0;
    double m_dE1;
    double m_dE2;
    double m_dE;
    afx_msg void OnBnClickedButton3();
};
