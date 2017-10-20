// delta-barrierDlg.cpp : implementation file
//

#include "stdafx.h"
#include "delta-barrier.h"
#include "delta-barrierDlg.h"
#include "afxdialogex.h"

#include <vector>

#include <util/common/math/common.h>
#include <util/common/math/dsolve.h>

#include "model.h"

using namespace plot;
using namespace util;
using namespace math;
using namespace model;

using points_t = std::vector < point < double > > ;
using plot_t = simple_list_plot < points_t > ;

const size_t n_points = 1000;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_INVOKE WM_USER + 1234

plot_t barrier_plot, wavefunc_plot, transmission_plot;

// CDeltaBarrierDlg dialog

UINT SimulationThreadProc(LPVOID pParam)
{
    CDeltaBarrierDlg & dlg = * (CDeltaBarrierDlg *) pParam;
    while (dlg.m_bWorking)
    {
        // todo
        Sleep(16);
    }
    return 0;
}

CDeltaBarrierDlg::CDeltaBarrierDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDeltaBarrierDlg::IDD, pParent)
    , m_pWorkerThread(NULL)
    , m_bWorking(FALSE)
    , m_dL(1)
    , m_dV0(1)
    , m_nN(1)
    , m_dS0(0.01)
    , m_dE1(0)
    , m_dE2(0)
    , m_dX(0)
    , m_dE(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDeltaBarrierDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PLOT1, m_cBarrier);
    DDX_Control(pDX, IDC_PLOT2, m_cWaveFunc);
    DDX_Control(pDX, IDC_PLOT3, m_cTransmission);
    DDX_Text(pDX, IDC_EDIT1, m_dL);
    DDX_Text(pDX, IDC_EDIT2, m_dV0);
    DDX_Text(pDX, IDC_EDIT3, m_nN);
    DDX_Text(pDX, IDC_EDIT4, m_dS0);
    DDX_Text(pDX, IDC_EDIT5, m_dE1);
    DDX_Text(pDX, IDC_EDIT6, m_dE2);
    DDX_Text(pDX, IDC_EDIT7, m_dX);
    DDX_Text(pDX, IDC_EDIT8, m_dE);
}

BEGIN_MESSAGE_MAP(CDeltaBarrierDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_MESSAGE(WM_INVOKE, &CDeltaBarrierDlg::OnInvoke)
    ON_BN_CLICKED(IDC_BUTTON1, &CDeltaBarrierDlg::OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BUTTON2, &CDeltaBarrierDlg::OnBnClickedButton2)
    ON_BN_CLICKED(IDC_BUTTON3, &CDeltaBarrierDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CDeltaBarrierDlg message handlers

BOOL CDeltaBarrierDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

    auto_viewport_params params;
    params.factors = { 0, 0, 0.1, 0.1 };
    auto barrier_avp      = min_max_auto_viewport < points_t > ::create();
    auto wavefunc_avp     = min_max_auto_viewport < points_t > ::create();
    auto transmission_avp = min_max_auto_viewport < points_t > ::create();
    barrier_avp->set_params(params);
    wavefunc_avp->set_params(params);
    transmission_avp->set_params(params);

    barrier_plot
        .with_view()
        .with_view_line_pen(plot::palette::pen(RGB(255, 255, 255), 1))
        .with_data()
        .with_auto_viewport(barrier_avp);
    wavefunc_plot
        .with_view()
        .with_view_line_pen(plot::palette::pen(RGB(255, 255, 255), 1))
        .with_data()
        .with_auto_viewport(wavefunc_avp);
    transmission_plot
        .with_view()
        .with_view_line_pen(plot::palette::pen(RGB(255, 255, 255), 1))
        .with_data()
        .with_auto_viewport(transmission_avp);

    m_cBarrier.background = palette::brush();
    m_cWaveFunc.background = palette::brush();
    m_cTransmission.background = palette::brush();

    m_cTransmission.triple_buffered = true;

    m_cBarrier.plot_layer.with(
        viewporter::create(
            tick_drawable::create(
                barrier_plot.view,
                const_n_tick_factory<axe::x>::create(
                    make_simple_tick_formatter(2, 5),
                    0,
                    5
                ),
                const_n_tick_factory<axe::y>::create(
                    make_simple_tick_formatter(2, 5),
                    0,
                    5
                ),
                palette::pen(RGB(80, 80, 80)),
                RGB(200, 200, 200)
            ),
            make_viewport_mapper(barrier_plot.viewport_mapper)
        )
    );

    m_cWaveFunc.plot_layer.with(
        viewporter::create(
            tick_drawable::create(
                wavefunc_plot.view,
                const_n_tick_factory<axe::x>::create(
                    make_simple_tick_formatter(1),
                    0,
                    10
                ),
                const_n_tick_factory<axe::y>::create(
                    make_simple_tick_formatter(3),
                    0,
                    5
                ),
                palette::pen(RGB(80, 80, 80)),
                RGB(200, 200, 200)
            ),
            make_viewport_mapper(wavefunc_plot.viewport_mapper)
        )
    );

    m_cTransmission.plot_layer.with(
        viewporter::create(
            tick_drawable::create(
                transmission_plot.view,
                const_n_tick_factory<axe::x>::create(
                    make_simple_tick_formatter(1),
                    0,
                    10
                ),
                const_n_tick_factory<axe::y>::create(
                    make_simple_tick_formatter(3),
                    0,
                    5
                ),
                palette::pen(RGB(80, 80, 80)),
                RGB(200, 200, 200)
            ),
            make_viewport_mapper(transmission_plot.viewport_mapper)
        )
    );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDeltaBarrierDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDeltaBarrierDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDeltaBarrierDlg::StartSimulationThread()
{
    if (this->m_bWorking)
    {
        return;
    }
    this->m_bWorking = TRUE;
    this->m_pWorkerThread = AfxBeginThread(&SimulationThreadProc, this, 0, 0, CREATE_SUSPENDED);
    this->m_pWorkerThread->m_bAutoDelete = FALSE;
    ResumeThread(this->m_pWorkerThread->m_hThread);
}


void CDeltaBarrierDlg::StopSimulationThread()
{
    if (this->m_bWorking)
    {
        this->m_bWorking = FALSE;
        while (MsgWaitForMultipleObjects(
            1, &this->m_pWorkerThread->m_hThread, FALSE, INFINITE, QS_SENDMESSAGE) != WAIT_OBJECT_0)
        {
            MSG msg;
            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        //this->m_pWorkerThread->Delete();
        delete this->m_pWorkerThread;
        this->m_pWorkerThread = NULL;
    }
}


void CDeltaBarrierDlg::Invoke(const std::function < void () > & fn)
{
    SendMessage(WM_INVOKE, 0, (LPARAM)&fn);
}


afx_msg LRESULT CDeltaBarrierDlg::OnInvoke(WPARAM wParam, LPARAM lParam)
{
    (*(const std::function < void () > *) lParam)();
    return 0;
}

BOOL CDeltaBarrierDlg::DestroyWindow()
{
    StopSimulationThread();

    return CDialogEx::DestroyWindow();
}


void CDeltaBarrierDlg::OnBnClickedButton1()
{
    UpdateData(TRUE);

    StartSimulationThread();
}


void CDeltaBarrierDlg::OnBnClickedButton2()
{
    StopSimulationThread();
}


void CDeltaBarrierDlg::OnBnClickedButton3()
{
    UpdateData(TRUE);

    barrier_plot.data->resize(n_points);
    wavefunc_plot.data->resize(n_points);

    double a = (m_nN == 1) ? m_dL : (m_dL / (m_nN - 1));
    double width = m_dL + ((m_nN == 1) ? 0 : a);
    double s = m_dS0 * a;

    continuous_t barrier = make_barrier_fn(m_nN, m_dV0, a, s);

    for (size_t i = 0; i < n_points; ++i)
    {
        double x = - a / 2 + (double) i / n_points * width;
        barrier_plot.data->at(i) = { x, barrier(x) };
    }

    barrier_plot.refresh();

    m_cBarrier.RedrawWindow();

    double k      = std::sqrt(m_dE);
    double period = 2 * M_PI / k;

    dfunc3_t < cv3 > alpha_beta = make_sweep_method_dfunc(barrier, m_dE);

    /* use 3-sigma rule to maximally reduce the interval
       is it applicable in our case? */
    double left_x  = - 3 * s;
    double right_x = (m_nN == 1) ? 3 * s : (m_dL + 3 * s);

    /* barrier has a number of wide gaps
       however we use constant step even if
       it is not efficient just for simplicity */
    dresult3 < cv3 > ab_ = rk4_solve3i < cv3 >
    (
        alpha_beta,
        left_x,
        right_x,
        s / 100,
        { _im(-k), _im(2 * k) }
    );

    cv3 u  = ab_.x.at<1>() / (_im(k) - ab_.x.at<0>());
    cv3 du = _im(k) * ab_.x.at<1>() / (_im(k) - ab_.x.at<0>());

    dfunc3s_t < cv3 > wavefunc_dfunc = make_schrodinger_dfunc(barrier, m_dE);

    dresult3s < cv3 > wavefunc = { right_x, u, du };

    double step = (double) 1. / (n_points / 2) * (period * 5);

    for (size_t i = 0; i < n_points / 2; ++i)
    {
        double x = wavefunc.t;
        wavefunc = rk4_solve3s < cv3 > (wavefunc_dfunc, x, -step, wavefunc.x, wavefunc.dx);
        wavefunc_plot.data->at(n_points / 2 - 1 - i) = { x, wavefunc.x.at<0>().re };
    }

    wavefunc = { right_x, u, du };

    for (size_t i = 0; i < n_points / 2 + 1; ++i)
    {
        double x = wavefunc.t;
        wavefunc = rk4_solve3s < cv3 > (wavefunc_dfunc, x, step, wavefunc.x, wavefunc.dx);
        wavefunc_plot.data->at(n_points / 2 - 1 + i) = { x, wavefunc.x.at<0>().re };
    }

    wavefunc_plot.refresh();

    m_cWaveFunc.RedrawWindow();
}
