// ViewFenjiCableInfo.cpp : implementation file
//

#include "stdafx.h"
#include "remotemonitor.h"
#include "ViewFenjiCableInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewFenjiCableInfo dialog


CViewFenjiCableInfo::CViewFenjiCableInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CViewFenjiCableInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CViewFenjiCableInfo)
	m_edit1 = 0;
	m_edit2 = 0;
	m_edit3 = 0;
	m_edit4 = 0;
	//}}AFX_DATA_INIT
}


void CViewFenjiCableInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewFenjiCableInfo)
	DDX_Control(pDX, IDC_MODE2, m_mode2);
	DDX_Control(pDX, IDC_MODE1, m_mode1);
	DDX_Control(pDX, IDC_CABLELIST, m_list2);
	DDX_Control(pDX, IDC_FENJILIST, m_list1);
	DDX_Text(pDX, IDC_EDIT1, m_edit1);
	DDV_MinMaxInt(pDX, m_edit1, 0, 999);
	DDX_Text(pDX, IDC_EDIT2, m_edit2);
	DDV_MinMaxInt(pDX, m_edit2, 0, 999);
	DDX_Text(pDX, IDC_EDIT3, m_edit3);
	DDV_MinMaxInt(pDX, m_edit3, 0, 999);
	DDX_Text(pDX, IDC_EDIT4, m_edit4);
	DDV_MinMaxInt(pDX, m_edit4, 0, 999);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CViewFenjiCableInfo, CDialog)
	//{{AFX_MSG_MAP(CViewFenjiCableInfo)
	ON_BN_CLICKED(IDC_PROBE1, OnProbe1)
	ON_BN_CLICKED(IDC_PROBE2, OnProbe2)
	ON_CBN_SELCHANGE(IDC_MODE1, OnSelchangeMode1)
	ON_CBN_SELCHANGE(IDC_MODE2, OnSelchangeMode2)
	ON_BN_CLICKED(IDC_EXPORT1, OnExport1)
	ON_BN_CLICKED(IDC_EXPORT2, OnExport2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewFenjiCableInfo message handlers

BOOL CViewFenjiCableInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString str;

    GetDlgItem(IDC_MYSTATIC1)->GetWindowPlacement(&m_mystaticwnd[0]);
    GetDlgItem(IDC_MYSTATIC2)->GetWindowPlacement(&m_mystaticwnd[1]);
    GetDlgItem(IDC_MYSTATIC3)->GetWindowPlacement(&m_mystaticwnd[2]);
    GetDlgItem(IDC_MYSTATIC4)->GetWindowPlacement(&m_mystaticwnd[3]);
    GetDlgItem(IDC_EDIT1)->GetWindowPlacement(&m_editwnd[0]);
    GetDlgItem(IDC_EDIT2)->GetWindowPlacement(&m_editwnd[1]);
    GetDlgItem(IDC_EDIT3)->GetWindowPlacement(&m_editwnd[2]);
    GetDlgItem(IDC_EDIT4)->GetWindowPlacement(&m_editwnd[3]);

	WINDOWPLACEMENT wndplace;
	wndplace.rcNormalPosition.left	    = 2000;
	wndplace.rcNormalPosition.right	    = 2000;
	wndplace.rcNormalPosition.top		= 2000;
	wndplace.rcNormalPosition.bottom	= 2000;
    GetDlgItem(IDC_MYSTATIC1)->SetWindowPlacement(&wndplace);
    GetDlgItem(IDC_MYSTATIC2)->SetWindowPlacement(&wndplace);
    GetDlgItem(IDC_MYSTATIC3)->SetWindowPlacement(&wndplace);
    GetDlgItem(IDC_MYSTATIC4)->SetWindowPlacement(&wndplace);
    GetDlgItem(IDC_EDIT1)->SetWindowPlacement(&wndplace);
    GetDlgItem(IDC_EDIT2)->SetWindowPlacement(&wndplace);
    GetDlgItem(IDC_EDIT3)->SetWindowPlacement(&wndplace);
    GetDlgItem(IDC_EDIT4)->SetWindowPlacement(&wndplace);

	m_mode1.AddString("ȫ����¼");
	m_mode1.AddString("���ֻ���");
    m_mode1.SetCurSel(0);
	m_mode2.AddString("ȫ����¼");
	m_mode2.AddString("���ֻ���");
    m_mode2.SetCurSel(0);
    GetDlgItem(IDC_PROBE1)->EnableWindow(FALSE);
    GetDlgItem(IDC_PROBE2)->EnableWindow(FALSE);

	m_list1.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_list2.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	m_list1.InsertColumn( 0, "�ֻ���", LVCFMT_LEFT,60, 0 );
    m_list1.InsertColumn( 1, "�ֻ���", LVCFMT_LEFT,70, 0 );
    m_list1.InsertColumn( 2, "�ֻ��绰", LVCFMT_LEFT,100, 0 );
	m_list1.InsertColumn( 3, "�û�1", LVCFMT_LEFT,70, 0 );
    m_list1.InsertColumn( 4, "�û��绰1", LVCFMT_LEFT,100, 0 );
	m_list1.InsertColumn( 5, "�û�2", LVCFMT_LEFT,70, 0 );
    m_list1.InsertColumn( 6, "�û��绰2", LVCFMT_LEFT,100, 0 );
	m_list1.InsertColumn( 7, "�û�3", LVCFMT_LEFT,70, 0 );
    m_list1.InsertColumn( 8, "�û��绰3", LVCFMT_LEFT,100, 0 );
	m_list1.InsertColumn( 9, "�û�4", LVCFMT_LEFT,70, 0 );
    m_list1.InsertColumn( 10, "�û��绰4", LVCFMT_LEFT,100, 0 );
	m_list1.InsertColumn( 11, "�û�5", LVCFMT_LEFT,70, 0 );
    m_list1.InsertColumn( 12, "�û��绰5", LVCFMT_LEFT,100, 0 );
	m_list1.InsertColumn( 13, "�û�6", LVCFMT_LEFT,70, 0 );
    m_list1.InsertColumn( 14, "�û��绰6", LVCFMT_LEFT,100, 0 );
	m_list1.InsertColumn( 15, "ά��1", LVCFMT_LEFT,70,0);
    m_list1.InsertColumn( 16, "ά���绰1",   LVCFMT_LEFT,100,0);
	m_list1.InsertColumn( 17, "ά��2", LVCFMT_LEFT,70,0);
    m_list1.InsertColumn( 18, "ά���绰2",   LVCFMT_LEFT,100,0);
	m_list1.InsertColumn( 19, "ά��3", LVCFMT_LEFT,70,0);
    m_list1.InsertColumn( 20, "ά���绰3",   LVCFMT_LEFT,100,0);
	m_list1.InsertColumn( 21, "ά��4", LVCFMT_LEFT,70,0);
    m_list1.InsertColumn( 22, "ά���绰4",   LVCFMT_LEFT,100,0);
	m_list1.InsertColumn( 23, "ά��5", LVCFMT_LEFT,70,0);
    m_list1.InsertColumn( 24, "ά���绰5",   LVCFMT_LEFT,100,0);
	m_list1.InsertColumn( 25, "ά��6", LVCFMT_LEFT,70,0);
    m_list1.InsertColumn( 26, "ά���绰6",   LVCFMT_LEFT,100,0);
    m_list1.InsertColumn( 27, "��Ӧ��ȴ�ʱ��",   LVCFMT_LEFT,100,0);
    m_list1.InsertColumn( 28, "������",   LVCFMT_LEFT,100,0);
    m_list1.InsertColumn( 29, "ѭ���������",   LVCFMT_LEFT,100,0);
    m_list1.InsertColumn( 30, "�����ȴ�ʱ��",   LVCFMT_LEFT,100,0);
    m_list1.InsertColumn( 31, "��ѹת�۵�",   LVCFMT_LEFT,100,0);
    m_list1.InsertColumn( 32, "����ģʽ",   LVCFMT_LEFT,100,0);
    m_list1.InsertColumn( 33, "���ϵ��",   LVCFMT_LEFT,100,0);

	m_list2.InsertColumn( 0, "�ֻ���", LVCFMT_LEFT,60, 0 );
    m_list2.InsertColumn( 1, "���º�", LVCFMT_LEFT,60, 0 );
    m_list2.InsertColumn( 2, "�������", LVCFMT_LEFT,100, 0 );
	m_list2.InsertColumn( 3, "�����յ�", LVCFMT_LEFT,100, 0 );
    m_list2.InsertColumn( 4, "��������", LVCFMT_LEFT,100, 0 );


	m_table1.m_strSort=_T("[Id]");
	if(!m_table1.IsOpen())
       m_table1.Open();
    InSertList1();

	m_table2.m_strSort=_T("[Id]");
	if(!m_table2.IsOpen())
       m_table2.Open();
	InSertList2();

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CViewFenjiCableInfo::InSertList1()
{
	int i,ntotalItem;
	CString str;

	while (!m_table1.IsEOF())
	{
		ntotalItem = m_list1.GetItemCount();
		str.Format("%3.3d",m_table1.m_Id);
		m_list1.InsertItem(ntotalItem,str);
		m_list1.SetItem(ntotalItem,1,LVIF_TEXT,_T(m_table1.m_Name),0,0,0,NULL);
		m_list1.SetItem(ntotalItem,2,LVIF_TEXT,_T(m_table1.m_telephone),0,0,0,NULL);
		for(i=0;i<6;i++)
		{
			m_list1.SetItem(ntotalItem,i*2+3,LVIF_TEXT,_T(m_table1.m_username[i]),0,0,0,NULL);
			m_list1.SetItem(ntotalItem,i*2+4,LVIF_TEXT,_T(m_table1.m_usertel[i]),0,0,0,NULL);
			m_list1.SetItem(ntotalItem,i*2+15,LVIF_TEXT,_T(m_table1.m_maintenance[i]),0,0,0,NULL);
			m_list1.SetItem(ntotalItem,i*2+16,LVIF_TEXT,_T(m_table1.m_maintenancetel[i]),0,0,0,NULL);
		}
		str.Format("%d",m_table1.m_ackwaittime);
		m_list1.SetItem(ntotalItem,27,LVIF_TEXT,_T(str),0,0,0,NULL);
		str.Format("%d",m_table1.m_sensitive);
		m_list1.SetItem(ntotalItem,28,LVIF_TEXT,_T(str),0,0,0,NULL);
		str.Format("%d",m_table1.m_callnumber);
		m_list1.SetItem(ntotalItem,29,LVIF_TEXT,_T(str),0,0,0,NULL);
		str.Format("%d",m_table1.m_waittime);
		m_list1.SetItem(ntotalItem,30,LVIF_TEXT,_T(str),0,0,0,NULL);
		str.Format("%2.1f",m_table1.m_xudianchi);
		m_list1.SetItem(ntotalItem,31,LVIF_TEXT,_T(str),0,0,0,NULL);
		if(m_table1.m_mode==1)
		   str="����ģʽ";
		else 
		   str="����ģʽ";
		m_list1.SetItem(ntotalItem,32,LVIF_TEXT,_T(str),0,0,0,NULL);
		str.Format("%d",m_table1.m_zejuxisu);
		m_list1.SetItem(ntotalItem,33,LVIF_TEXT,_T(str),0,0,0,NULL);

        m_table1.MoveNext();
	}
}
void CViewFenjiCableInfo::InSertList2()
{
	int ntotalItem;
	CString str;
    while (!m_table2.IsEOF())
	{
		ntotalItem = m_list2.GetItemCount();
		str.Format("%3.3d",m_table2.m_Id/100);
		m_list2.InsertItem(ntotalItem,str);
		str.Format("%3.3d",m_table2.m_Id%100);
		m_list2.SetItem(ntotalItem,1,LVIF_TEXT,_T(str),0,0,0,NULL);
		m_list2.SetItem(ntotalItem,2,LVIF_TEXT,_T(m_table2.m_startname),0,0,0,NULL);
		m_list2.SetItem(ntotalItem,3,LVIF_TEXT,_T(m_table2.m_endname),0,0,0,NULL);
		if(m_table2.m_Id%100<=8)
		{
			if(m_table2.m_state==0)
				str="����";
			else
				str="����";
		}
		else
		{
			switch(m_table2.m_state)
			{
			case 0:
				str="���·���";
				break;
			case 1:
				str="�¶ȸ澯";
				break;
			case 2:
				str="ʪ�ȸ澯";
				break;
			case 3:
				str="����澯";
				break;
			case 4:
				str="�Ž��澯";
				break;
			case 5:
				str="����澯";
				break;
			case 6:
				str="��ˮ�澯";
				break;
			case 7:
				str="�к�����澯";
			case 8:
				str="����";
				break;
			default:
				break;
				
			}
		}
		m_list2.SetItem(ntotalItem,4,LVIF_TEXT,_T(str),0,0,0,NULL);
		m_table2.MoveNext();
	}
}
void CViewFenjiCableInfo::OnOK() 
{
	// TODO: Add extra validation here
	if(m_table1.IsOpen())
       m_table1.Close();
	if(m_table2.IsOpen())
       m_table2.Close();
	CDialog::OnOK();
}

void CViewFenjiCableInfo::OnProbe1() 
{
	// TODO: Add your control notification handler code here
	int nIndex=m_mode1.GetCurSel();
    CString str1,str2;
	UpdateData(TRUE);
	if(nIndex==0)//ȫ����¼
		return;
	else//���ֻ��Ų�ѯ
	{
		if(m_edit1>m_edit2)
		{
			int temp=m_edit1;
			m_edit1=m_edit2;
			m_edit2=temp;
		}
        m_table1.m_strSort=_T("[Id]");
		str1.Format("%d",m_edit1);
		str2.Format("%d",m_edit2);
        m_table1.m_strFilter=_T("[Id] Between " + str1 + " AND " + str2); 
		if(m_table1.Requery())
		{
			m_list1.DeleteAllItems();
			InSertList1();
		}
	}
    UpdateData(FALSE);		
}
void CViewFenjiCableInfo::OnProbe2() 
{
	// TODO: Add your control notification handler code here
	int nIndex=m_mode2.GetCurSel();
    CString str1,str2;
	UpdateData(TRUE);
	if(nIndex==0)//ȫ����¼
		return;
	else//���ֻ��Ų�ѯ
	{
		if(m_edit3>m_edit4)
		{
			int temp=m_edit3;
			m_edit3=m_edit4;
			m_edit4=temp;
		}
        m_table2.m_strSort=_T("[Id]");
		str1.Format("%d",m_edit3*100);
		str2.Format("%d",m_edit4*100+16);
        m_table2.m_strFilter=_T("[Id] Between " + str1 + " AND " + str2); 
		if(m_table2.Requery())
		{
			m_list2.DeleteAllItems();
			InSertList2();
		}
	}
    UpdateData(FALSE);			
}


void CViewFenjiCableInfo::OnSelchangeMode1() 
{
	// TODO: Add your control notification handler code here
	int nIndex=m_mode1.GetCurSel();
	if(nIndex==0)//ȫ����¼
	{
        GetDlgItem(IDC_PROBE1)->EnableWindow(FALSE);
		WINDOWPLACEMENT wndplace;
		wndplace.rcNormalPosition.left	    = 2000;
		wndplace.rcNormalPosition.right	    = 2000;
		wndplace.rcNormalPosition.top		= 2000;
		wndplace.rcNormalPosition.bottom	= 2000;
		GetDlgItem(IDC_MYSTATIC1)->SetWindowPlacement(&wndplace);
		GetDlgItem(IDC_MYSTATIC2)->SetWindowPlacement(&wndplace);
		GetDlgItem(IDC_EDIT1)->SetWindowPlacement(&wndplace);
		GetDlgItem(IDC_EDIT2)->SetWindowPlacement(&wndplace);

		m_table1.m_strSort=_T("[Id]");
        m_table1.m_strFilter=_T(""); 
        m_table1.Requery();
        m_list1.DeleteAllItems();
	    InSertList1();	
    }
	else//���ֻ��Ų�ѯ
	{
        GetDlgItem(IDC_PROBE1)->EnableWindow(TRUE);
		GetDlgItem(IDC_MYSTATIC1)->SetWindowPlacement(&m_mystaticwnd[0]);
		GetDlgItem(IDC_MYSTATIC2)->SetWindowPlacement(&m_mystaticwnd[1]);
		GetDlgItem(IDC_EDIT1)->SetWindowPlacement(&m_editwnd[0]);
		GetDlgItem(IDC_EDIT2)->SetWindowPlacement(&m_editwnd[1]);
        m_edit1=0;
		m_edit2=100;
	}
	UpdateData(FALSE);	
}

void CViewFenjiCableInfo::OnSelchangeMode2() 
{
	// TODO: Add your control notification handler code here
	int nIndex=m_mode2.GetCurSel();
	if(nIndex==0)//ȫ����¼
	{
        GetDlgItem(IDC_PROBE2)->EnableWindow(FALSE);
		WINDOWPLACEMENT wndplace;
		wndplace.rcNormalPosition.left	    = 2000;
		wndplace.rcNormalPosition.right	    = 2000;
		wndplace.rcNormalPosition.top		= 2000;
		wndplace.rcNormalPosition.bottom	= 2000;
		GetDlgItem(IDC_MYSTATIC3)->SetWindowPlacement(&wndplace);
		GetDlgItem(IDC_MYSTATIC4)->SetWindowPlacement(&wndplace);
		GetDlgItem(IDC_EDIT3)->SetWindowPlacement(&wndplace);
		GetDlgItem(IDC_EDIT4)->SetWindowPlacement(&wndplace);

		m_table2.m_strSort=_T("[Id]");
        m_table2.m_strFilter=_T(""); 
        m_table2.Requery();
        m_list2.DeleteAllItems();
	    InSertList2();	
    }
	else//���ֻ��Ų�ѯ
	{
        GetDlgItem(IDC_PROBE2)->EnableWindow(TRUE);
		GetDlgItem(IDC_MYSTATIC3)->SetWindowPlacement(&m_mystaticwnd[2]);
		GetDlgItem(IDC_MYSTATIC4)->SetWindowPlacement(&m_mystaticwnd[3]);
		GetDlgItem(IDC_EDIT3)->SetWindowPlacement(&m_editwnd[2]);
		GetDlgItem(IDC_EDIT4)->SetWindowPlacement(&m_editwnd[3]);
        m_edit3=0;
		m_edit4=10;
	}
	UpdateData(FALSE);		
}

void CViewFenjiCableInfo::OnExport1() 
{
	// TODO: Add your control notification handler code here
	if(!bHasExcelDriver())
	{
		MessageBox("û�а�װMicrosoft Excel Driver!","��ʾ��Ϣ");
		return;
	}
	CString strPathname;

    CFileDialog filesavebox(FALSE,"xls",/*"default.xls"*/NULL,OFN_HIDEREADONLY/*|OFN_OVERWRITEPROMPT*/,"Excel�ļ�(*.xls)|*.xls|",NULL);	
	if(filesavebox.DoModal()!=IDOK) return;
	strPathname=filesavebox.GetPathName();
	CDatabase database;
	CString   sDriver="Microsoft Excel Driver (*.xls)";
	CString sSql;
	CString str;
	int i,nTotalItem,j;
	TRY
	{
		sSql.Format("DRIVER={%s};DSN=;FIRSTROWHASNAMES=1;READONLY=FALSE;CREATE_DB=\"%s\";DBQ=%s",sDriver,strPathname,strPathname);
		if(database.OpenEx(sSql,CDatabase::noOdbcDialog))
		{
			//������ṹ
			//		sSql="DROP TABLE �ֻ���Ϣ��";
			//     database.ExecuteSQL(sSql);
			sSql="CREATE TABLE �ֻ���Ϣ�� (�ֻ��� TEXT,�ֻ��� TEXT,�ֻ��绰 TEXT,";
            sSql+="�û�1 TEXT,�û��绰1 TEXT,�û�2 TEXT,�û��绰2 TEXT,�û�3 TEXT,�û��绰3 TEXT,";
            sSql+="�û�4 TEXT,�û��绰4 TEXT,�û�5 TEXT,�û��绰5 TEXT,�û�6 TEXT,�û��绰6 TEXT,";
            sSql+="ά��1 TEXT,ά���绰1 TEXT,ά��2 TEXT,ά���绰2 TEXT,ά��3 TEXT,ά���绰3 TEXT,";
            sSql+="ά��4 TEXT,ά���绰4 TEXT,ά��5 TEXT,ά���绰5 TEXT,ά��6 TEXT,ά���绰6 TEXT,";
            sSql+="��Ӧ��ȴ�ʱ�� NUMBER,������ NUMBER,ѭ��������� NUMBER,�����ȴ�ʱ�� NUMBER,��ѹת�۵� NUMBER,����ģʽ TEXT,���ϵ�� NUMBER)";
			database.ExecuteSQL(sSql);
	//		sSql="CREATE TABLE �ֻ���Ϣ�� (Name TEXT,Age NUMBER)";
	//		database.ExecuteSQL(sSql);
			str="(�ֻ���,�ֻ���,�ֻ��绰,";
            str+="�û�1,�û��绰1,�û�2,�û��绰2,�û�3,�û��绰3,";
            str+="�û�4,�û��绰4,�û�5,�û��绰5,�û�6,�û��绰6,";
            str+="ά��1,ά���绰1,ά��2,ά���绰2,ά��3,ά���绰3,";
            str+="ά��4,ά���绰4,ά��5,ά���绰5,ά��6,ά���绰6,";
            str+="��Ӧ��ȴ�ʱ��,������,ѭ���������,�����ȴ�ʱ��,��ѹת�۵�,����ģʽ,���ϵ��)";
            nTotalItem=m_list1.GetItemCount();
			for(i=0;i<nTotalItem;i++)
			{
				sSql="INSERT INTO �ֻ���Ϣ�� ";
				sSql+=str;
				sSql+=" VALUES(";
				for(j=0;j<27;j++)
				{
					sSql+="'";
					sSql+=m_list1.GetItemText(i,j);
					sSql+="',";
				}
				for(j=0;j<5;j++)
				{
					sSql+=m_list1.GetItemText(i,j+27);
					sSql+=",";
				}
				sSql+="'";
				sSql+=m_list1.GetItemText(i,32);
				sSql+="',";
				sSql+=m_list1.GetItemText(i,33);
				sSql+=")";
				database.ExecuteSQL(sSql);
			}
		}
        database.Close();
	}
	CATCH_ALL(e)
	{
		
		TRACE0("%s","���ܶԱ�����д��!");
	}
	END_CATCH_ALL;			
}

void CViewFenjiCableInfo::OnExport2() 
{
	// TODO: Add your control notification handler code here
	if(!bHasExcelDriver())
	{
		MessageBox("û�а�װMicrosoft Excel Driver!","��ʾ��Ϣ");
		return;
	}
	CString strPathname;

    CFileDialog filesavebox(FALSE,"xls",/*"default.xls"*/NULL,OFN_HIDEREADONLY/*|OFN_OVERWRITEPROMPT*/,"Excel�ļ�(*.xls)|*.xls|",NULL);	
	if(filesavebox.DoModal()!=IDOK) return;
	strPathname=filesavebox.GetPathName();
	CDatabase database;
	CString   sDriver="Microsoft Excel Driver (*.xls)";
	CString sSql;
	CString str;
	int i,nTotalItem,j;
	TRY
	{
		sSql.Format("DRIVER={%s};DSN=;FIRSTROWHASNAMES=1;READONLY=FALSE;CREATE_DB=\"%s\";DBQ=%s",sDriver,strPathname,strPathname);
		if(database.OpenEx(sSql,CDatabase::noOdbcDialog))
		{
     		sSql="CREATE TABLE ������Ϣ�� (�ֻ��� TEXT,���º� TEXT,������� TEXT,";
            sSql+="�����յ� TEXT,�������� TEXT)";
			database.ExecuteSQL(sSql);
			str="(�ֻ���,���º�,�������,";
            str+="�����յ�,��������)";
            nTotalItem=m_list2.GetItemCount();
			for(i=0;i<nTotalItem;i++)
			{
				sSql="INSERT INTO ������Ϣ�� ";
				sSql+=str;
				sSql+=" VALUES(";
				for(j=0;j<5;j++)
				{
					sSql+="'";
					sSql+=m_list2.GetItemText(i,j);
					sSql+="'";
					if(j<4)
					  sSql+=",";
				}
				sSql+=")";
				database.ExecuteSQL(sSql);
			}
		}
        database.Close();
	}
	CATCH_ALL(e)
	{
		
		TRACE0("%s","���ܶԱ�����д��!");
	}
	END_CATCH_ALL;				
}
BOOL CViewFenjiCableInfo::bHasExcelDriver()
{
    char szBuf[2001];
	WORD cbBufMax=2000;
	WORD cbBufOut;
	char* pszBuf=szBuf;
	CString sDriver;
	if(!SQLGetInstalledDrivers(szBuf,cbBufMax,&cbBufOut)) return FALSE;
	do
	{
		if(strstr(pszBuf,"Excel")!=0)
		   return TRUE;
		pszBuf=strchr(pszBuf,'\0')+1;
	}
	while(pszBuf[0]!='\0');
	return FALSE;
}
