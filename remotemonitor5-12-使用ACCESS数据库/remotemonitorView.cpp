// remotemonitorView.cpp : implementation of the CRemotemonitorView class
//

#include "stdafx.h"
#include "remotemonitor.h"
#include "MainFrm.h"
#include "comm32.h"
#include "remotemonitorDoc.h"
#include "remotemonitorView.h"
#include "MMSYSTEM.H"
#include "define.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

COMMINFO Comminfo;
COMMINFO* pCommInfo;
extern int nPort,nBaudrate;
extern unsigned char  InDataBuf[];		// ���ڻ�����
extern int InDataLen;			        // ���ڻ�������ʵ�����ݳ���
extern  bool bPoll;			            // TRUE: ��ʱ�жϻ򴮿��ж���ʹ��DataBuf��nDataBuf.

extern CString strCurrentUser;
extern CString strDutyTime;

/////////////////////////////////////////////////////////////////////////////
// CRemotemonitorView
int   m_nScale=100;
extern BOOL bRight[];	// ����Ȩ��
IMPLEMENT_DYNCREATE(CRemotemonitorView, CScrollView)

BEGIN_MESSAGE_MAP(CRemotemonitorView, CScrollView)
	//{{AFX_MSG_MAP(CRemotemonitorView)
	ON_COMMAND(ID_ZOOMIN, OnZoomin)
	ON_COMMAND(ID_ZOOMOUT, OnZoomout)
	ON_COMMAND(ID_PAN, OnPan)
	ON_UPDATE_COMMAND_UI(ID_ZOOMIN, OnUpdateZoomin)
	ON_UPDATE_COMMAND_UI(ID_ZOOMOUT, OnUpdateZoomout)
	ON_UPDATE_COMMAND_UI(ID_PAN, OnUpdatePan)
	ON_COMMAND(ID_ARROW, OnArrow)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_MAPCHANGE, OnMapchange)
	ON_COMMAND(ID_DRAWLINE, OnDrawline)
	ON_COMMAND(ID_DRAWCIRCLE, OnDrawcircle)
	ON_UPDATE_COMMAND_UI(ID_DRAWLINE, OnUpdateDrawline)
	ON_UPDATE_COMMAND_UI(ID_DRAWCIRCLE, OnUpdateDrawcircle)
	ON_COMMAND(ID_SELECT, OnSelect)
	ON_UPDATE_COMMAND_UI(ID_SELECT, OnUpdateSelect)
	ON_COMMAND(ID_DELETE, OnDelete)
	ON_UPDATE_COMMAND_UI(ID_DELETE, OnUpdateDelete)
	ON_WM_TIMER()
	ON_COMMAND(ID_SENDAA, OnSendaa)
	ON_COMMAND(ID_SENDAB, OnSendab)
	ON_COMMAND(ID_SENDCA, OnSendca)
	ON_COMMAND(ID_SENDCC, OnSendcc)
	ON_COMMAND(ID_SENDCD, OnSendcd)
	ON_COMMAND(ID_SENDDA, OnSendda)
	ON_COMMAND(ID_DELETELISTITEM, OnDeletelistitem)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_DESTROY()
	ON_COMMAND(ID_CLEARALARM, OnClearalarm)
	ON_COMMAND(ID_DUTYRECORD, OnDutyrecord)
	ON_COMMAND(ID_JIAOBAN, OnJiaoban)
	ON_COMMAND(ID_FENJICABLESET, OnFenjicableset)
	ON_COMMAND(ID_USERSET, OnUserset)
	ON_COMMAND(ID_VIEWDUTYRECORD, OnViewdutyrecord)
	ON_COMMAND(ID_VIEWFENJICABLEINFO, OnViewfenjicableinfo)
	ON_COMMAND(ID_VIEWCABLEALARMINFO, OnViewcablealarminfo)
	ON_UPDATE_COMMAND_UI(ID_USERSET, OnUpdateUserset)
	ON_UPDATE_COMMAND_UI(ID_FENJICABLESET, OnUpdateFenjicableset)
	ON_COMMAND(ID_SENDDC, OnSenddc)
	ON_COMMAND(ID_SENDCB, OnSendcb)
	ON_COMMAND(ID_SETCENTERTEL, OnSetcentertel)
	ON_UPDATE_COMMAND_UI(ID_CLEARALARM, OnUpdateClearalarm)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRemotemonitorView construction/destruction

CRemotemonitorView::CRemotemonitorView()
{
	// TODO: add construction code here
	m_strMapPathName="";
    m_hMemoryDC=NULL;
    m_hBitmap=NULL;
    m_hOldBitmap=NULL;
	m_bMouseDown=FALSE;
    m_pPenProperties=NULL;
    m_nCurTool=ID_ARROW;
	m_usrCurrentObject = NULL;
	m_nCurrentPenColor = RGB(255,0,0);
    m_nCurrentPenWidth =1;
	m_nCurrentFillColor= RGB(255,255,0);
	m_nTextColor=RGB(0,0,255);

    m_uRevTimer=0;
	m_uSendTimer=0;
	m_uFlashTimer=0;
    m_uProbeTimer=0;
	m_uDisplayTimer=0;
	m_uReSendTimer=0;
	m_bAlarm=false;
    m_nCounter=0;
    m_bBusy=FALSE;
	m_nLinkState=IDLE;
    m_bHasAck=FALSE;
	CreateCommInfo(&Comminfo,nPort,/*nBaudrate*/9600);
	OpenConnection(&Comminfo,(LPTHREAD_START_ROUTINE)CommWatchProc);


}
void CRemotemonitorView::ChangePenColor(COLORREF nColor)
{
	m_nCurrentPenColor = nColor;
	if(m_usrCurrentObject && m_usrCurrentObject->m_bSelected)
	{//���µ�ǰѡ��ͼԪ����ʾ
		m_usrCurrentObject->SetPenColor(m_nCurrentPenColor);
		// ��������
	    CRemotemonitorDoc* pDoc = GetDocument();
		pDoc->SetModifiedFlag(TRUE);
		Invalidate();
	}
}

void CRemotemonitorView::ChangeFillColor(COLORREF nColor)
{
	m_nCurrentFillColor = nColor;
	if(m_usrCurrentObject && m_usrCurrentObject->m_bSelected)
	{//���µ�ǰѡ��ͼԪ����ʾ
		m_usrCurrentObject->SetFillColor(m_nCurrentFillColor);
	    CRemotemonitorDoc* pDoc = GetDocument();
		pDoc->SetModifiedFlag(TRUE);
		Invalidate();
	}
}
void CRemotemonitorView::ChangeTextColor(COLORREF nColor)
{
	m_nTextColor = nColor;
	if(m_usrCurrentObject && m_usrCurrentObject->m_bSelected)
	{//���µ�ǰѡ��ͼԪ����ʾ
		m_usrCurrentObject->SetTextColor(m_nTextColor);
	    CRemotemonitorDoc* pDoc = GetDocument();
		pDoc->SetModifiedFlag(TRUE);

		Invalidate();
	}
}

void CRemotemonitorView::ChangePenWidth(int nWidth)
{
	m_nCurrentPenWidth = nWidth;
	if(m_usrCurrentObject && m_usrCurrentObject->m_bSelected)
	{//���µ�ǰѡ��ͼԪ����ʾ
		m_usrCurrentObject->SetPenWidth(m_nCurrentPenWidth);
	    CRemotemonitorDoc* pDoc = GetDocument();
		pDoc->SetModifiedFlag(TRUE);

		Invalidate();
	}
}

CRemotemonitorView::~CRemotemonitorView()
{
	CloseConnection(&Comminfo); // �ر�ͨ�ſ�1
	DestroyCommInfo(&Comminfo); // ɾ��ͨ�ſ��ƿ�

	if(m_hOldBitmap)
		SelectObject(m_hMemoryDC,m_hOldBitmap);
	if(m_hBitmap)
		DeleteObject(m_hBitmap);
	if(m_hMemoryDC)
		DeleteDC(m_hMemoryDC);

	if(m_cabletable.IsOpen())
        m_cabletable.Close();
	if(m_fenjitable.IsOpen())
        m_fenjitable.Close();
	if(m_cablealarmtable.IsOpen())
        m_cablealarmtable.Close();

}

BOOL CRemotemonitorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CRemotemonitorView drawing

void CRemotemonitorView::OnDraw(CDC* pDC)
{
	CRemotemonitorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
    //��λͼ
	if(m_hMemoryDC!=NULL)
	{

     //   BitBlt(pDC->m_hDC,0,0,m_bmInfo.bmWidth,m_bmInfo.bmHeight,m_hMemoryDC,0,0,SRCCOPY);
		::SetStretchBltMode(pDC->m_hDC,COLORONCOLOR);
        ::StretchBlt(pDC->m_hDC,0,0,(int)(m_bmInfo.bmWidth*((float)m_nScale/100.0)),(int)(m_bmInfo.bmHeight*((float)m_nScale/100.0)),m_hMemoryDC,0,0,m_bmInfo.bmWidth,m_bmInfo.bmHeight,SRCCOPY);
	}
	//�����ػ���ͼ��
	if(pDoc->m_aObjects.GetSize()>0)
	{
	     pDoc->DrawObjects(pDC);
	}

}

void CRemotemonitorView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	if(m_hMemoryDC==NULL)
	{
	    sizeTotal.cx=sizeTotal.cy = 100;
	    SetScrollSizes(MM_TEXT, sizeTotal);
	}
	else
	{
	    sizeTotal.cx=(long)(m_bmInfo.bmWidth*((float)m_nScale/100.0));
		sizeTotal.cy =(long)( m_bmInfo.bmHeight*((float)m_nScale/100.0));
	}
	SetScrollSizes(MM_TEXT, sizeTotal);
    m_usrCurrentObject=NULL;
	// TODO: calculate the total size of this view

//	CloseConnection(&Comminfo); // �ر�ͨ�ſ�1
//	DestroyCommInfo(&Comminfo); // ɾ��ͨ�ſ��ƿ�

//	CreateCommInfo(&Comminfo,nPort,nBaudrate);
//	OpenConnection(&Comminfo,(LPTHREAD_START_ROUTINE)CommWatchProc);

	m_pkstart.syn=0x16;
	m_pkstart.dle=0x10;
	m_pkstart.stx=0x02;
	m_pkend.dle=0x10;
	m_pkend.etx=0x03;

	if(m_uFlashTimer)
		KillTimer(m_uFlashTimer);
	if(m_uRevTimer)
		KillTimer(m_uRevTimer);
	if(m_uSendTimer)
		KillTimer(m_uSendTimer);
    if(m_uProbeTimer)
		KillTimer(m_uProbeTimer);
    if(m_uDisplayTimer)
		KillTimer(m_uDisplayTimer);
    if(m_uReSendTimer)
		KillTimer(m_uReSendTimer);

	if(!m_cabletable.IsOpen())
        m_cabletable.Open();
	if(!m_fenjitable.IsOpen())
        m_fenjitable.Open();
	if(!m_cablealarmtable.IsOpen())
        m_cablealarmtable.Open();

	m_uFlashTimer=SetTimer(FLASHTIMER,m_FlashT,NULL);
	m_uRevTimer=SetTimer(REVTIMER,m_RevT,NULL);
	m_uSendTimer=SetTimer(SNDTIMER,m_SendT,NULL);
	m_uProbeTimer=SetTimer(PROBETIMER,m_ProbeT,NULL);
	m_uDisplayTimer=SetTimer(DISPLAYTIME,m_DisplayT,NULL);
    SendLR();
    m_bHasAck=FALSE;
}
void CRemotemonitorView::initMemoryDC()
{
   HDC hDC;
   hDC=::GetDC(*this);
   m_hMemoryDC=CreateCompatibleDC(hDC);

   m_hBitmap=(HBITMAP)LoadImage(NULL,m_strMapPathName,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
 //   m_bitmap.Attach(hbitmap);
   m_hOldBitmap=(HBITMAP)SelectObject(m_hMemoryDC,m_hBitmap);
   ::ReleaseDC(*this,hDC);

//	m_pOldBitmap=m_memoryDC.SelectObject(&m_bitmap);
 //   m_bitmap.GetObject(sizeof(BITMAP),sizeof(BITMAP));

   
   ::GetObject(m_hBitmap,sizeof(BITMAP),&m_bmInfo);
	CSize sizeTotal;

	sizeTotal.cx=m_bmInfo.bmWidth;
	sizeTotal.cy = m_bmInfo.bmHeight;
	SetScrollSizes(MM_TEXT, sizeTotal);
}
/////////////////////////////////////////////////////////////////////////////
// CRemotemonitorView printing

BOOL CRemotemonitorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CRemotemonitorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CRemotemonitorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CRemotemonitorView diagnostics

#ifdef _DEBUG
void CRemotemonitorView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CRemotemonitorView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CRemotemonitorDoc* CRemotemonitorView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRemotemonitorDoc)));
	return (CRemotemonitorDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRemotemonitorView message handlers

/*
void CRemotemonitorView::ReadCfgFromIni()
{
	char path_buffer[_MAX_PATH];
	char path_ini[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	CString strIniName;

	GetModuleFileName( NULL, path_buffer, _MAX_PATH );
	_splitpath( path_buffer, drive, dir, fname, ext );	
	_makepath(  path_ini, drive, dir, fname, "ini" );
    strIniName = path_ini;
	int nMapCur = GetPrivateProfileInt("MAP", "MAPCUR", 0, strIniName);
    m_nCurMap = nMapCur;
	CString strMapPathName,strEntry;
	strEntry.Format(TEXT("MAP%d"),nMapCur);
	GetPrivateProfileString("MAP", strEntry,"", strMapPathName.GetBuffer(80), 80,strIniName);

	strMapPathName.ReleaseBuffer();
	strMapPathName.TrimLeft();
	strMapPathName.TrimRight();
	m_strMapPathName = strMapPathName;
}
void CRemotemonitorView::WriteCfgToIni()
{
	char path_buffer[_MAX_PATH];
	char path_ini[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	CString strIniName;

	GetModuleFileName( NULL, path_buffer, _MAX_PATH );
	_splitpath( path_buffer, drive, dir, fname, ext );	
	_makepath(  path_ini, drive, dir, fname, "ini" );
    strIniName = path_ini;

	CString strMapCur;
    strMapCur.Format("%d",m_nCurMap);
	WritePrivateProfileString("MAP", "MAPCUR",strMapCur, strIniName); 
}

*/

void CRemotemonitorView::OnZoomin() 
{
	// TODO: Add your command handler code here
	//���õ�ǰ����Ϊ �Ŵ�  ��ͼ����
	m_nCurTool=ID_ZOOMIN;
	HCURSOR hCursor;
    hCursor=AfxGetApp()->LoadCursor(IDC_ZOOMIN);
	::SetClassLong(GetSafeHwnd(), GCL_HCURSOR, (LONG)hCursor);
	int i;
    CDC* pDC = GetDC();
	OnPrepareDC(pDC);//����DC�Ĺ������ԣ���ScollView�Ĺ����й�
	CRemotemonitorDoc* pDoc = GetDocument();

	for(i = pDoc->m_aObjects.GetSize()-1; i >= 0; i--)
	{
		m_usrCurrentObject = pDoc->m_aObjects[i];
		m_usrCurrentObject->Selected(pDC, false);
		m_usrCurrentObject = NULL;
	}
	ReleaseDC(pDC);//�ͷ�DC��Դ
}
void CRemotemonitorView::OnUpdateZoomin(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_nCurTool== ID_ZOOMIN);
	
}
void CRemotemonitorView::OnZoomout() 
{
	// TODO: Add your command handler code here
	//���õ�ǰ����Ϊ ��С ��ͼ��Զ
	m_nCurTool=ID_ZOOMOUT;
	HCURSOR hCursor;
    hCursor=AfxGetApp()->LoadCursor(IDC_ZOOMOUT);
	::SetClassLong(GetSafeHwnd(), GCL_HCURSOR, (LONG)hCursor);
	int i;
    CDC* pDC = GetDC();
	OnPrepareDC(pDC);//����DC�Ĺ������ԣ���ScollView�Ĺ����й�
	CRemotemonitorDoc* pDoc = GetDocument();

	for(i = pDoc->m_aObjects.GetSize()-1; i >= 0; i--)
	{
		m_usrCurrentObject = pDoc->m_aObjects[i];
		m_usrCurrentObject->Selected(pDC, false);
		m_usrCurrentObject = NULL;
	}
	ReleaseDC(pDC);//�ͷ�DC��Դ
}
void CRemotemonitorView::OnUpdateZoomout(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_nCurTool== ID_ZOOMOUT);
}
void CRemotemonitorView::OnPan() 
{
	// TODO: Add your command handler code here
	//���õ�ǰ����Ϊ ���� ��ͼƽ��
	m_nCurTool=ID_PAN;
	HCURSOR hCursor;
    hCursor=AfxGetApp()->LoadCursor(IDC_PAN);
	::SetClassLong(GetSafeHwnd(), GCL_HCURSOR, (LONG)hCursor);


	int i;

    CDC* pDC = GetDC();
	OnPrepareDC(pDC);//����DC�Ĺ������ԣ���ScollView�Ĺ����й�
	CRemotemonitorDoc* pDoc = GetDocument();

	for(i = pDoc->m_aObjects.GetSize()-1; i >= 0; i--)
	{
		m_usrCurrentObject = pDoc->m_aObjects[i];
		if(m_usrCurrentObject!=NULL)
		{
			m_usrCurrentObject->Selected(pDC, false);
			m_usrCurrentObject = NULL;
		}
	}
	ReleaseDC(pDC);//�ͷ�DC��Դ


}
void CRemotemonitorView::OnUpdatePan(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_nCurTool== ID_PAN);
}
void CRemotemonitorView::OnArrow() 
{
	// TODO: Add your command handler code here
	m_nCurTool=ID_ARROW;
	HCURSOR hCursor;
    hCursor=AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	::SetClassLong(GetSafeHwnd(), GCL_HCURSOR, (LONG)hCursor);

	int i;
    CDC* pDC = GetDC();
	OnPrepareDC(pDC);//����DC�Ĺ������ԣ���ScollView�Ĺ����й�
	CRemotemonitorDoc* pDoc = GetDocument();

	for(i = pDoc->m_aObjects.GetSize()-1; i >= 0; i--)
	{
		m_usrCurrentObject = pDoc->m_aObjects[i];
		if(m_usrCurrentObject!=NULL)
		{
			m_usrCurrentObject->Selected(pDC, false);
			m_usrCurrentObject = NULL;
		}
	}
	ReleaseDC(pDC);//�ͷ�DC��Դ

}


void CRemotemonitorView::ChangetoArrow()//CMainFrame����
{
   OnArrow();
}

void CRemotemonitorView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	float x,y;
	CPoint pt;
	CRect rect;
	GetClientRect(&rect);

//	CPoint Point=GetDeviceScrollPosition();
//	Point+=point;

    CDC* pDC = GetDC();
	OnPrepareDC(pDC);//����DC�Ĺ������ԣ���ScollView�Ĺ����й�
	pDC->DPtoLP(&point);//ת����ǰ��Ϊ�߼�λ������
	CRemotemonitorDoc* pDoc = GetDocument();

	CRect client;
	GetClientRect(&client);//��ÿͻ�����Χ
	ClientToScreen(&client);//ת��Ϊ��Ļ����
	::ClipCursor(&client);//��������ƶ���Χ


	switch(m_nCurTool)
	{

	case ID_ARROW:
	   ::ClipCursor(NULL);//��������ƶ���Χ
       break;
	case ID_ZOOMIN:
        if((m_nScale+10)<=300)
		{
           m_nScale+=10;
	       CSize sizeTotal;
	       sizeTotal.cx=(long)(m_bmInfo.bmWidth*((float)m_nScale/100.0));
	       sizeTotal.cy = (long)(m_bmInfo.bmHeight*((float)m_nScale/100.0));
	       SetScrollSizes(MM_TEXT, sizeTotal);

           point.x=(long)(1.1*(float)(point.x));
           point.y=(long)(1.1*(float)(point.y));
		   if((point.x-rect.Width()/2)>=0)
			   point.x=point.x-rect.Width()/2;
		   else point.x=0;
		   if((point.y-rect.Height()/2)>=0)
			   point.y=point.y-rect.Height()/2;
		   else point.y=0;
		   ScrollToPosition(point);
           Invalidate(TRUE);
		   CStatic* pScaleStatic = &((CMainFrame*)AfxGetMainWnd())->m_scalestatic;
//		   CMainFrame* pWnd=(CMainFrame*)AfxGetMainWnd();
//           pWnd->m_nScale=m_nScale;
		   CString str;
		   str.Format("���ű���:%d",m_nScale);
		   str+="%";
           pScaleStatic->SetWindowText(str);
		}
		break;
	case ID_ZOOMOUT:
		if((m_bmInfo.bmWidth*((float)((m_nScale-10.0)/100.0))>=rect.Width())||(m_bmInfo.bmHeight*((float)((m_nScale-10.0)/100.0))>=rect.Height()))
		{
           m_nScale-=10;
	       CSize sizeTotal;
	       sizeTotal.cx=(long)(m_bmInfo.bmWidth*((float)m_nScale/100.0));
	       sizeTotal.cy = (long)(m_bmInfo.bmHeight*((float)m_nScale/100.0));
	       SetScrollSizes(MM_TEXT, sizeTotal);

           point.x=(long)(0.9*(float)(point.x));
           point.y=(long)(0.9*(float)(point.y));
		   if((point.x-rect.Width()/2)>=0)
			   point.x=point.x-rect.Width()/2;
		   else point.x=0;
		   if((point.y-rect.Height()/2)>=0)
			   point.y=point.y-rect.Height()/2;
		   else point.y=0;
		   ScrollToPosition(point);
           Invalidate(TRUE);
		   CStatic* pScaleStatic = &((CMainFrame*)AfxGetMainWnd())->m_scalestatic;
//		   CMainFrame* pWnd=(CMainFrame*)AfxGetMainWnd();
//           pWnd->m_nScale=m_nScale;

		   CString str;
		   str.Format("���ű���:%d",m_nScale);
		   str+="%";
           pScaleStatic->SetWindowText(str);
		}
		break;
	case ID_PAN:
		m_bMouseDown=TRUE;
		m_LastPt=point;
//		SetCapture();
		break;

	case ID_SELECT:
		x=(float)point.x/(float)(m_nScale/100.0);
		y=(float)point.y/(float)(m_nScale/100.0);
		pt.x=(int)(x+0.5);
		pt.y=(int)(y+0.5);
//		if(m_usrCurrentObject && (m_nMoveMode = m_usrCurrentObject->SelectAt(point.x, point.y)))
		if(m_usrCurrentObject && (m_nMoveMode = m_usrCurrentObject->SelectAt(pt.x, pt.y)))
		{//���ָ��ָ����ѡ��Ķ��󣬲����ٱ���ȫ������
			m_usrCurrentObject->Selected(pDC, true);
			m_bMouseDown = TRUE;
		//	m_nCurrentColor = m_usrCurrentObject->GetPenColor();
		//	SetCursor(m_hcurMoving);//���ù����ʾ
			::SetClassLong(GetSafeHwnd(), GCL_HCURSOR, (LONG)(AfxGetApp()->LoadCursor(IDC_USERMOVING)));
			break;//����switch�ṹ
		}
		int i;
		for(i = pDoc->m_aObjects.GetSize()-1; i >= 0; i--)
		{
			m_usrCurrentObject = pDoc->m_aObjects[i];
			m_nMoveMode = m_usrCurrentObject->SelectAt(pt.x, pt.y);
			if(m_nMoveMode)//�ж��Ƿ�ѡ��ǰͼԪ
			{//ѡ��ĳͼԪ
				m_usrCurrentObject->Selected(pDC, true);
				m_bMouseDown = true;
			//	m_nCurrentColor = m_usrCurrentObject->GetPenColor();
			//	SetCursor(m_hcurMoving);//���ù����ʾ
			    ::SetClassLong(GetSafeHwnd(), GCL_HCURSOR, (LONG)(AfxGetApp()->LoadCursor(IDC_USERMOVING)));
				break;
			}
			else
			{
				m_usrCurrentObject->Selected(pDC, false);
			}
			m_usrCurrentObject = NULL;
			m_bMouseDown = false;
		}
		for(i--;i>=0; i--)
		{//�������ͼԪ��ѡ��
			pDoc->m_aObjects[i]->Selected(pDC, false);
		}
		break;
	case ID_DELETE:
        break;
	case ID_DRAWLINE:
		m_bMouseDown = TRUE;
		m_usrCurrentObject = new CDrawLine(m_nCurrentPenColor,m_nTextColor,m_nCurrentPenWidth);
		x=(float)point.x/(float)(m_nScale/100.0);
		y=(float)point.y/(float)(m_nScale/100.0);
		pt.x=(int)(x+0.5);
		pt.y=(int)(y+0.5);
		m_nMoveMode = m_usrCurrentObject->NewPoint(pt.x, pt.y);
		break;
	case ID_DRAWCIRCLE:
		m_bMouseDown = TRUE;
		m_usrCurrentObject = new CDrawCircle(m_nCurrentPenColor,m_nCurrentFillColor,m_nTextColor);
		x=(float)point.x/(float)(m_nScale/100.0);
		y=(float)point.y/(float)(m_nScale/100.0);
		pt.x=(int)(x+0.5);
		pt.y=(int)(y+0.5);
		m_nMoveMode = m_usrCurrentObject->NewPoint(pt.x, pt.y);
		m_usrCurrentObject->Draw(pDC);
		break;

    default:
        break;	
	}
    ReleaseDC(pDC);//�ͷ�DC��Դ
	CScrollView::OnLButtonDown(nFlags, point);
}

void CRemotemonitorView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
//	CClientDC dc(this);
	float x,y;
	CPoint pt;
	CDC* pDC = GetDC();
	OnPrepareDC(pDC);//����DC�Ĺ������ԣ���ScollView�Ĺ����й�
	if(m_bMouseDown)
	{
	    if(m_nCurTool==ID_PAN)
		{
			if((m_LastPt.x-point.x)<0) m_LastPt.x=0;
			else m_LastPt.x-=point.x;
			if((m_LastPt.y-point.y)<0) m_LastPt.y=0;
			else m_LastPt.y-=point.y;
			ScrollToPosition(m_LastPt);
			pDC->DPtoLP(&point);//ת����ǰ��Ϊ�߼�λ������
			m_LastPt=point;
			//	   Invalidate(TRUE);����ScrollToPositionʵ�����ѱ�ע��������Ч��������˵һ��
		}
		if(m_usrCurrentObject)
		{
			pDC->DPtoLP(&point);
			x=(float)point.x/(float)(m_nScale/100.0);
			y=(float)point.y/(float)(m_nScale/100.0);
			pt.x=(int)(x+0.5);
			pt.y=(int)(y+0.5);
			m_usrCurrentObject->MoveAt(pDC, m_nMoveMode, pt.x, pt.y);	
		}
	}

	ReleaseDC(pDC);//�ͷ�DC��Դ
	CScrollView::OnMouseMove(nFlags, point);
}
#include <math.h>
void CRemotemonitorView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CDC* pDC = GetDC();//���DC
	OnPrepareDC(pDC);//����DC�Ĺ������ԣ���ScollView�Ĺ����й�
	pDC->DPtoLP(&point);//ת����ǰ��Ϊ�߼�λ������
	CRemotemonitorDoc* pDoc = GetDocument();

	switch(m_nCurTool)
	{
	case ID_ARROW:
	//	::ClipCursor(NULL);//�ͷ��������
	//	m_bMouseDown = false;//ֹͣ��ͼ
	//	SetCursor(m_hcurArrow);//�ָ������ʾ
	//	::SetClassLong(GetSafeHwnd(), GCL_HCURSOR, (LONG)(AfxGetApp()->LoadStandardCursor(IDC_ARROW)));
	//	Invalidate();//ˢ����ͼ
		break;

	case ID_ZOOMIN:
	case ID_ZOOMOUT:
		::ClipCursor(NULL);//�ͷ��������
		break;
	case ID_PAN:
		m_bMouseDown=FALSE;
		::ClipCursor(NULL);//�ͷ��������
		Invalidate(TRUE);
		break;
	case ID_SELECT:
		::ClipCursor(NULL);//�ͷ��������
		m_bMouseDown = false;//ֹͣ��ͼ
	//	SetCursor(m_hcurArrow);//�ָ������ʾ
		::SetClassLong(GetSafeHwnd(), GCL_HCURSOR, (LONG)(AfxGetApp()->LoadCursor(IDC_SELECT)));
		if(m_usrCurrentObject)
		   pDoc->SetModifiedFlag(TRUE);

		Invalidate();//ˢ����ͼ
		break;

	case ID_DRAWLINE:
		::ClipCursor(NULL);//�ͷ��������
		if(m_usrCurrentObject)
		{
			if(m_usrCurrentObject->distance()<10)
				delete m_usrCurrentObject;
			else
				pDoc->m_aObjects.Add(m_usrCurrentObject);
		}
		m_bMouseDown = false;
		m_usrCurrentObject = NULL;
		pDoc->SetModifiedFlag(TRUE);
		Invalidate();//ˢ����ͼ
		break;


	case ID_DRAWCIRCLE:
		::ClipCursor(NULL);//�ͷ��������
		if(m_usrCurrentObject)
		  pDoc->m_aObjects.Add(m_usrCurrentObject);
		m_bMouseDown = false;
		m_usrCurrentObject = NULL;
		pDoc->SetModifiedFlag(TRUE);
		Invalidate();//ˢ����ͼ
		break;
	}
	ReleaseDC(pDC);//�ͷ�DC��Դ
	CScrollView::OnLButtonUp(nFlags, point);
}


void CRemotemonitorView::Viewpenproperties(BOOL bShow) 
{
	// TODO: Add your command handler code here
	if(bShow)
	{
		if(m_pPenProperties==NULL)
		{
			m_pPenProperties=new CPenProperties(this);
			m_pPenProperties->Create(IDD_PENPROPERTIES,this);
			m_pPenProperties->ShowWindow(SW_SHOW);
		}
		else 
			m_pPenProperties->SetActiveWindow();
	}
	else
	{
//		m_pPenProperties->DestroyWindow();
		delete m_pPenProperties;
		m_pPenProperties=NULL;
	}
}
void CRemotemonitorView::OnMapchange() 
{
	// TODO: Add your command handler code here
    CFileDialog fileopenbox(TRUE,NULL,NULL,OFN_HIDEREADONLY,"��ͼ(*.bmp)|*.bmp|",NULL);	
	if(fileopenbox.DoModal()!=IDOK) return;
	m_strMapPathName=fileopenbox.GetPathName();

    SetRedraw(FALSE);
	if(m_hOldBitmap&&m_hMemoryDC)
	{
		SelectObject(m_hMemoryDC,m_hOldBitmap);
		m_hOldBitmap=NULL;
	}
	if(m_hBitmap)
	{
		DeleteObject(m_hBitmap);
        m_hBitmap=NULL;
	}
	if(m_hMemoryDC)
	{
		DeleteDC(m_hMemoryDC);
        m_hMemoryDC=NULL;
	}
	   initMemoryDC();
       m_nScale=100;
	   CStatic* pScaleStatic = &((CMainFrame*)AfxGetMainWnd())->m_scalestatic;
//	   CMainFrame* pWnd=(CMainFrame*)AfxGetMainWnd();
//	   pWnd->m_nScale=m_nScale;
	   
	   CString str;
	   str.Format("���ű���:%d",m_nScale);
	   str+="%";
	   pScaleStatic->SetWindowText(str);
	   
	   SetRedraw(TRUE);	
	   Invalidate(TRUE);
//	}
}

void CRemotemonitorView::OnDrawline() 
{
	// TODO: Add your command handler code here
	m_nCurTool=ID_DRAWLINE;
	HCURSOR hCursor;
    hCursor=AfxGetApp()->LoadCursor(IDC_USERDRAW);
	::SetClassLong(GetSafeHwnd(), GCL_HCURSOR, (LONG)hCursor);
	int i;
    CDC* pDC = GetDC();
	OnPrepareDC(pDC);//����DC�Ĺ������ԣ���ScollView�Ĺ����й�
	CRemotemonitorDoc* pDoc = GetDocument();

	for(i = pDoc->m_aObjects.GetSize()-1; i >= 0; i--)
	{
		m_usrCurrentObject = pDoc->m_aObjects[i];
		m_usrCurrentObject->Selected(pDC, false);
		m_usrCurrentObject = NULL;
	}
	ReleaseDC(pDC);//�ͷ�DC��Դ
	
}

void CRemotemonitorView::OnDrawcircle() 
{
	// TODO: Add your command handler code here
	m_nCurTool=ID_DRAWCIRCLE;
	HCURSOR hCursor;
    hCursor=AfxGetApp()->LoadCursor(IDC_USERDRAW);
	::SetClassLong(GetSafeHwnd(), GCL_HCURSOR, (LONG)hCursor);
	int i;
    CDC* pDC = GetDC();
	OnPrepareDC(pDC);//����DC�Ĺ������ԣ���ScollView�Ĺ����й�
	CRemotemonitorDoc* pDoc = GetDocument();

	for(i = pDoc->m_aObjects.GetSize()-1; i >= 0; i--)
	{
		m_usrCurrentObject = pDoc->m_aObjects[i];
		m_usrCurrentObject->Selected(pDC, false);
		m_usrCurrentObject = NULL;
	}
	ReleaseDC(pDC);//�ͷ�DC��Դ
	
}

void CRemotemonitorView::OnUpdateDrawline(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
    CMainFrame* pWnd=(CMainFrame*)AfxGetMainWnd();
	pCmdUI->Enable(pWnd->m_bFlag);

	pCmdUI->SetCheck(m_nCurTool== ID_DRAWLINE);
	
}

void CRemotemonitorView::OnUpdateDrawcircle(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
    CMainFrame* pWnd=(CMainFrame*)AfxGetMainWnd();
	pCmdUI->Enable(pWnd->m_bFlag);

	pCmdUI->SetCheck(m_nCurTool== ID_DRAWCIRCLE);
	
}

void CRemotemonitorView::OnSelect() 
{
	// TODO: Add your command handler code here
	m_nCurTool=ID_SELECT;
	HCURSOR hCursor;
    hCursor=AfxGetApp()->LoadCursor(IDC_SELECT);
	::SetClassLong(GetSafeHwnd(), GCL_HCURSOR, (LONG)hCursor);
	
}

void CRemotemonitorView::OnUpdateSelect(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
    CMainFrame* pWnd=(CMainFrame*)AfxGetMainWnd();
	pCmdUI->Enable(pWnd->m_bFlag);
	pCmdUI->SetCheck(m_nCurTool== ID_SELECT);
	
}

void CRemotemonitorView::OnDelete() 
{
	// TODO: Add your command handler code here
	CRemotemonitorDoc* pDoc = GetDocument();
	int i;

	if(m_usrCurrentObject)
	{//��ǰѡ����ͼԪ
		for(i=pDoc->m_aObjects.GetSize()-1; i>=0; i--)
		{//����ͼԪ�����б�
			if(pDoc->m_aObjects[i] == m_usrCurrentObject)
			{//�ҵ���ɾ����ͼԪ
				pDoc->m_aObjects.RemoveAt(i);
				delete m_usrCurrentObject;
				m_usrCurrentObject = NULL;
				break;
			}
		}
		pDoc->SetModifiedFlag(TRUE);
		Invalidate();
	}	
}

void CRemotemonitorView::OnUpdateDelete(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_nCurTool == ID_SELECT && m_usrCurrentObject != NULL);
	//��ѡ��ͼԪʱ���ſ�ʹ��ɾ������
	
}
BOOL CRemotemonitorView::SeekFrameHead(int &start)
{
	int m=0;
	while(m<InDataLen)
	{
        if((InDataBuf[m]==SYN)&&(InDataBuf[m+1]==DLE)&&(InDataBuf[m+2]==STX))
		{
		   // �ҵ�ͬ��ͷ
           start=m;
		   return TRUE;
		}
		else
		   m++;
	}
    start=0;
	return FALSE;	// û�ҵ�ͬ��ͷ
}
BOOL CRemotemonitorView::SeekFrameEnd(int start,int &end)
{
	int m=start;
	while(m<(InDataLen-1))
	{
		if(InDataBuf[m]==DLE&&InDataBuf[m+1]==DLE)//�����г�����DLE
			m+=2;
		else if(InDataBuf[m]==DLE&&InDataBuf[m+1]==ETX)
		{
		   // �ҵ���������־
           end=m;
		   return TRUE;
		}
		else
		   m++;
	}
    end=0;
	return FALSE;	// û�ҵ�ͬ��ͷ
}
unsigned char CRemotemonitorView::crc(BYTE* buffer,int Len)
{
    unsigned char fcs;
	fcs=0x00;
	while(Len--)
	{
		fcs^=*buffer++;
	}
	return fcs;
}
//��Header��Data�в���ת���ַ�DLE
int CRemotemonitorView::InsertDLE(BYTE* buffer0,BYTE* buffer1,int len)
{
	int i;
	int addbyte=0;
	for(i=0;i<sizeof(PKSTART);i++)
		*buffer1++=*buffer0++;
	for(i=0;i<len;i++)
	{
	   *buffer1=*buffer0;
       if(*buffer0==0x10)
	   {
          buffer1++;
          *buffer1=0x10;
          addbyte++;
	   }
	   buffer1++;
	   buffer0++;
	}
	for(i=0;i<sizeof(PKEND)+2;i++)
		*buffer1++=*buffer0++;
	return  addbyte;
}
//ȥ��ת���ַ�DLE
int CRemotemonitorView::DeleteDLE(int nStart,int nEnd)
{
	int m,n;
	m=nStart+3;
	n=nEnd;
	while(m<n)
	{
		if(InDataBuf[m]==DLE&&InDataBuf[m+1]==DLE)
		{
           memcpy(&InDataBuf[m+1],&InDataBuf[m+2],InDataLen-(m+2));
           InDataLen--;
		   n--;
		}
		m++;
	}
	return n;
}
void CRemotemonitorView::SendLR()
{
   unsigned char SendBuf[1024];
   unsigned char SendBuf1[1024];
   int nlen;
   int addByte;

   memset(SendBuf,0,sizeof(SendBuf));
   memset(SendBuf1,0,sizeof(SendBuf1));
   memcpy(SendBuf,&m_pkstart,sizeof(PKSTART));
   nlen=sizeof(PKSTART);
   m_header.pktype=0x01;
   m_header.para=0x00;
   memcpy(SendBuf+nlen,&m_header,sizeof(HEADER));
   nlen+=sizeof(HEADER);
   memcpy(SendBuf+nlen,&m_pkend,sizeof(PKEND));
   nlen+=sizeof(PKEND);
   addByte=InsertDLE(SendBuf,SendBuf1,nlen-sizeof(PKSTART)-sizeof(PKEND));
   nlen+=addByte;
   WriteCommBlock(&Comminfo, (LPSTR)SendBuf1,nlen);
}

void CRemotemonitorView::SendLA(int nflag)
{
   unsigned char SendBuf[1024];
   unsigned char SendBuf1[1024];
   int nlen;
   int addByte;

   memset(SendBuf,0,sizeof(SendBuf));
   memset(SendBuf1,0,sizeof(SendBuf1));
   memcpy(SendBuf,&m_pkstart,sizeof(PKSTART));
   nlen=sizeof(PKSTART);
   m_header.pktype=0x02;
   if(nflag==1)
     m_header.para=0x01;//��LRӦ��
   else if(nflag==2)
     m_header.para=0x02;//����ȷ��LTӦ��
   else
     m_header.para=0x04;//�Դ����LTӦ��
   memcpy(SendBuf+nlen,&m_header,sizeof(HEADER));
   nlen+=sizeof(HEADER);
   memcpy(SendBuf+nlen,&m_pkend,sizeof(PKEND));
   nlen+=sizeof(PKEND);
   addByte=InsertDLE(SendBuf,SendBuf1,nlen-sizeof(PKSTART)-sizeof(PKEND));
   nlen+=addByte;
   WriteCommBlock(&Comminfo, (LPSTR)SendBuf1,nlen);
}

void CRemotemonitorView::SendLT()
{
   unsigned char SendBuf[1024];
   unsigned char SendBuf1[1024];
   int nlen;
   int addByte;
   CString str;
   LPTSTR p;

   memset(SendBuf,0,sizeof(SendBuf));
   memset(SendBuf1,0,sizeof(SendBuf1));
   memcpy(SendBuf,&m_pkstart,sizeof(PKSTART));
   nlen=sizeof(PKSTART);
   m_header.pktype=0x04;
   m_header.para=0x00;
   memcpy(SendBuf+nlen,&m_header,sizeof(HEADER));
   nlen+=sizeof(HEADER);
   str=m_strArray.GetAt(0);
   p=str.GetBuffer(1024); 
   memcpy(SendBuf+nlen,p,str.GetLength());
   maptoDTMF(SendBuf+nlen,str.GetLength());
   str.ReleaseBuffer();
   nlen+=str.GetLength();
   SendBuf[nlen]=crc(SendBuf+sizeof(PKSTART),sizeof(HEADER)+str.GetLength());
   nlen+=1;
   memcpy(SendBuf+nlen,&m_pkend,sizeof(PKEND));
   nlen+=sizeof(PKEND);
   addByte=InsertDLE(SendBuf,SendBuf1,nlen-sizeof(PKSTART)-sizeof(PKEND));
   nlen+=addByte;
   WriteCommBlock(&Comminfo, (LPSTR)SendBuf1,nlen);
}
void CRemotemonitorView::maptoDTMF(unsigned char* pbuff,int len)
{
	CString str;
	int nIndex;
	str=pbuff;

	if(str.Left(2)=="CD")//GBK������
	{
      pbuff[len-1]=0x0b;//������*��Ϊ0x0b
      nIndex=str.Find("#")+1;
      while(nIndex--)
	  {
		*pbuff=asctodtmf[*pbuff];
         pbuff++;
	  }
      str=pbuff;
      nIndex=str.Find("#")+1;
      while(nIndex--)
	  {
		*pbuff=asctodtmf[*pbuff];
         pbuff++;
	  }
      str=pbuff;
      nIndex=str.Find("#")+1;
      while(nIndex--)
	  {
		*pbuff=asctodtmf[*pbuff];
         pbuff++;
	  }
	}
	else if(str.Left(2)=="BD")
	{
      pbuff[len-1]=0x0b;//������*��Ϊ0x0b
	  nIndex=str.Find('#')+1;
      while(nIndex--)
	  {
		*pbuff=asctodtmf[*pbuff];
         pbuff++;
	  }
	}
	else
	{
        while(len--)
		{
			*pbuff=asctodtmf[*pbuff];
            pbuff++;
		}
	}
}

void CRemotemonitorView::SetAlarmFlag(CString strNodeNum,CString strCableNum)
{
	int i;
	int nNodeNum;
    CDrawObject* pObject=NULL;
	CRemotemonitorDoc* pDoc = GetDocument();
    nNodeNum=atoi(strNodeNum)*100+atoi(strCableNum);
	for(i=0;i<pDoc->m_aObjects.GetSize(); i++)
	{
		pObject = pDoc->m_aObjects[i];
        if(pObject&&pObject->GetMark()==nNodeNum)
		{
           pObject->m_bAlarm=TRUE;
		   break;
		}
	}
}

void CRemotemonitorView::ProcessRevInfo(CString strRevInfo)
{
	CString strNodeNum;
	int     nNode;
	CString strFenjiName;
	CString strCableNum;
	CString strCableName;
	CString strNodeName;
//	CString strState;
	CString strDistance;
	CString strAlarmTime;
	CString strMaintenance[6];
	CString strTelephone[6];

	CString strTemp;

    int nIndex;
	int nYear,nMonth,nDay,nHour,nMinite,nSecond;
	int nAlarmtype;
	int i;
	CString strSendInfo;

	CTime time;
    time=CTime::GetCurrentTime();

    if(strRevInfo.Left(2)==_T("AA"))
	{
       strRevInfo.Delete(0,3);
	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1) return;
       strNodeNum=strRevInfo.Left(nIndex);
	   if(atoi(strNodeNum)<1||atoi(strNodeNum)>999)
		   return;
       strRevInfo.Delete(0,nIndex);//#��û��ɾ��
	   do
	   {
           strRevInfo.Delete(0,1);//�ѿ�ʼ��#ɾ��
		   nIndex=strRevInfo.Find('#');
		   if(nIndex==-1) return;
		   strCableNum=strRevInfo.Left(nIndex);
		   if(atoi(strCableNum)<1||atoi(strCableNum)>16)
			   return;
		   strRevInfo.Delete(0,nIndex+1);
		   nAlarmtype=atoi(strRevInfo.Left(1));
		   if(nAlarmtype<0||nAlarmtype>8)
			   return;
           strRevInfo.Delete(0,1);
		   nNode=atoi(strNodeNum)*100+atoi(strCableNum);
		   strTemp.Format("%d",nNode);
		   m_cabletable.m_strSort=_T("[Id]");
           m_cabletable.m_strFilter=_T("[Id] = "+strTemp); 
           m_cabletable.Requery();
		   if(!(m_cabletable.IsEOF()&&m_cabletable.IsBOF()))//��Ϊ��
		   {
			   m_cabletable.Edit();
			   m_cabletable.m_state=nAlarmtype;
			   m_cabletable.Update();
		   }
		   //�˴�����/����״̬д�����ݿ�
	   }while(strRevInfo.Left(1)!="*");
	}
/*    else if(strRevInfo.Left(2)==_T("AB"))
	{
       strRevInfo.Delete(0,3);

	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1) return;
       strNodeNum=strRevInfo.Left(nIndex);// �ֻ���
       strRevInfo.Delete(0,nIndex+1);

	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1) return;
	   strCableNum=strRevInfo.Left(nIndex);//���º�
	   strRevInfo.Delete(0,nIndex+1);

	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1) return;
	   strTemp=strRevInfo.Left(nIndex);//��
	   strRevInfo.Delete(0,nIndex+1);
       nMonth=atoi(strTemp);

	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1) return;
	   strTemp=strRevInfo.Left(nIndex);//��
	   strRevInfo.Delete(0,nIndex+1);
       nDay=atoi(strTemp);

	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1) return;
	   strTemp=strRevInfo.Left(nIndex);//ʱ
	   strRevInfo.Delete(0,nIndex+1);
       nHour=atoi(strTemp);

	   nIndex=strRevInfo.Find('*');
	   if(nIndex==-1) return;
	   strTemp=strRevInfo.Left(nIndex);//��
	   strRevInfo.Delete(0,nIndex+1);
       nMinite=atoi(strTemp);

       nSecond=0;

       nYear=time.GetYear();
	   CTime alarmtime(nYear,nMonth,nDay,nHour,nMinite,nSecond);

       cablealarmtable.m_strSort=_T("[alarmtime]");
       nNode=atoi(strNodeNum)*100+atoi(strCableNum);
	   strTemp.Format("%d",nNode);
       cablealarmtable.m_strFilter=_T("[cablenumber]="+strTemp);
       if(!cablealarmtable.IsOpen())
          cablealarmtable.Open();
	   if(!(cablealarmtable.IsBOF()&&cablealarmtable.IsEOF()))
	   {
		   while(!cablealarmtable.IsEOF())
               cablealarmtable.MoveNext();
           cablealarmtable.MovePrev();
           cablealarmtable.Edit();
           cablealarmtable.m_recovertime=alarmtime;
           cablealarmtable.Update();
	   }
	   if(cablealarmtable.IsOpen())
          cablealarmtable.Close();
	}
	*/
    else if(strRevInfo.Left(2)==_T("AC"))
	{
       strRevInfo.Delete(0,3);
	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1) return;
       strNodeNum=strRevInfo.Left(nIndex);
	   if(atoi(strNodeNum)<1||atoi(strNodeNum)>999)
		   return;
       strRevInfo.Delete(0,nIndex+1);
	   nIndex=strRevInfo.Find('*');
	   if(nIndex==-1) return;
       strTemp=strRevInfo.Left(nIndex);//��Ӧ��ȴ�ʱ��
	   if(atoi(strTemp)<1||atoi(strTemp)>99)
		   return;
	   m_fenjitable.m_strSort=_T("[Id]");
       m_fenjitable.m_strFilter=_T("[Id] = "+strNodeNum); 
       m_fenjitable.Requery();
	   if(!(m_fenjitable.IsEOF()&&m_fenjitable.IsBOF()))//��Ϊ��
	   {
		   m_fenjitable.Edit();
		   m_fenjitable.m_ackwaittime=atoi(strTemp);
		   m_fenjitable.Update();
	   }
	}
    else if(strRevInfo.Left(2)==_T("AD"))
	{
       strRevInfo.Delete(0,3);
	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1) return;
       strNodeNum=strRevInfo.Left(nIndex);
	   if(atoi(strNodeNum)<1||atoi(strNodeNum)>999)
		   return;
       strRevInfo.Delete(0,nIndex+1);
	   nIndex=strRevInfo.Find('*');
	   if(nIndex==-1) return;
       strTemp=strRevInfo.Left(nIndex);//������
	   if(atoi(strTemp)<1||atoi(strTemp)>30)
		   return;
	   m_fenjitable.m_strSort=_T("[Id]");
       m_fenjitable.m_strFilter=_T("[Id] = "+strNodeNum); 
       m_fenjitable.Requery();
	   if(!(m_fenjitable.IsEOF()&&m_fenjitable.IsBOF()))//��Ϊ��
	   {
		   m_fenjitable.Edit();
		   m_fenjitable.m_sensitive=atoi(strTemp);
		   m_fenjitable.Update();
	   }
	}
    else if(strRevInfo.Left(2)==_T("BA"))
	{
       strRevInfo.Delete(0,3);
	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1) return;
       strNodeNum=strRevInfo.Left(nIndex);
	   if(atoi(strNodeNum)<1||atoi(strNodeNum)>999)
		   return;
       strRevInfo.Delete(0,nIndex+1);
	   nIndex=strRevInfo.Find('*');
	   if(nIndex==-1) return;
       strTemp=strRevInfo.Left(nIndex);//ѭ������绰����
	   if(atoi(strTemp)<1||atoi(strTemp)>9)
		   return;
	   m_fenjitable.m_strSort=_T("[Id]");
       m_fenjitable.m_strFilter=_T("[Id] = "+strNodeNum); 
       m_fenjitable.Requery();
	   if(!(m_fenjitable.IsEOF()&&m_fenjitable.IsBOF()))//��Ϊ��
	   {
		   m_fenjitable.Edit();
		   m_fenjitable.m_callnumber=atoi(strTemp);
		   m_fenjitable.Update();
	   }
	}
    else if(strRevInfo.Left(2)==_T("BB"))
	{
       strRevInfo.Delete(0,3);
	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1) return;
       strNodeNum=strRevInfo.Left(nIndex);
	   if(atoi(strNodeNum)<1||atoi(strNodeNum)>999)
		   return;
       strRevInfo.Delete(0,nIndex+1);
	   nIndex=strRevInfo.Find('*');
	   if(nIndex==-1) return;
       strTemp=strRevInfo.Left(nIndex);//������ʱ��
	   if(atoi(strTemp)<20||atoi(strTemp)>255)
		   return;
	   m_fenjitable.m_strSort=_T("[Id]");
       m_fenjitable.m_strFilter=_T("[Id] = "+strNodeNum); 
       m_fenjitable.Requery();
	   if(!(m_fenjitable.IsEOF()&&m_fenjitable.IsBOF()))//��Ϊ��
	   {
		   m_fenjitable.Edit();
		   m_fenjitable.m_waittime=atoi(strTemp);
		   m_fenjitable.Update();
	   }
	}
    else if(strRevInfo.Left(2)==_T("BC"))
	{
       strRevInfo.Delete(0,3);
	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1) return;
       strNodeNum=strRevInfo.Left(nIndex);
	   if(atoi(strNodeNum)<1||atoi(strNodeNum)>999)
		   return;
       strRevInfo.Delete(0,nIndex+1);
	   nIndex=strRevInfo.Find('*');
	   if(nIndex==-1) return;
       strTemp=strRevInfo.Left(nIndex);//���ص�ѹת�۵�
	   if(atof(strTemp)<1.1||atof(strTemp)>9.9)
		   return;
	   m_fenjitable.m_strSort=_T("[Id]");
       m_fenjitable.m_strFilter=_T("[Id] = "+strNodeNum); 
       m_fenjitable.Requery();
	   if(!(m_fenjitable.IsEOF()&&m_fenjitable.IsBOF()))//��Ϊ��
	   {
		   m_fenjitable.Edit();
		   m_fenjitable.m_xudianchi=(float)(atof(strTemp)/10.0);
		   m_fenjitable.Update();
	   }
	}
	else if(strRevInfo.Left(2)==_T("BD"))
	{
      //
	   Sleep(2000);
       if(m_uSendTimer)
	   {
		   KillTimer(m_uSendTimer);
		   m_uSendTimer=SetTimer(SNDTIMER,m_SendT,NULL);
	   }

	   CMainFrame* pWnd;

	   pWnd=(CMainFrame*)AfxGetMainWnd();


       pWnd->ShowWindowMax();
       strRevInfo.Delete(0,3);

	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1) return;
       strNodeNum=strRevInfo.Left(nIndex);
	   if(atoi(strNodeNum)<1||atoi(strNodeNum)>999)
		   return;
       strRevInfo.Delete(0,nIndex+1);


	   //���ݷֻ����ҳ�ά����Ա���绰��������strMaintenance��strTelephone������
	   nNode=atoi(strNodeNum);
	   strTemp.Format("%d",nNode);
	   m_fenjitable.m_strSort=_T("[Id]");
	   m_fenjitable.m_strFilter=_T("[Id] = "+strTemp); 
	   m_fenjitable.Requery();
	   if(!(m_fenjitable.IsEOF()&&m_fenjitable.IsBOF()))//��Ϊ��
	   {
		   strFenjiName=m_fenjitable.m_Name;
		   for(i=0;i<6;i++)
		   {
			   strMaintenance[i]=m_fenjitable.m_maintenance[i];
			   strTelephone[i]=m_fenjitable.m_maintenancetel[i];
		   }
	   }
	   else
	   {
		   strFenjiName="";
		   for(i=0;i<6;i++)
		   {
			   strMaintenance[i]="";
			   strTelephone[i]="";
		   }
	   }

	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1) return;
	   strCableNum=strRevInfo.Left(nIndex);//�˿ں�
	   if(atoi(strCableNum)<1||atoi(strCableNum)>16)
		   return;

	   strRevInfo.Delete(0,nIndex+1);
	   
	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1) return;
	   nAlarmtype=atoi(strRevInfo.Left(nIndex));//�������
	   if(nAlarmtype<0||nAlarmtype>9)
		   return;
	   strRevInfo.Delete(0,nIndex+1);

	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1) return;
	   strDistance=strRevInfo.Left(nIndex);//����
	   if(atoi(strDistance)<0||atoi(strDistance)>99999)
		   return;
	   strRevInfo.Delete(0,nIndex+1);
	   
	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1) return;
	   strTemp=strRevInfo.Left(nIndex);//��
	   strRevInfo.Delete(0,nIndex+1);
	   nMonth=atoi(strTemp);
	   if(nMonth<1||nMonth>12)
		   return;
	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1) return;
	   strTemp=strRevInfo.Left(nIndex);//��
	   strRevInfo.Delete(0,nIndex+1);
	   nDay=atoi(strTemp);
	   if(nDay<1||nDay>31)
		   return;
	   
	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1) return;
	   strTemp=strRevInfo.Left(nIndex);//ʱ
	   strRevInfo.Delete(0,nIndex+1);
	   nHour=atoi(strTemp);
	   if(nHour<0||nHour>24)
		   return;
	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1)//û��#�ţ���һ����*��
	       nIndex=strRevInfo.Find('*');
	   if(nIndex==-1) return;//�����Ϊ-1���򷵻�
	   strTemp=strRevInfo.Left(nIndex);//��
	   strRevInfo.Delete(0,nIndex);//û��ɾ������#��
	   nMinite=atoi(strTemp);
	   if(nMinite<0||nMinite>60)
		   return;

       nSecond=0;
       nYear=time.GetYear();
	   CTime alarmtime(nYear,nMonth,nDay,nHour,nMinite,nSecond);
//	   strAlarmTime=alarmtime.Format("%Y-%m-%d %H:%M:%S");
	   strAlarmTime=alarmtime.Format("%Y-%m-%d %H:%M");
       if(nAlarmtype==9)//�ѱ����ָ�ʱ��д�����ݿ�
	   {
          m_cablealarmtable.m_strSort=_T("[alarmtime]");
          nNode=atoi(strNodeNum)*100+atoi(strCableNum);
	      strTemp.Format("%d",nNode);
          m_cablealarmtable.m_strFilter=_T("[cablenumber]="+strTemp);
	      if(!(m_cablealarmtable.IsBOF()&&m_cablealarmtable.IsEOF()))
		  {
		     while(!m_cablealarmtable.IsEOF())
                 m_cablealarmtable.MoveNext();
             m_cablealarmtable.MovePrev();
             m_cablealarmtable.Edit();
             m_cablealarmtable.m_recovertime=alarmtime;
             m_cablealarmtable.Update();
		  }
	   }
	   else//strState=0-8
	   {
		   if(nAlarmtype==0)
		   {
			   nAlarmtype=9+atoi(strCableNum);
               strCableNum="00";
		   }
		   m_cablealarmtable.AddNew();
		   m_cablealarmtable.m_cablenumber=atoi(strNodeNum)*100+atoi(strCableNum);
		   m_cablealarmtable.m_alarmtype=nAlarmtype;
		   m_cablealarmtable.m_distant=atoi(strDistance);
		   m_cablealarmtable.m_alarmtime=alarmtime;
		   m_cablealarmtable.m_fenjiname=strFenjiName;
		   m_cablealarmtable.Update();
           //�ӵ��±����ҵ��õ��µ�����
		   if(nAlarmtype==8)
		   {
			   nNode=atoi(strNodeNum)*100+atoi(strCableNum);
			   strTemp.Format("%d",nNode);
			   m_cabletable.m_strSort=_T("[Id]");
			   m_cabletable.m_strFilter=_T("[Id] = "+strTemp); 
			   m_cabletable.Requery();
			   if(!(m_cabletable.IsEOF()&&m_cabletable.IsBOF()))//��Ϊ��
			   {
				   if((m_cabletable.m_startname!="")&&(m_cabletable.m_startname!=""))
				      strCableName=m_cabletable.m_startname+"��"+m_cabletable.m_endname;
				   else
                      strCableName="";
			   }
			   else
				   strCableName="";
		   }
	/*	   //���ݷֻ����ҳ�ά����Ա���绰��������strMaintenance��strTelephone������
		   nNode=atoi(strNodeNum);
		   strTemp.Format("%d",nNode);
		   m_fenjitable.m_strSort=_T("[Id]");
		   m_fenjitable.m_strFilter=_T("[Id] = "+strTemp); 
		   m_fenjitable.Requery();
		   if(!(m_fenjitable.IsEOF()&&m_fenjitable.IsBOF()))//��Ϊ��
		   {
			   strFenjiName=m_fenjitable.m_Name;
			   for(i=0;i<6;i++)
			   {
				   strMaintenance[i]=m_fenjitable.m_maintenance[i];
				   strTelephone[i]=m_fenjitable.m_maintenancetel[i];
			   }
		   }
		   else
		   {
			   strFenjiName="";
			   for(i=0;i<6;i++)
			   {
				   strMaintenance[i]="";
				   strTelephone[i]="";
			   }
		   }
*/		   if(nAlarmtype==8)
              strNodeName=strCableName;
		   else
              strNodeName=strFenjiName;
	
		   //�����Ļ�����GBK��
		   strSendInfo="BD#";
		   strSendInfo+=strFenjiName;
		   strSendInfo+=",";
		   strSendInfo+=strNodeNum;
		   strSendInfo+="�ŷֻ�,";
		   if(nAlarmtype==8)
		   {
		      strSendInfo+=strCableName;
		      strSendInfo+=strCableNum;
		      strSendInfo+="�ŵ���,��";
              CString strTemp;
			  strTemp.Format("%d",atoi(strDistance));
		      strSendInfo+=strTemp;
		      strSendInfo+="�״�����*";
		   }
		   else if(nAlarmtype>=1&&nAlarmtype<=7)
		   {
		      strSendInfo+=strCableNum;
		      strSendInfo+="�Ŷ˿�";
			  strSendInfo+=strAlarmType[nAlarmtype-1];
			  strSendInfo+="*";
		   }
		   else
		   {
			  strSendInfo+=strAlarmType[nAlarmtype-1];
			  strSendInfo+="*";
		   }
//		   Sleep(2000);
		   m_strArray.Add(strSendInfo);
		   //�����б�
           InsertListItem(strNodeNum,
                          strFenjiName,
					      strCableNum,
					      strNodeName,
					      strAlarmType[nAlarmtype-1],
					      strDistance,
					      strAlarmTime,
					      strMaintenance,
					      strTelephone);
		   if(!m_bAlarm)
		   {
			   m_bAlarm=TRUE;
			   PlaySound("Alarm.wav",NULL,SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NODEFAULT);
		   }
		   if(nAlarmtype==8)
		     SetAlarmFlag(strNodeNum,strCableNum);
		   else
		     SetAlarmFlag(strNodeNum);
	   }
	   if(strRevInfo.Left(1)!="*")
	   {
		   do
		   {
			   strRevInfo.Delete(0,1);//�ѿ�ʼ��#ɾ��
			   
			   nIndex=strRevInfo.Find('#');
			   if(nIndex==-1) return;
			   strCableNum=strRevInfo.Left(nIndex);//�˿ں�
			   if(atoi(strCableNum)<1||atoi(strCableNum)>16)
				   return;
			   strRevInfo.Delete(0,nIndex+1);
			   
			   nIndex=strRevInfo.Find('#');
			   if(nIndex==-1) return;
			   nAlarmtype=atoi(strRevInfo.Left(nIndex));//�������
			   if(nAlarmtype<0||nAlarmtype>9)
				   return;

			   strRevInfo.Delete(0,nIndex+1);
			   
			   nIndex=strRevInfo.Find('#');
			   if(nIndex==-1) return;
			   strDistance=strRevInfo.Left(nIndex);//����
			   if(atoi(strDistance)<0||atoi(strDistance)>99999)
				   return;
			   strRevInfo.Delete(0,nIndex+1);
			   
			   nIndex=strRevInfo.Find('#');
			   if(nIndex==-1) return;
			   strTemp=strRevInfo.Left(nIndex);//��
			   strRevInfo.Delete(0,nIndex+1);
			   nMonth=atoi(strTemp);
			   if(nMonth<1||nMonth>12)
				   return;
			   
			   nIndex=strRevInfo.Find('#');
			   if(nIndex==-1) return;
			   strTemp=strRevInfo.Left(nIndex);//��
			   strRevInfo.Delete(0,nIndex+1);
			   nDay=atoi(strTemp);
			   if(nDay<1||nDay>31)
				   return;
			   
			   nIndex=strRevInfo.Find('#');
			   if(nIndex==-1) return;
			   strTemp=strRevInfo.Left(nIndex);//ʱ
			   strRevInfo.Delete(0,nIndex+1);
			   nHour=atoi(strTemp);
			   if(nHour<0||nHour>24)
				   return;
			   
			   nIndex=strRevInfo.Find('#');
			   if(nIndex==-1)//û��#�ţ���һ����*��
				   nIndex=strRevInfo.Find('*');
			   if(nIndex==-1) return;//�����Ϊ-1���򷵻�
			   strTemp=strRevInfo.Left(nIndex);//��
			   strRevInfo.Delete(0,nIndex);//û��ɾ������#��
			   nMinite=atoi(strTemp);
			   if(nMinite<0||nMinite>60)
				   return;
			   
			   nSecond=0;
			   nYear=time.GetYear();
			   CTime alarmtime(nYear,nMonth,nDay,nHour,nMinite,nSecond);
//			   strAlarmTime=alarmtime.Format("%Y-%m-%d %H:%M:%S");
			   strAlarmTime=alarmtime.Format("%Y-%m-%d %H:%M");
			   if(nAlarmtype==9)//�ѱ����ָ�ʱ��д�����ݿ�
			   {
				   m_cablealarmtable.m_strSort=_T("[alarmtime]");
				   nNode=atoi(strNodeNum)*100+atoi(strCableNum);
				   strTemp.Format("%d",nNode);
				   m_cablealarmtable.m_strFilter=_T("[cablenumber]="+strTemp);
				   if(!(m_cablealarmtable.IsBOF()&&m_cablealarmtable.IsEOF()))
				   {
					   while(!m_cablealarmtable.IsEOF())
						   m_cablealarmtable.MoveNext();
					   m_cablealarmtable.MovePrev();
					   m_cablealarmtable.Edit();
					   m_cablealarmtable.m_recovertime=alarmtime;
					   m_cablealarmtable.Update();
				   }
			   }
			   else//strState=1-8
			   {
				   if(nAlarmtype==0)
				   {
					   nAlarmtype=9+atoi(strCableNum);
					   strCableNum="00";
				   }

				   CString strNodeName;
				   m_cablealarmtable.AddNew();
				   m_cablealarmtable.m_cablenumber=atoi(strNodeNum)*100+atoi(strCableNum);
				   m_cablealarmtable.m_alarmtype=nAlarmtype;
				   m_cablealarmtable.m_distant=atoi(strDistance);
				   m_cablealarmtable.m_alarmtime=alarmtime;
				   m_cablealarmtable.m_fenjiname=strFenjiName;
				   m_cablealarmtable.Update();
				   //�ӵ��±����ҵ��õ��µ�����
				   if(nAlarmtype==8)
				   {
					   nNode=atoi(strNodeNum)*100+atoi(strCableNum);
					   strTemp.Format("%d",nNode);
					   m_cabletable.m_strSort=_T("[Id]");
					   m_cabletable.m_strFilter=_T("[Id] = "+strTemp); 
					   m_cabletable.Requery();
					   if(!(m_cabletable.IsEOF()&&m_cabletable.IsBOF()))//��Ϊ��
					   {
						   strCableName=m_cabletable.m_startname+"��"+m_cabletable.m_endname;
					   }
					   else
						   strCableName="";
				   }
/*				   //���ݷֻ����ҳ�ά����Ա���绰��������strMaintenance��strTelephone������
				   nNode=atoi(strNodeNum);
				   strTemp.Format("%d",nNode);
				   m_fenjitable.m_strSort=_T("[Id]");
				   m_fenjitable.m_strFilter=_T("[Id] = "+strTemp); 
				   m_fenjitable.Requery();
				   if(!(m_fenjitable.IsEOF()&&m_fenjitable.IsBOF()))//��Ϊ��
				   {
					   strFenjiName=m_fenjitable.m_Name;
					   for(i=0;i<6;i++)
					   {
						   strMaintenance[i]=m_fenjitable.m_maintenance[i];
						   strTelephone[i]=m_fenjitable.m_maintenancetel[i];
					   }
				   }
				   else
				   {
					   strFenjiName="";
					   for(i=0;i<6;i++)
					   {
						   strMaintenance[i]="";
						   strTelephone[i]="";
					   }
				   }
*/				   if(nAlarmtype==8)
					   strNodeName=strCableName;
				   else
					   strNodeName=strFenjiName;

				   //�����б�
				   InsertListItem(strNodeNum,
                       strFenjiName,
					   strCableNum,
					   strNodeName,
					   strAlarmType[nAlarmtype-1],
					   strDistance,
					   strAlarmTime,
					   strMaintenance,
					   strTelephone);
				   if(!m_bAlarm)
				   {
					   m_bAlarm=TRUE;
					   PlaySound("Alarm.wav",NULL,SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NODEFAULT);
				   }
		           if(nAlarmtype==8)
		              SetAlarmFlag(strNodeNum,strCableNum);
		           else
		              SetAlarmFlag(strNodeNum);
			   }
		   }while(strRevInfo.Left(1)!="*");
	   }
	}
	else if(strRevInfo.Left(2)==_T("CA"))
	{
       strRevInfo.Delete(0,3);

	   nIndex=strRevInfo.Find('*');
	   if(nIndex==-1) return;
       strNodeNum=strRevInfo.Left(nIndex);
       if(atoi(strNodeNum)<1||atoi(strNodeNum)>999)
		   return;
 	   //����Ϣ�������ʱ�������·�������ǰʱ��
	   //���ݷֻ����ҳ��绰����
	   CString Telephone;

	   nNode=atoi(strNodeNum);
	   strTemp.Format("%d",nNode);
	   m_fenjitable.m_strSort=_T("[Id]");
	   m_fenjitable.m_strFilter=_T("[Id] = "+strTemp); 
	   m_fenjitable.Requery();
	   if(!(m_fenjitable.IsEOF()&&m_fenjitable.IsBOF()))//��Ϊ��
	   {
		   strSendInfo="CA#";
           strSendInfo+=m_fenjitable.m_telephone;
           strSendInfo+="#";
		   nMonth=time.GetMonth();
		   strTemp.Format("%2.2d",nMonth);
		   strSendInfo+=strTemp;
           strSendInfo+="#";
		   nDay=time.GetDay();
		   strTemp.Format("%2.2d",nDay);
		   strSendInfo+=strTemp;
           strSendInfo+="#";
		   nHour=time.GetHour();
		   strTemp.Format("%2.2d",nHour);
		   strSendInfo+=strTemp;
           strSendInfo+="#";
		   nMinite=time.GetMinute();
		   strTemp.Format("%2.2d",nMinite);
		   strSendInfo+=strTemp;
           strSendInfo+="#";
		   nSecond=time.GetSecond();
		   strTemp.Format("%2.2d",nSecond);
		   strSendInfo+=strTemp;
           strSendInfo+="*";
		   m_strArray.Add(strSendInfo);
	   }
	}
    else if(strRevInfo.Left(2)==_T("CB"))//�����ֻ���
	{
       strRevInfo.Delete(0,3);

	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1) return;
       strTemp=strRevInfo.Left(nIndex);
	   if(strTemp.GetLength()<1||strTemp.GetLength()>13)
           return;
       strRevInfo.Delete(0,nIndex+1);

	   nIndex=strRevInfo.Find('*');
	   if(nIndex==-1) return;
       strNodeNum=strRevInfo.Left(nIndex);
       if(atoi(strNodeNum)<1||atoi(strNodeNum)>999)
		   return;
	   //���ݷֻ��绰�����޸Ķ�Ӧ�ķֻ���

	   m_fenjitable.m_strSort=_T("[Id]");
	   m_fenjitable.m_strFilter=_T("[telephone] = '"+strTemp+"'"); 
	   m_fenjitable.Requery();
	   if(!(m_fenjitable.IsEOF()&&m_fenjitable.IsBOF()))//��Ϊ��
	   {
           m_fenjitable.Edit();
           m_fenjitable.m_Id=atoi(strNodeNum);
           m_fenjitable.Update();
	   }
	}
    else if(strRevInfo.Left(2)==_T("CC"))
	{
       strRevInfo.Delete(0,3);
	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1) return;
       strNodeNum=strRevInfo.Left(nIndex);
	   if(atoi(strNodeNum)<1||atoi(strNodeNum)>999)
		   return;
       strRevInfo.Delete(0,nIndex+1);
	   nIndex=strRevInfo.Find('*');
	   if(nIndex==-1) return;
       strTemp=strRevInfo.Left(nIndex);//����ģʽ
	   if(atoi(strTemp)<1||atoi(strTemp)>2)
		   return;
	   m_fenjitable.m_strSort=_T("[Id]");
       m_fenjitable.m_strFilter=_T("[Id] = "+strNodeNum); 
       m_fenjitable.Requery();
	   if(!(m_fenjitable.IsEOF()&&m_fenjitable.IsBOF()))//��Ϊ��
	   {
		   m_fenjitable.Edit();
		   m_fenjitable.m_mode=atoi(strTemp);
		   m_fenjitable.Update();
	   }
	}
/*    else if(strRevInfo.Left(2)==_T("CD"))
	{
		//�ֻ���������
       strRevInfo.Delete(0,3);

	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1) return;
       strNodeNum=strRevInfo.Left(nIndex);// �ֻ���
       strRevInfo.Delete(0,nIndex+1);

	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1) return;
	   nAlarmtype=atoi(strRevInfo.Left(nIndex));//������
	   strRevInfo.Delete(0,nIndex+1);

	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1) return;
	   strTemp=strRevInfo.Left(nIndex);//��
	   strRevInfo.Delete(0,nIndex+1);
       nMonth=atoi(strTemp);

	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1) return;
	   strTemp=strRevInfo.Left(nIndex);//��
	   strRevInfo.Delete(0,nIndex+1);
       nDay=atoi(strTemp);

	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1) return;
	   strTemp=strRevInfo.Left(nIndex);//ʱ
	   strRevInfo.Delete(0,nIndex+1);
       nHour=atoi(strTemp);

	   nIndex=strRevInfo.Find('*');
	   if(nIndex==-1) return;
	   strTemp=strRevInfo.Left(nIndex);//��
	   strRevInfo.Delete(0,nIndex+1);
       nMinite=atoi(strTemp);

       nSecond=0;

       nYear=time.GetYear();
	   CTime alarmtime(nYear,nMonth,nDay,nHour,nMinite,nSecond);
	   strAlarmTime=alarmtime.Format("%Y-%m-%d %H:%M:%S");
       if(!fenjialarmTable.IsOpen())
          fenjialarmTable.Open();
       fenjialarmTable.AddNew();

       fenjialarmTable.m_fenjinumber=atoi(strNodeNum);
       fenjialarmTable.m_type=nAlarmtype;
       fenjialarmTable.m_alarmtime=alarmtime;
       fenjialarmTable.Update();
	   if(fenjialarmTable.IsOpen())
          fenjialarmTable.Close();

	   //���ݷֻ����ҳ�ά����Ա���绰��������strMaintenance��strTelephone������
	   if(!fenjitable.IsOpen())
		   fenjitable.Open();
	   nNode=atoi(strNodeNum);
	   strTemp.Format("%d",nNode);
	   fenjitable.m_strSort=_T("[Id]");
	   fenjitable.m_strFilter=_T("[Id] = "+strTemp); 
	   fenjitable.Requery();
	   if(!(fenjitable.IsEOF()&&fenjitable.IsBOF()))//��Ϊ��
	   {
           strFenjiName=fenjitable.m_Name;
		   for(i=0;i<6;i++)
		   {
              strMaintenance[i]=fenjitable.m_maintenance[i];
              strTelephone[i]=fenjitable.m_maintenancetel[i];
		   }
	   }
       else
	   {
		   strFenjiName="";
		   for(i=0;i<6;i++)
		   {
              strMaintenance[i]="";
              strTelephone[i]="";
		   }
	   }
	   if(fenjitable.IsOpen())
		   fenjitable.Close();
	   //�����Ļ�����GBK��
	   strSendInfo="BD#";
	   strSendInfo+=strFenjiName;
	   strSendInfo+=strNodeNum;
	   strSendInfo+="�ŷֻ�����";
	   strSendInfo+=strAlarmType[nAlarmtype-1];
	   strSendInfo+="�澯*";
	   m_strArray.Add(strSendInfo);
       //�����б�
	   strCableNum="00";//�ֻ�ʱ�����º�Ϊ0
	   strDistance="";
       InsertListItem(strNodeNum,
					  strCableNum,
					  strFenjiName,
					  strAlarmType[nAlarmtype-1],
					  strDistance,
					  strAlarmTime,
					  strMaintenance,
					  strTelephone);
	   	if(!m_bAlarm)
		{
			m_bAlarm=TRUE;
			PlaySound("Alarm.wav",NULL,SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NODEFAULT);
		}
       SetAlarmFlag(strNodeNum);
	}
*/
	else if(strRevInfo.Left(2)==_T("DA"))//�û��绰�����ϴ�
	{
       strRevInfo.Delete(0,3);

	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1) return;
       strNodeNum=strRevInfo.Left(nIndex);
	   if(atoi(strNodeNum)<1||atoi(strNodeNum)>999)
		   return;

       strRevInfo.Delete(0,nIndex);
	   for(i=0;i<6;i++)  strTelephone[i]="";
	   i=0;
	   do
	   {
		   strRevInfo.Delete(0,1);
		   nIndex=strRevInfo.Find('#');
		   if(nIndex==-1)
		      nIndex=strRevInfo.Find('*');
		   if(nIndex==-1)
			   return;
		   if(i>5) return;
		   strTelephone[i]=strRevInfo.Left(nIndex);
		   if(strTelephone[i].GetLength()<1||strTelephone[i].GetLength()>13)
			   return;
		   i++;
		   strRevInfo.Delete(0,nIndex);
	   }while(strRevInfo.Left(1)!="*");
	   nNode=atoi(strNodeNum);
	   strTemp.Format("%d",nNode);
	   m_fenjitable.m_strSort=_T("[Id]");
	   m_fenjitable.m_strFilter=_T("[Id] = "+strTemp); 
	   m_fenjitable.Requery();
	   if(!(m_fenjitable.IsEOF()&&m_fenjitable.IsBOF()))//��Ϊ��
	   {
           m_fenjitable.Edit();
		   for(i=0;i<6;i++)
              m_fenjitable.m_usertel[i]=strTelephone[i];
           m_fenjitable.Update();
	   }
	}
    else if(strRevInfo.Left(2)==_T("DB"))
	{
       strRevInfo.Delete(0,3);
	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1) return;
       strNodeNum=strRevInfo.Left(nIndex);
	   if(atoi(strNodeNum)<1||atoi(strNodeNum)>999)
		   return;

       strRevInfo.Delete(0,nIndex+1);
	   nIndex=strRevInfo.Find('*');
	   if(nIndex==-1) return;
       strTemp=strRevInfo.Left(nIndex);//���ϵ��
	   if(atoi(strTemp)<10||atoi(strTemp)>99)
		   return;
	   m_fenjitable.m_strSort=_T("[Id]");
       m_fenjitable.m_strFilter=_T("[Id] = "+strNodeNum); 
       m_fenjitable.Requery();
	   if(!(m_fenjitable.IsEOF()&&m_fenjitable.IsBOF()))//��Ϊ��
	   {
		   m_fenjitable.Edit();
		   m_fenjitable.m_zejuxisu=atoi(strTemp);
		   m_fenjitable.Update();
	   }
	}
	else if(strRevInfo.Left(2)==_T("DC"))//�ֻ��绰�����ϴ�
	{
       strRevInfo.Delete(0,3);

	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1) return;
       strNodeNum=strRevInfo.Left(nIndex);
	   if(atoi(strNodeNum)<1||atoi(strNodeNum)>999)
		   return;

       strRevInfo.Delete(0,nIndex+1);

	   nIndex=strRevInfo.Find('*');
	   if(nIndex==-1) return;
       strTelephone[0]=strRevInfo.Left(nIndex);
	   if(strTelephone[0].GetLength()<1||strTelephone[0].GetLength()>13)
		   return;
       strRevInfo.Delete(0,nIndex+1);

	   nNode=atoi(strNodeNum);
	   strTemp.Format("%d",nNode);
	   m_fenjitable.m_strSort=_T("[Id]");
	   m_fenjitable.m_strFilter=_T("[Id] = "+strTemp); 
	   m_fenjitable.Requery();
	   if(!(m_fenjitable.IsEOF()&&m_fenjitable.IsBOF()))//��Ϊ��
	   {
		   m_fenjitable.Edit();
           m_fenjitable.m_telephone=strTelephone[0];
		   m_fenjitable.Update();
	   }
	}
/*	else if(strRevInfo.Left(2)==_T("DD"))
	{
		//�ֻ���������
       strRevInfo.Delete(0,3);

	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1) return;
       strNodeNum=strRevInfo.Left(nIndex);// �ֻ���
       strRevInfo.Delete(0,nIndex+1);

	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1) return;
	   nAlarmtype=atoi(strRevInfo.Left(nIndex))+8;//������
	   strRevInfo.Delete(0,nIndex+1);

	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1) return;
	   strTemp=strRevInfo.Left(nIndex);//��
	   strRevInfo.Delete(0,nIndex+1);
       nMonth=atoi(strTemp);

	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1) return;
	   strTemp=strRevInfo.Left(nIndex);//��
	   strRevInfo.Delete(0,nIndex+1);
       nDay=atoi(strTemp);

	   nIndex=strRevInfo.Find('#');
	   if(nIndex==-1) return;
	   strTemp=strRevInfo.Left(nIndex);//ʱ
	   strRevInfo.Delete(0,nIndex+1);
       nHour=atoi(strTemp);

	   nIndex=strRevInfo.Find('*');
	   if(nIndex==-1) return;
	   strTemp=strRevInfo.Left(nIndex);//��
	   strRevInfo.Delete(0,nIndex+1);
       nMinite=atoi(strTemp);

       nSecond=0;

       nYear=time.GetYear();
	   CTime alarmtime(nYear,nMonth,nDay,nHour,nMinite,nSecond);
	   strAlarmTime=alarmtime.Format("%Y-%m-%d %H:%M:%S");

       m_fenjialarmTable.AddNew();
       m_fenjialarmTable.m_fenjinumber=atoi(strNodeNum);
       m_fenjialarmTable.m_type=nAlarmtype-8;
       m_fenjialarmTable.m_alarmtime=alarmtime;
       m_fenjialarmTable.Update();

	   //���ݷֻ����ҳ�ά����Ա���绰��������strMaintenance��strTelephone������
	   nNode=atoi(strNodeNum);
	   strTemp.Format("%d",nNode);
	   m_fenjitable.m_strSort=_T("[Id]");
	   m_fenjitable.m_strFilter=_T("[Id] = "+strTemp); 
	   m_fenjitable.Requery();
	   if(!(m_fenjitable.IsEOF()&&m_fenjitable.IsBOF()))//��Ϊ��
	   {
           strFenjiName=m_fenjitable.m_Name;
		   for(i=0;i<6;i++)
		   {
              strMaintenance[i]=m_fenjitable.m_maintenance[i];
              strTelephone[i]=m_fenjitable.m_maintenancetel[i];
		   }
	   }
       else
	   {
		   strFenjiName="";
		   for(i=0;i<6;i++)
		   {
              strMaintenance[i]="";
              strTelephone[i]="";
		   }
	   }
	   //�����Ļ�����GBK��
	   strSendInfo="BD#";
	   strSendInfo+=strFenjiName;
	   strSendInfo+=strNodeNum;
	   strSendInfo+="�ŷֻ�����";
	   strSendInfo+=strAlarmType[nAlarmtype-1];
	   strSendInfo+="�澯*";
	   m_strArray.Add(strSendInfo);
       //�����б�
	   strCableNum="00";//�ֻ�ʱ�����º�Ϊ0
	   strDistance="";
       InsertListItem(strNodeNum,
					  strCableNum,
					  strFenjiName,
					  strAlarmType[nAlarmtype-1],
					  strDistance,
					  strAlarmTime,
					  strMaintenance,
					  strTelephone);
	   	if(!m_bAlarm)
		{
			m_bAlarm=TRUE;
			PlaySound("Alarm.wav",NULL,SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NODEFAULT);
		}
       SetAlarmFlag(strNodeNum);
	}
	*/

}
void CRemotemonitorView::maptoASC(unsigned char* pbuff,int len)
{
    while(len--)
	{
		*pbuff=dtmftoasc[*pbuff];
		pbuff++;
	}
}
void CRemotemonitorView::Process()
{
	int   nIndex0,nIndex1;
    int i;
    unsigned char cRevInfo[1024];
	CString strRevInfo;
	if(bPoll)
		return;
	bPoll=TRUE;	
	
    if(InDataLen<7)
    {
		bPoll=FALSE;
		return;
	}
LL:
    nIndex0=0;
    nIndex1=0;
	if(!SeekFrameHead(nIndex0)) //���ͬ��ͷ,û�ҵ�
	{
		InDataLen=0;	          // ��ȥȫ������
		bPoll=FALSE;
		return;
	}
	if(!SeekFrameEnd(nIndex0,nIndex1))    // ����������־��û�ҵ�
	{
		InDataLen -= nIndex0;             //�Ѱ�ͷǰ�����ò���ȥ��
        for(i=0;i<InDataLen;i++)
          InDataBuf[i]=InDataBuf[nIndex0+i];
	    bPoll=FALSE;	
		return;
	}

    nIndex1=DeleteDLE(nIndex0,nIndex1);  //������ת���DLEȥ��
	if(m_nLinkState==IDLE)
	{
        if(InDataBuf[nIndex0+3]==0x01)//�յ�LR��
		{
		   CMainFrame* pWnd;
		   pWnd=(CMainFrame*)AfxGetMainWnd();
		   pWnd->ChangeLinkState(1);
           m_nLinkState=READY;
		   m_bHasAck=TRUE;
		   SendLA(1);
		}
		else if(InDataBuf[nIndex0+3]==0x02)//�յ�Ӧ���
		{
			if(InDataBuf[nIndex0+4]==0x01)//�Է��յ��˱�����LR,�������ݴ���״̬
			{
		        CMainFrame* pWnd;
		        pWnd=(CMainFrame*)AfxGetMainWnd();
		        pWnd->ChangeLinkState(1);
                m_nLinkState=READY;
		        m_bHasAck=TRUE;
			}
			else
			{
				//�Է���ʱ��Ӧ�յ�������LT��������Ӧ��������������������������
			}			
		}
		else if(InDataBuf[nIndex0+3]==0x04)//�յ�LT��
		{
			//��ʱ�յ�LT��Ҳ��������
		}
		else
		{
			//��ʱ�յ����������ǲ�Ӧ���ֵģ��ʲ����κδ���
		}
	}
	else//READY״̬
	{   //�����յ�LR��LA��LT
        if(InDataBuf[nIndex0+3]==0x01)//�յ�LR��
		{
           //m_nLinkState=READY;�Ѵ���READY״̬
		   m_bHasAck=TRUE;
//		   m_bBusy=FALSE;
		   SendLA(1);
		}
		else if(InDataBuf[nIndex0+3]==0x02)//�յ�LA
		{
			if(InDataBuf[nIndex0+4]==0x01)//�Է��յ�������ʱ������LR
			{
		       m_bHasAck=TRUE;//˵����·�ĳ�ͨ��
			}
			else if(InDataBuf[nIndex0+4]==0x02)//�Է���������ȷ
			{
				if(m_strArray.GetSize()>0)//���ѷ��͵���Ϣȥ��
				{
					m_strArray.RemoveAt(0);
					m_nCounter=0;
		            m_bBusy=FALSE;
					if(m_uReSendTimer)
                       KillTimer(m_uReSendTimer);
				}
			}
			else if(InDataBuf[nIndex0+4]==0x04)//�Է��յ������ݲ���ȷ
			{
		//	   MessageBox("�Է������ݲ���ȷ!","��ʾ��Ϣ");
		//	   m_strArray.RemoveAll();
			}
			else
			{
               //�ڴ�״̬�ǲ�Ӧ���ֵģ��ʲ�������
			}
		}
		else if(InDataBuf[nIndex0+3]==0x04)//�յ�LT��
		{
			if(crc(InDataBuf+nIndex0+3,nIndex1-(nIndex0+3))==0x00)//��LT�������ݽ���У��
			{
		        SendLA(2);//�յ���ȷ��LT
                //����Ϣ��¼�����ݿ⣬����ʾ����
                memset(cRevInfo,0,1024);
                memcpy(cRevInfo,&InDataBuf[nIndex0+5],nIndex1-(nIndex0+6));
                maptoASC(cRevInfo,nIndex1-(nIndex0+6));
				strRevInfo=cRevInfo;
				ProcessRevInfo(strRevInfo);
			}
			else
			{
		        SendLA(3);//�յ�����ȷ��LT��������Ӧ��
			}
		}
		else
		{
			//��Ӧ������������������ֲ�����
		}
	}
    //һ���������Ӧ���ð�ȥ��
	InDataLen -= (nIndex1+2);             //����һ����֮ǰ������ȥ��
    for(i=0;i<InDataLen;i++)
	   InDataBuf[i]=InDataBuf[nIndex1+2+i];
	if(InDataLen>=7)
		goto LL;
	bPoll=FALSE;
}

void CRemotemonitorView::flash()
{
	int i,n;
    CDC* pDC = GetDC();
	OnPrepareDC(pDC);//����DC�Ĺ������ԣ���ScollView�Ĺ����й�
///	pDC->DPtoLP(&point);//ת����ǰ��Ϊ�߼�λ������
	CRemotemonitorDoc* pDoc = GetDocument();

	n=pDoc->m_aObjects.GetSize();
	if(n>0)
	{
		for(i=0; i<n; i++)
		{
			CDrawObject* pObject = pDoc->m_aObjects[i];
			if(pObject!=NULL)
			{
				if(pObject->m_bAlarm==TRUE)
				{
					pObject->FlashDraw(pDC);
				}
			}
		}
	}
	ReleaseDC(pDC);//�ͷ�DC��Դ
}

void CRemotemonitorView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	switch(nIDEvent)
	{
	case PROBETIMER://ÿ��10�뷢��һ��LR��̽����·�Ƿ��ڳ�ͨ״̬
		 if(!m_bHasAck)//û���յ�Ӧ��
		 {
			if(m_nLinkState!=IDLE)//���ԭΪREADY����Ҫ�ı�״̬
			{
				CMainFrame* pWnd;
				pWnd=(CMainFrame*)AfxGetMainWnd();
				pWnd->ChangeLinkState(0);
				m_nLinkState=IDLE;
			}
		 }
         SendLR();
		 m_bHasAck=FALSE;
		 break;
	case REVTIMER:
		 Process();
		 break;
    case SNDTIMER:
		if(m_nLinkState==READY&&m_strArray.GetSize()>0&&m_bBusy==FALSE)
		{
		//����ַ������������д�����Ϣ������LT
		   SendLT();
		   m_bBusy=TRUE;
           m_nCounter=0;
		   m_uReSendTimer=SetTimer(RESENDTIME,m_ReSendT,NULL);
		}
		 break;
    case RESENDTIME:
		if(m_nCounter<3)
		{
		   SendLT();
           m_nCounter++;
		}
		else
		{
			m_strArray.RemoveAt(0);
			if(m_uReSendTimer)
               KillTimer(m_uReSendTimer);
			m_nCounter=0;
		    m_bBusy=FALSE;
		}
        break;
	case FLASHTIMER:
		flash();
		break;
	case DISPLAYTIME:
	    CMainFrame* pWnd;
	    pWnd=(CMainFrame*)AfxGetMainWnd();
        pWnd->ChangeTime();
		break;
	default:
		break;
	}		
	CScrollView::OnTimer(nIDEvent);
}
#include "SendAAdlg.h"
void CRemotemonitorView::OnSendaa() 
{
	// TODO: Add your command handler code here
	CString str;
/*	if(m_nLinkState==IDLE)
	{
		MessageBox("��·��δ����,���ܷ�����Ϣ!","��ʾ��Ϣ");
		return;
	}*/
	CSendAAdlg  dlg;
	if(dlg.DoModal()==IDOK)
	{
		str="AA#";
		str+=dlg.m_strInfo;
		str+="*";

		m_strArray.Add(str);
	}
}

#include "SendABdlg.h"
void CRemotemonitorView::OnSendab() 
{
	// TODO: Add your command handler code here
	CString str;
	if(m_nLinkState==IDLE)
	{
		MessageBox("��·��δ����,���ܷ�����Ϣ!","��ʾ��Ϣ");
		return;
	}

    CSendABdlg	dlg;
	dlg.m_flag=0;
    if(dlg.DoModal()==IDOK)
	{
		str="AB#";
		str+=dlg.m_strInfo;
		str+="*";
		m_strArray.Add(str);
	}
}
#include "SendDCDlg.h"
void CRemotemonitorView::OnSenddc() 
{
	// TODO: Add your command handler code here
	CString str;

	if(m_nLinkState==IDLE)
	{
		MessageBox("��·��δ����,���ܷ�����Ϣ!","��ʾ��Ϣ");
		return;
	}

    CSendDCDlg	dlg;
    if(dlg.DoModal()==IDOK)
	{
		str="DC#";
		str+=dlg.m_strInfo;
		str+="*";
		m_strArray.Add(str);
	}	
}
void CRemotemonitorView::OnSendca() 
{
	// TODO: Add your command handler code here
	CString str;
	if(m_nLinkState==IDLE)
	{
		MessageBox("��·��δ����,���ܷ�����Ϣ!","��ʾ��Ϣ");
		return;
	}

    CSendABdlg	dlg;
	dlg.m_flag=1;
    if(dlg.DoModal()==IDOK)
	{
		str="CA#";
		str+=dlg.m_strInfo;
		str+="*";
		m_strArray.Add(str);
	}
}
#include "SendCCdlg.h"
void CRemotemonitorView::OnSendcc() 
{
	// TODO: Add your command handler code here
	CString str;
    CString strcoff;

	if(m_nLinkState==IDLE)
	{
		MessageBox("��·��δ����,���ܷ�����Ϣ!","��ʾ��Ϣ");
		return;
	}

    CSendCCdlg	dlg;
    if(dlg.DoModal()==IDOK) 
	{
		switch(dlg.m_radio1)
		{
		case 0:
			str="AC#";
			break;
		case 1:
			str="AD#";
			break;
		case 2:
			str="BA#";
			break;
		case 3:
			str="BB#";
			break;
		case 4:
			str="BC#";
			break;
		case 5:
			str="CC#";
			break;
		case 6:
			str="DB#";
			break;
		default:
			break;
		}
		str+=dlg.m_strInfo;
		str+="*";
		m_strArray.Add(str);

	}			
}

#include "SendCDdlg.h"
void CRemotemonitorView::OnSendcd() 
{
	// TODO: Add your command handler code here
	CString str;
    CString strcoff;
	if(m_nLinkState==IDLE)
	{
		MessageBox("��·��δ����,���ܷ�����Ϣ!","��ʾ��Ϣ");
		return;
	}
    CSendCDdlg	dlg;
    if(dlg.DoModal()==IDOK) 
	{
		str="CD#";
		str+=dlg.m_strInfo;
		str+="*";
		m_strArray.Add(str);
	}			
}

#include "SendDAdlg.h"
void CRemotemonitorView::OnSendda() 
{
	// TODO: Add your command handler code here
	CString str;
    CString strcoff;
	if(m_nLinkState==IDLE)
	{
		MessageBox("��·��δ����,���ܷ�����Ϣ!","��ʾ��Ϣ");
		return;
	}
    CSendDAdlg	dlg;
    if(dlg.DoModal()==IDOK) 
	{
		str="DA#";
		str+=dlg.m_strInfo;
		str+="*";
		m_strArray.Add(str);
	}		
}
#include "SendCBdlg.h"
void CRemotemonitorView::OnSendcb() 
{
	// TODO: Add your command handler code here
	
	CString str;
    CString strcoff;
	if(m_nLinkState==IDLE)
	{
		MessageBox("��·��δ����,���ܷ�����Ϣ!","��ʾ��Ϣ");
		return;
	}
    CSendCBdlg	dlg;
    if(dlg.DoModal()==IDOK) 
	{
		str="CB#";
		str+=dlg.m_strInfo;
		str+="*";
		m_strArray.Add(str);
	}		
}
void CRemotemonitorView::InsertListItem(CString strNodeNum,
                                        CString strFenjiName,
										CString strCableNum,
										CString strNodeName,
										CString strTroubleType,
										CString strDistance,
										CString strAlarmTime,
										CString strMaintenance[],
										CString strTelephone[]
										)
{
    int ntotalItem;
//	int nIndex;
	CString strItem,strSubItem;
	CListCtrl*   pListCtrl;
	CMainFrame* pWnd;

	pWnd=(CMainFrame*)AfxGetMainWnd();
	pListCtrl = &pWnd->m_wndList;
	pWnd->ViewListbar();

	ntotalItem = pListCtrl->GetItemCount();
	if(ntotalItem>100)
	{
		OnDeletelistitem();
        ntotalItem=0;
	}
/*	for(nIndex=0;nIndex<ntotalItem;nIndex++)
	{
	   strItem=pListCtrl->GetItemText(nIndex,0);
       strSubItem=pListCtrl->GetItemText(nIndex,1);
	   if(strItem==strNodeNum&&strSubItem==strCableNum) break;
	}
	//�����Ӧ�����Ѵ��ڣ�ֻ���޸ĵ�ǰ����������Ϣ
	if(nIndex<ntotalItem)
	{
		pListCtrl->SetItem(nIndex,2,LVIF_TEXT,_T(strNodeName),0,0,0,NULL);
		pListCtrl->SetItem(nIndex,3,LVIF_TEXT,_T(strTroubleType),0,0,0,NULL);
		pListCtrl->SetItem(nIndex,4,LVIF_TEXT,_T(strDistance),0,0,0,NULL);
		pListCtrl->SetItem(nIndex,5,LVIF_TEXT,_T(strAlarmTime),0,0,0,NULL);
		pListCtrl->SetItem(nIndex,6,LVIF_TEXT,_T(strMaintenance[0]),0,0,0,NULL);
	}

	//�Թ���Ӧ������ڣ������������
    else	*/
	{
		pListCtrl->InsertItem(ntotalItem,strNodeNum);
		pListCtrl->SetItem(ntotalItem,1,LVIF_TEXT,_T(strFenjiName),0,0,0,NULL);
		pListCtrl->SetItem(ntotalItem,2,LVIF_TEXT,_T(strCableNum),0,0,0,NULL);
		pListCtrl->SetItem(ntotalItem,3,LVIF_TEXT,_T(strNodeName),0,0,0,NULL);
		pListCtrl->SetItem(ntotalItem,4,LVIF_TEXT,_T(strTroubleType),0,0,0,NULL);
		pListCtrl->SetItem(ntotalItem,5,LVIF_TEXT,_T(strDistance),0,0,0,NULL);
		pListCtrl->SetItem(ntotalItem,6,LVIF_TEXT,_T(strAlarmTime),0,0,0,NULL);
		pListCtrl->SetItem(ntotalItem,7,LVIF_TEXT,_T(strMaintenance[0]),0,0,0,NULL);
		pListCtrl->SetItem(ntotalItem,8,LVIF_TEXT,_T(strTelephone[0]),0,0,0,NULL);
		pListCtrl->SetItem(ntotalItem,9,LVIF_TEXT,_T(strMaintenance[1]),0,0,0,NULL);
		pListCtrl->SetItem(ntotalItem,10,LVIF_TEXT,_T(strTelephone[1]),0,0,0,NULL);
		pListCtrl->SetItem(ntotalItem,11,LVIF_TEXT,_T(strMaintenance[2]),0,0,0,NULL);
		pListCtrl->SetItem(ntotalItem,12,LVIF_TEXT,_T(strTelephone[2]),0,0,0,NULL);
		pListCtrl->SetItem(ntotalItem,13,LVIF_TEXT,_T(strMaintenance[3]),0,0,0,NULL);
		pListCtrl->SetItem(ntotalItem,14,LVIF_TEXT,_T(strTelephone[3]),0,0,0,NULL);
		pListCtrl->SetItem(ntotalItem,15,LVIF_TEXT,_T(strMaintenance[4]),0,0,0,NULL);
		pListCtrl->SetItem(ntotalItem,16,LVIF_TEXT,_T(strTelephone[4]),0,0,0,NULL);
		pListCtrl->SetItem(ntotalItem,17,LVIF_TEXT,_T(strMaintenance[5]),0,0,0,NULL);
		pListCtrl->SetItem(ntotalItem,18,LVIF_TEXT,_T(strTelephone[5]),0,0,0,NULL);
	}
}

void CRemotemonitorView::OnDeletelistitem() 
{
	// TODO: Add your command handler code here
	CListCtrl*   pListCtrl;
	CMainFrame* pWnd;
    int ntotalItem;

	pWnd=(CMainFrame*)AfxGetMainWnd();
	pListCtrl = &pWnd->m_wndList;
	ntotalItem = pListCtrl->GetItemCount();
	if(ntotalItem==0) return;
	pListCtrl->DeleteAllItems();	
}
#include "FenjiNumDlg.h"
#include "CableNumDlg.h"
void CRemotemonitorView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_nCurTool==ID_SELECT)
	{
       if(m_usrCurrentObject &&m_nMoveMode)
	   {
		   if(m_usrCurrentObject->m_nStyle==ID_DRAWCIRCLE)//�Ƿֻ��ڵ�
		   {
               CFenjiNumDlg   dlg(m_usrCurrentObject);
			   if(dlg.DoModal()==IDOK)
			   {
				   CRemotemonitorDoc* pDoc = GetDocument();
				   pDoc->SetModifiedFlag(TRUE);
				   Invalidate();//ˢ����ͼ
			   }
		   }
		   else//����
		   {
               CCableNumDlg   dlg(m_usrCurrentObject);
			   if(dlg.DoModal()==IDOK)
			   {
				   CRemotemonitorDoc* pDoc = GetDocument();
				   pDoc->SetModifiedFlag(TRUE);
				   Invalidate();//ˢ����ͼ
			   }
		   }
	   }
	}
	CScrollView::OnLButtonDblClk(nFlags, point);
}
#include "DutyTable.h"

void CRemotemonitorView::OnDestroy() 
{
	CScrollView::OnDestroy();
	
	// TODO: Add your message handler code here
   if(m_uRevTimer)
	   KillTimer(m_uRevTimer);
    if(m_uSendTimer)
	   KillTimer(m_uSendTimer);
    if(m_uFlashTimer)
	   KillTimer(m_uFlashTimer);
    if(m_uProbeTimer)
	   KillTimer(m_uProbeTimer);
    if(m_uDisplayTimer)
		KillTimer(m_uDisplayTimer);

	CDutyTable dutytable;

	CString strFilter = _T("[Name] = '");
	strFilter +=  strCurrentUser;
    strFilter+="'";
    strFilter+=" AND ";
    strFilter+=_T("[dutytime] = #");
    strFilter+=strDutyTime;
    strFilter+="#";
//	dutytable.m_strSort=_T("[Id]");
	dutytable.m_strFilter = _T(strFilter);
 //   m_stafftable.Requery();


	dutytable.Open();
	if(!dutytable.IsBOF())
	{
		dutytable.Edit();
		dutytable.m_lefttime=CTime::GetCurrentTime();
		dutytable.Update();
	}
	if(dutytable.IsOpen())
		dutytable.Close();

//	MessageBox("1");
}



void CRemotemonitorView::OnClearalarm() 
{
	// TODO: Add your command handler code here
	CRemotemonitorDoc* pDoc = GetDocument();
    CDrawObject* pObject=NULL;
    unsigned char SendBuf[1024];
	if(m_bAlarm==TRUE)
	{
		memset(SendBuf,0,sizeof(SendBuf));
		SendBuf[0]=0x16;
		SendBuf[1]=0x10;
		SendBuf[2]=0x02;
		SendBuf[3]=0x06;
		SendBuf[4]=0x00;
		SendBuf[5]=0x10;
		SendBuf[6]=0x03;
		WriteCommBlock(&Comminfo, (LPSTR)SendBuf,7);
	}


	int i;

	for(i=0;i<pDoc->m_aObjects.GetSize(); i++)
	{
		pObject = pDoc->m_aObjects[i];
		if(pObject&&pObject->m_bAlarm)
			pObject->m_bAlarm=FALSE;
	}
	PlaySound(NULL,NULL,SND_FILENAME);
	m_bAlarm=FALSE;
	Invalidate();
}


#include "DutyRecordDlg.h"
void CRemotemonitorView::OnDutyrecord() 
{
	// TODO: Add your command handler code here
    CDutyRecordDlg dlg;
	dlg.DoModal();
}
#include "JiaoBanDlg.h"
#include "FenjiPropertySheet.h"
void CRemotemonitorView::OnJiaoban() 
{
	// TODO: Add your command handler code here
	CJiaoBanDlg dlg;
	dlg.DoModal();
}


#include "FenjiCableSet.h"
void CRemotemonitorView::OnFenjicableset() 
{
	// TODO: Add your command handler code here
	CFenjiCableSet  dlg;
	dlg.DoModal();
}
#include "UserSetDlg.h"
void CRemotemonitorView::OnUserset() 
{
	// TODO: Add your command handler code here
	CUserSetDlg dlg;
	dlg.DoModal();
}
#include "ViewDutyInfo.h"
void CRemotemonitorView::OnViewdutyrecord() 
{
	// TODO: Add your command handler code here
	CViewDutyInfo dlg;
	dlg.DoModal();
}
#include "ViewFenjiCableInfo.h"
void CRemotemonitorView::OnViewfenjicableinfo() 
{
	// TODO: Add your command handler code here
	CViewFenjiCableInfo dlg;
	dlg.DoModal();

}

#include "ViewCableAlarmInfo.h"
void CRemotemonitorView::OnViewcablealarminfo() 
{
	// TODO: Add your command handler code here
	CViewCableAlarmInfo dlg;
	dlg.DoModal();

}

void CRemotemonitorView::OnUpdateUserset(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(bRight[0]);
}

void CRemotemonitorView::OnUpdateFenjicableset(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(bRight[1]);
}




#include "SetCenterTeldlg.h"
void CRemotemonitorView::OnSetcentertel() 
{
	// TODO: Add your command handler code here
	CSetCenterTeldlg dlg;
	dlg.DoModal();
}

void CRemotemonitorView::OnUpdateClearalarm(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_bAlarm);
}
