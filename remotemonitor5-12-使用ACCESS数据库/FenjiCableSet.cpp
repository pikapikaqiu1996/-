// FenjiCableSet.cpp : implementation file
//

#include "stdafx.h"
#include "remotemonitor.h"
#include "FenjiCableSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFenjiCableSet dialog


CFenjiCableSet::CFenjiCableSet(CWnd* pParent /*=NULL*/)
	: CDialog(CFenjiCableSet::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFenjiCableSet)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFenjiCableSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFenjiCableSet)
	DDX_Control(pDX, IDC_DELETE2, m_delete2);
	DDX_Control(pDX, IDC_DELETE1, m_delete1);
	DDX_Control(pDX, IDC_CHANGE2, m_change2);
	DDX_Control(pDX, IDC_CHANGE1, m_change1);
	DDX_Control(pDX, IDC_CABLELIST, m_list2);
	DDX_Control(pDX, IDC_FENJILIST, m_list1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFenjiCableSet, CDialog)
	//{{AFX_MSG_MAP(CFenjiCableSet)
	ON_BN_CLICKED(IDC_ADD1, OnAdd1)
	ON_BN_CLICKED(IDC_CHANGE1, OnChange1)
	ON_BN_CLICKED(IDC_DELETE1, OnDelete1)
	ON_BN_CLICKED(IDC_ADD2, OnAdd2)
	ON_BN_CLICKED(IDC_CHANGE2, OnChange2)
	ON_BN_CLICKED(IDC_DELETE2, OnDelete2)
	ON_NOTIFY(NM_CLICK, IDC_CABLELIST, OnClickCablelist)
	ON_NOTIFY(NM_CLICK, IDC_FENJILIST, OnClickFenjilist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFenjiCableSet message handlers

BOOL CFenjiCableSet::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString str;

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
    m_list2.InsertColumn( 1, "���º�", LVCFMT_LEFT,100, 0 );
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

    m_change1.EnableWindow(FALSE);
    m_delete1.EnableWindow(FALSE);
    m_change2.EnableWindow(FALSE);
    m_delete2.EnableWindow(FALSE);

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CFenjiCableSet::InSertList2()
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
				str="����";
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

void CFenjiCableSet::InSertList1()
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
void CFenjiCableSet::OnOK() 
{
	// TODO: Add extra validation here
	if(m_table1.IsOpen())
       m_table1.Close();
	if(m_table2.IsOpen())
       m_table2.Close();
	CDialog::OnOK();
}
#include "AddorChangeFenjiInfo.h"
void CFenjiCableSet::OnAdd1() 
{
	// TODO: Add your control notification handler code here
	CAddorChangeFenjiInfo dlg;
	dlg.m_flag=0;//��
    dlg.m_pSet=&m_table1;
	if(dlg.DoModal()==IDOK)
	{
       m_list1.DeleteAllItems();
       m_table1.m_strSort=_T("[Id]");
       m_table1.m_strFilter=_T("");
       m_table1.Requery();
       InSertList1();
	}

}

void CFenjiCableSet::OnChange1() 
{
	// TODO: Add your control notification handler code here
	int index;
    CString str;

	index=m_list1.GetNextItem(-1,LVNI_SELECTED);
	if(index==-1)
	{
        m_change1.EnableWindow(FALSE);
        m_delete1.EnableWindow(FALSE);	
		return;
	}

	str=m_list1.GetItemText(index,0);
	m_table1.m_strFilter=_T("[Id]= ");
    m_table1.m_strFilter+=str;
    m_table1.Requery();
	CAddorChangeFenjiInfo dlg;
	dlg.m_flag=1;//
    dlg.m_pSet=&m_table1;
	if(dlg.DoModal()==IDOK)
	{
       m_list1.DeleteAllItems();
       m_table1.m_strSort=_T("[Id]");
       m_table1.m_strFilter=_T("");
       m_table1.Requery();
       InSertList1();
	}
    m_change1.EnableWindow(FALSE);
    m_delete1.EnableWindow(FALSE);	
}

void CFenjiCableSet::OnDelete1() 
{
	// TODO: Add your control notification handler code here
	int index;
    CString str;

	index=m_list1.GetNextItem(-1,LVNI_SELECTED);
	if(index==-1)
	{
        m_change1.EnableWindow(FALSE);
        m_delete1.EnableWindow(FALSE);	
		return;
	}
	str=m_list1.GetItemText(index,0);
	m_table1.m_strFilter=_T("[Id]= ");
    m_table1.m_strFilter+=str;
    m_table1.Requery();
    m_table1.Delete();
    m_table1.MoveNext();
	m_list1.DeleteItem(index);
    m_change1.EnableWindow(FALSE);
    m_delete1.EnableWindow(FALSE);		
}
#include "CableInfoSet.h"
void CFenjiCableSet::OnAdd2() 
{
	// TODO: Add your control notification handler code here
	CCableInfoSet dlg;
	dlg.m_flag=0;//��
    dlg.m_pSet=&m_table2;
	if(dlg.DoModal()==IDOK)
	{
       m_list2.DeleteAllItems();
       m_table2.m_strSort=_T("[Id]");
       m_table2.m_strFilter=_T("");
       m_table2.Requery();
       InSertList2();
	}	
}

void CFenjiCableSet::OnChange2() 
{
	// TODO: Add your control notification handler code here

	int index;
	int fenjinum,cablenum;
    CString str;

	index=m_list2.GetNextItem(-1,LVNI_SELECTED);
	if(index==-1)
	{
        m_change2.EnableWindow(FALSE);
        m_delete2.EnableWindow(FALSE);	
		return;
	}

	str=m_list2.GetItemText(index,0);
	fenjinum=atoi(str);
	str=m_list2.GetItemText(index,1);
	cablenum=atoi(str);
	str.Format("%d",fenjinum*100+cablenum);
	m_table2.m_strFilter=_T("[Id]= ");
    m_table2.m_strFilter+=str;
    m_table2.Requery();

	CCableInfoSet dlg;
	dlg.m_flag=1;
    dlg.m_pSet=&m_table2;
	dlg.m_fenjinum=fenjinum;
	dlg.m_cablenum=cablenum;
	if(dlg.DoModal()==IDOK)
	{
       m_list2.DeleteAllItems();
       m_table2.m_strSort=_T("[Id]");
       m_table2.m_strFilter=_T("");
       m_table2.Requery();
       InSertList2();
	}

    m_change2.EnableWindow(FALSE);
    m_delete2.EnableWindow(FALSE);	

}

void CFenjiCableSet::OnDelete2() 
{
	// TODO: Add your control notification handler code here
	int index,fenjinum,cablenum;
    CString str;

	index=m_list2.GetNextItem(-1,LVNI_SELECTED);
	if(index==-1)
	{
        m_change2.EnableWindow(FALSE);
        m_delete2.EnableWindow(FALSE);	
		return;
	}
	str=m_list2.GetItemText(index,0);
    fenjinum=atoi(str);
	str=m_list2.GetItemText(index,1);
    cablenum=atoi(str);
	str.Format("%d",fenjinum*100+cablenum);
	m_table2.m_strFilter=_T("[Id]= ");
    m_table2.m_strFilter+=str;
    m_table2.Requery();
    m_table2.Delete();
    m_table2.MoveNext();
	m_list2.DeleteItem(index);
    m_change2.EnableWindow(FALSE);
    m_delete2.EnableWindow(FALSE);			
}

void CFenjiCableSet::OnClickCablelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int index=m_list2.GetNextItem(-1,LVNI_SELECTED);
	if(index!=-1)
	{
       m_change2.EnableWindow(TRUE);
       m_delete2.EnableWindow(TRUE);
	}
	else
	{
       m_change2.EnableWindow(FALSE);
       m_delete2.EnableWindow(FALSE);
	}		
	*pResult = 0;
}

void CFenjiCableSet::OnClickFenjilist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int nIndex;

	nIndex=m_list1.GetNextItem(-1,LVNI_SELECTED);
	if(nIndex!=-1)
	{
		m_change1.EnableWindow(TRUE);
		m_delete1.EnableWindow(TRUE);
	}
	else
	{
       m_change1.EnableWindow(FALSE);
       m_delete1.EnableWindow(FALSE);
	}	
	*pResult = 0;
}
