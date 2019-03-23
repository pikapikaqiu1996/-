#include "stdafx.h"
#include "resource.h"       // main symbols

#include "Draw.h"
#include "math.h"

#define MAXOFFSET 8 // �������������ʵ��������ֵ
					// ��AtPoint��AtLine������ʹ��

//-----------------------------
// ����ΪCDrawObject�෽��ʵ��
//-----------------------------
extern int   m_nScale;

IMPLEMENT_SERIAL(CDrawObject, CObject, 1);

COLORREF CDrawObject::GetPenColor()
{
	if(this)
	{//��ǰ��Ϊ��Ч��ָ��
		return m_PenColor;
	}
	else
	{
		return 0;
	}
}
COLORREF CDrawObject::GetFillColor()
{
	if(this)
	{//��ǰ��Ϊ��Ч��ָ��
		return m_FillColor;
	}
	else
	{
		return 0;
	}
}
COLORREF CDrawObject::GetTextColor()
{
	if(this)
	{//��ǰ��Ϊ��Ч��ָ��
		return m_TextColor;
	}
	else
	{
		return 0;
	}
}

void CDrawObject::SetPenColor(COLORREF color)
{
	if(this)
	{//��ǰ��Ϊ��Ч��ָ��
		m_PenColor = color;
	}
}
void CDrawObject::SetTextColor(COLORREF color)
{
	if(this)
	{//��ǰ��Ϊ��Ч��ָ��
		m_TextColor = color;
		m_FlashTextColor=color;
	}
}
void CDrawObject::SetFillColor(COLORREF color)
{
	if(this)
	{//��ǰ��Ϊ��Ч��ָ��
		m_FillColor = color;
	}
}

void CDrawObject::SetPenWidth(int nWidth)
{
	if(this)
	{
	   m_nPenWidth=nWidth;
	}
}
int CDrawObject::GetPenWidth()
{
	if(this)
	{//��ǰ��Ϊ��Ч��ָ��
		return m_nPenWidth;
	}
	else
	{
		return 1;
	}
}
void CDrawObject::HotPoint(CDC *pDC, long x, long y)
{
    float ftemp;
    long lx,ly;

	COLORREF XorColor = pDC->GetBkColor();
	CBrush brush(XorColor), *oldbrush;
	CPen pen(PS_SOLID, 1, XorColor), *oldpen;
	int oldmode = pDC->GetROP2();
	oldpen = pDC->SelectObject(&pen);
	oldbrush = pDC->SelectObject(&brush);
	pDC->SetROP2(R2_XORPEN);

    ftemp=(float)x*(float)(m_nScale/100.0);
	lx=(long)(ftemp+0.5);
    ftemp=(float)y*(float)(m_nScale/100.0);
	ly=(long)(ftemp+0.5);

//	pDC->Rectangle(x-2, y-2, x+2, y+2);
	pDC->Rectangle(lx-2, ly-2, lx+2, ly+2);

	pDC->SelectObject(oldpen);
	pDC->SelectObject(oldbrush);
	pDC->SetROP2(oldmode);
	pen.DeleteObject();//�ͷű���Դ
	brush.DeleteObject();//�ͷŻ�ˢ��Դ
}

bool CDrawObject::AtPoint(long x1, long y1, long x, long y)
{
	double d;//�������

	d = sqrt((double)(x1-x)*(x1-x)+(y1-y)*(y1-y));

	return(d < MAXOFFSET);
}

bool CDrawObject::AtLine(long x1, long y1, long x2, long y2, long x, long y)
{
	float A, B, C, d;

	A = (float)(y1-y2);
	B = (float)(x2-x1);
	C = (float)(x1*y2 - x2*y1);
	d = (A*x+B*y+C) * (A*x+B*y+C) / (A*A+B*B);//��ֱ����

	return(d < MAXOFFSET && ((x1-x)*(x2-x)<0 || (y1-y)*(y2-y)<0));
}


//--------------------------------
//����ΪCDrawLine(����)�෽��ʵ��
//--------------------------------

IMPLEMENT_SERIAL(CDrawLine, CDrawObject, 1);

void CDrawLine::Serialize(CArchive& ar)
{
	COLORREF color,textcolor;
    int nWidth;
	if(ar.IsLoading())
	{
		ar >> m_nStyle >> color >> textcolor >> nWidth >> m_x1 >> m_y1 >> m_x2 >> m_y2 >>m_nN;
		SetPenColor(color);
		SetTextColor(textcolor);
		SetPenWidth(nWidth);
        m_bAlarm=false;
	}
	else
	{
		color = GetPenColor();
		textcolor=GetTextColor();
        nWidth=GetPenWidth();

		ar << m_nStyle << color << textcolor << nWidth << m_x1 << m_y1 << m_x2 << m_y2 << m_nN;
	}
}

CDrawLine::CDrawLine(COLORREF color,COLORREF textcolor,int nPenWidth)
{
	m_x1=m_y1=m_x2=m_y2=0;
	SetPenColor(color);
	SetTextColor(textcolor);
	SetPenWidth(nPenWidth);
	m_nStyle = ID_DRAWLINE;
	m_nN=1;
	m_bAlarm=false;
}

void CDrawLine::Draw(CDC* pDC)
{
    float ftemp;
	long x1,y1,x2,y2;
	if(!this)
	{//��ǰ��Ϊ�գ�����Ч��ָ��
		return;
	}

	COLORREF CopyColor = GetPenColor();
	int nPenWidth=GetPenWidth();
	CPen pen(PS_SOLID, nPenWidth, CopyColor), *oldpen;
	int oldmode = pDC->GetROP2();
	oldpen = pDC->SelectObject(&pen);
	pDC->SetROP2(R2_COPYPEN);

    ftemp=(float)m_x1*(float)(m_nScale/100.0);
	x1=(long)(ftemp+0.5);
    ftemp=(float)m_y1*(float)(m_nScale/100.0);
	y1=(long)(ftemp+0.5);
    ftemp=(float)m_x2*(float)(m_nScale/100.0);
	x2=(long)(ftemp+0.5);
    ftemp=(float)m_y2*(float)(m_nScale/100.0);
	y2=(long)(ftemp+0.5);

	pDC->MoveTo(x1, y1);//�ƶ���ǰ��ͼ��Ϊ�߶ε�һ��
	pDC->LineTo(x2, y2);//����ֱ��

	pDC->SelectObject(oldpen);
	pDC->SetROP2(oldmode);
	pen.DeleteObject();//�ͷű���Դ

    ftemp=(float)m_x1*(float)(m_nScale/100.0)+(float)m_x2*(float)(m_nScale/100.0);
	ftemp/=2.0;
	x1=(int)ftemp;
    ftemp=(float)m_y1*(float)(m_nScale/100.0)+(float)m_y2*(float)(m_nScale/100.0);
	ftemp/=2.0;
	y1=(int)ftemp;
	CString str;

	str.Format("%3.3d-%2.2d",m_nN/100,m_nN%100);
//    int oldBkMode=pDC->SetBkMode(TRANSPARENT); 
    COLORREF oldtextcolor=pDC->SetTextColor(GetTextColor());
    pDC->TextOut(x1,y1,str);
    pDC->SetTextColor(oldtextcolor);
 //   pDC->SetBkMode(oldBkMode);
}
void CDrawLine::FlashDraw(CDC* pDC)
{
    float ftemp;
	CString str;
	long x1,y1;
    COLORREF oldtextcolor;

	if(!this)
	{//��ǰ��Ϊ�գ�����Ч��ָ��
		return;
	}


    ftemp=(float)m_x1*(float)(m_nScale/100.0)+(float)m_x2*(float)(m_nScale/100.0);
	ftemp/=2.0;
	x1=(int)ftemp;
    ftemp=(float)m_y1*(float)(m_nScale/100.0)+(float)m_y2*(float)(m_nScale/100.0);
	ftemp/=2.0;
	y1=(int)ftemp;

	str.Format("%3.3d-%2.2d",m_nN/100,m_nN%100);
//    int oldBkMode=pDC->SetBkMode(TRANSPARENT); 
	if(m_FlashTextColor==GetTextColor())
	   m_FlashTextColor=GetTextColor()^0xffffff;
    else
	   m_FlashTextColor=GetTextColor();
    oldtextcolor=pDC->SetTextColor(m_FlashTextColor);
    pDC->TextOut(x1,y1,str);
    pDC->SetTextColor(oldtextcolor);	
}

int CDrawLine::NewPoint(long x, long y)
{
	m_x1 = m_x2 = x;
	m_y1 = m_y2 = y;

	return 2;
}


int CDrawLine::SelectAt(long x, long y)
{

	if(AtPoint(m_x1, m_y1, x, y))
	{//ָ�뿿����һ��
		return 1;
	}
	else if(AtPoint(m_x2, m_y2, x, y))
	{//ָ�뿿���ڶ���
		return 2;
	}
	else if(AtLine(m_x1, m_y1, m_x2, m_y2, x, y))
	{//ָ����ֱ����
		m_oldx = x;//���浱ǰ��λ�ã������Ժ��ж��ƶ��ľ���
		m_oldy = y;
		return 3;
	}
	else 
		return 0;
}

void CDrawLine::MoveAt(CDC *pDC, int flags, long x, long y)
{
	//��������DC
    float ftemp;
	long x1,y1,x2,y2;
	COLORREF XorColor = pDC->GetBkColor() ^ GetPenColor();
	int nPenWidth=GetPenWidth();
	CPen pen(PS_SOLID, nPenWidth, XorColor), *oldpen;
	int oldmode = pDC->GetROP2();
	oldpen = pDC->SelectObject(&pen);
	pDC->SetROP2(R2_XORPEN);
	//�����ƶ�ֱ��
	if(m_bSelected)
	{//��ǰ�Ѿ�ѡ���ˣ�ȡ���ȵ�
		HotPoints(pDC);
	}

    ftemp=(float)m_x1*(float)(m_nScale/100.0);
	x1=(long)(ftemp+0.5);
    ftemp=(float)m_y1*(float)(m_nScale/100.0);
	y1=(long)(ftemp+0.5);
    ftemp=(float)m_x2*(float)(m_nScale/100.0);
	x2=(long)(ftemp+0.5);
    ftemp=(float)m_y2*(float)(m_nScale/100.0);
	y2=(long)(ftemp+0.5);

	pDC->MoveTo(x1, y1);
	pDC->LineTo(x2, y2);//ɾ��ԭͼ��

	//�޸���ͼ������
	if(flags == 1)
	{
		m_x1 = x, m_y1 = y;
	}else if(flags == 2)
	{
		m_x2 = x, m_y2 = y;
	}else if(flags == 3)
	{
		m_x1 += x-m_oldx;
		m_x2 += x-m_oldx;
		m_y1 += y-m_oldy;
		m_y2 += y-m_oldy;
		m_oldx = x;
		m_oldy = y;
	}

    ftemp=(float)m_x1*(float)(m_nScale/100.0);
	x1=(long)(ftemp+0.5);
    ftemp=(float)m_y1*(float)(m_nScale/100.0);
	y1=(long)(ftemp+0.5);
    ftemp=(float)m_x2*(float)(m_nScale/100.0);
	x2=(long)(ftemp+0.5);
    ftemp=(float)m_y2*(float)(m_nScale/100.0);
	y2=(long)(ftemp+0.5);

	pDC->MoveTo(x1, y1);
	pDC->LineTo(x2, y2);//����ͼ��

	if(m_bSelected)
	{//��ǰ�Ѿ�ѡ���ˣ������ȵ�
		HotPoints(pDC);
	}
	//���»ָ�DC
	pDC->SetROP2(oldmode);
	pDC->SelectObject(oldpen);
	pen.DeleteObject();//�ͷű���Դ
}

void CDrawLine::HotPoints(CDC* pDC)
{
	HotPoint(pDC, m_x1, m_y1);
	HotPoint(pDC, m_x2, m_y2);
}

void CDrawLine::Selected(CDC *pDC, bool select)
{
	if(!this)
	{//�����ָ����Ч��ֱ�ӷ���
		return;
	}

	if(select)
	{
		if(!m_bSelected)
		{//��ǰδѡ�񣬻��ȵ㲢����ѡ���־
			HotPoints(pDC);
			m_bSelected = true;
		}
	}
	else
	{
		if(m_bSelected)
		{//��ǰ�Ѿ�ѡ���ˣ�ȡ���ȵ����ò���ѡ���־
			HotPoints(pDC);
			m_bSelected = false;
		}
	}
}

//-----------------------------------
// ����ΪCDrawCircle(��Բ)�෽��ʵ��
//-----------------------------------

IMPLEMENT_SERIAL(CDrawCircle, CDrawObject, 1);

void CDrawCircle::Serialize(CArchive& ar)
{
	COLORREF color,fillcolor,textcolor;

	if(ar.IsLoading())
	{
		ar >> m_nStyle >> color >> fillcolor >> textcolor >> m_x >> m_y >> m_nN;
		SetPenColor(color);
		SetFillColor(fillcolor);
		SetTextColor(textcolor);
        m_bAlarm=false;
	}
	else
	{
		color = GetPenColor();
        fillcolor=GetFillColor();
		textcolor=GetTextColor();
		ar << m_nStyle << color << fillcolor << textcolor << m_x << m_y << m_nN;
	}
}

CDrawCircle::CDrawCircle(COLORREF color,COLORREF fillcolor,COLORREF textcolor)
{
	m_x=m_y=0;
	m_r=14;
	SetPenColor(color);
	SetFillColor(fillcolor);
	SetTextColor(textcolor);
	m_nStyle = ID_DRAWCIRCLE;
	m_nN=0;
	m_bAlarm=false;
}
void CDrawCircle::FlashDraw(CDC* pDC)
{
    float ftemp;
	CString str;
	long lx,ly;
    COLORREF oldtextcolor;

	if(!this)
	{//��ǰ��Ϊ�գ�����Ч��ָ��
		return;
	}

    ftemp=(float)m_x*(float)(m_nScale/100.0);
	lx=(long)(ftemp+0.5);
    ftemp=(float)m_y*(float)(m_nScale/100.0);
	ly=(long)(ftemp+0.5);

	str.Format("%3.3d",m_nN/100);

    int oldBkMode=pDC->SetBkMode(TRANSPARENT);

	if(m_FlashTextColor==GetTextColor())
	   m_FlashTextColor=GetTextColor()^0xffffff;
    else
	   m_FlashTextColor=GetTextColor();
    oldtextcolor=pDC->SetTextColor(m_FlashTextColor);

    pDC->TextOut(lx-12,ly-8,str);
    pDC->SetTextColor(oldtextcolor);
    pDC->SetBkMode(oldBkMode);



}
void CDrawCircle::Draw(CDC* pDC)
{
	float ftemp;
	long lx,ly;
	if(!this)
	{//��ǰ��Ϊ�գ�����Ч��ָ��
		return;
	}

	COLORREF CopyColor = GetPenColor();
	CPen pen(PS_SOLID, 1, CopyColor);
	int oldmode = pDC->GetROP2();
	CPen* oldpen = pDC->SelectObject(&pen);//ָ�����Ʊ�;
	//����Ϊ���ա�ˢ���Ȳ���䡣
	CBrush brush(GetFillColor());
//	CBrush* oldbrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);;
	CBrush* oldbrush = pDC->SelectObject(&brush);
	pDC->SetROP2(R2_COPYPEN);//ָ������ģʽ

    ftemp=(float)m_x*(float)(m_nScale/100.0);
	lx=(long)(ftemp+0.5);
    ftemp=(float)m_y*(float)(m_nScale/100.0);
	ly=(long)(ftemp+0.5);

//	pDC->Ellipse(m_x-m_r, m_y-m_r, m_x+m_r, m_y+m_r);//��Բ
	pDC->Ellipse(lx-m_r, ly-m_r, lx+m_r, ly+m_r);//��Բ
	//�ָ�DC
	pDC->SetROP2(oldmode);
	pDC->SelectObject(oldpen);
	pDC->SelectObject(oldbrush);
	pen.DeleteObject();//�ͷű���Դ
	brush.DeleteObject();//�ͷű���Դ

	CString str;
	str.Format("%3.3d",m_nN/100);
    int oldBkMode=pDC->SetBkMode(TRANSPARENT); 
    COLORREF oldtextcolor=pDC->SetTextColor(GetTextColor());
    pDC->TextOut(lx-12,ly-8,str);
    pDC->SetTextColor(oldtextcolor);
    pDC->SetBkMode(oldBkMode);
}

int CDrawCircle::NewPoint(long x, long y)
{
	m_x = x;
	m_y = y;
	m_oldx = x;
	m_oldy = y;

	return 1;
}

int CDrawCircle::SelectAt(long x, long y)
{

	if(AtinSide(x, y))
	{//ָ����Բ��
		m_oldx = x;
		m_oldy = y;
		return 1;
	}
	else 
		return 0;
}

void CDrawCircle::MoveAt(CDC *pDC, int flags, long x, long y)
{
	//��������DC
	float ftemp;
	long lx,ly;
	COLORREF XorColor = pDC->GetBkColor() ^ GetPenColor();
	CPen pen(PS_SOLID, 1, XorColor);
	int oldmode = pDC->GetROP2();
	CPen* oldpen = pDC->SelectObject(&pen);
	//����Ϊ���ա�ˢ���Ȳ���䡣
	CBrush brush(pDC->GetBkColor()^GetFillColor());
//	CBrush* oldbrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
	CBrush* oldbrush =pDC->SelectObject(&brush);

	pDC->SetROP2(R2_XORPEN);

	if(m_bSelected)
	{//��ǰ�Ѿ�ѡ���ˣ�ȡ���ȵ�
		HotPoints(pDC);
	}
	//ɾ��ԭͼ��
    ftemp=(float)m_x*(float)(m_nScale/100.0);
	lx=(long)(ftemp+0.5);
    ftemp=(float)m_y*(float)(m_nScale/100.0);
	ly=(long)(ftemp+0.5);

//	pDC->Ellipse(m_x-m_r, m_y-m_r, m_x+m_r, m_y+m_r);//��Բ
	pDC->Ellipse(lx-m_r, ly-m_r, lx+m_r, ly+m_r);//��Բ

	//�޸���ͼ������
	if(flags)
	{
		m_x += (x-m_oldx);
		m_y += (y-m_oldy);
		m_oldx = x;
		m_oldy = y;
	}

	//����ͼ��
    ftemp=(float)m_x*(float)(m_nScale/100.0);
	lx=(long)(ftemp+0.5);
    ftemp=(float)m_y*(float)(m_nScale/100.0);
	ly=(long)(ftemp+0.5);

//	pDC->Ellipse(m_x-m_r, m_y-m_r, m_x+m_r, m_y+m_r);//��Բ
	pDC->Ellipse(lx-m_r, ly-m_r, lx+m_r, ly+m_r);//��Բ
	if(m_bSelected)
	{//��ǰ�Ѿ�ѡ���ˣ������ȵ�
		HotPoints(pDC);
	}
	//���»ָ�DC
	pDC->SetROP2(oldmode);
	pDC->SelectObject(oldpen);
	pDC->SelectObject(oldbrush);
	pen.DeleteObject();
	brush.DeleteObject();
}

void CDrawCircle::HotPoints(CDC* pDC)
{
	HotPoint(pDC, m_x-m_r, m_y);// 1
	HotPoint(pDC, m_x, m_y-m_r);// 2
	HotPoint(pDC, m_x+m_r, m_y);// 3
	HotPoint(pDC, m_x, m_y+m_r);// 4
	HotPoint(pDC, m_x, m_y);// Բ��
}

void CDrawCircle::Selected(CDC *pDC, bool select)
{
	if(!this)
	{//�����ָ����Ч��ֱ�ӷ���
		return;
	}

	if(select)
	{
		if(!m_bSelected)
		{//��ǰδѡ�񣬻��ȵ㲢����ѡ���־
			HotPoints(pDC);
			m_bSelected = true;
		}
	}
	else
	{
		if(m_bSelected)
		{//��ǰ�Ѿ�ѡ���ˣ�ȡ���ȵ����ò���ѡ���־
			HotPoints(pDC);
			m_bSelected = false;
		}
	}
}
/*
bool CDrawCircle::AtCurve(long x, long y)
{
	double l;

	l = fabs(sqrt((m_x - x)*(m_x - x) + (m_y - y)*(m_y - y)) - m_r);

	return(l < MAXOFFSET);
}
*/
bool CDrawCircle::AtinSide(long x, long y)
{
	bool bInside=FALSE;
    if(sqrt((m_x - x)*(m_x - x) + (m_y - y)*(m_y - y)) < m_r)
	    bInside = TRUE;
	return bInside;
}

void CDrawLine::SetMark(int Mark)
{
  m_nN=Mark;
  
}

void CDrawCircle::SetMark(int Mark)
{
    m_nN=Mark;
}

int CDrawLine::GetMark()
{
 return m_nN;
}

int CDrawCircle::GetMark()
{
return m_nN;
}

int CDrawLine::distance()
{
	return (int)(fabs(m_x1-m_x2)+fabs(m_y1-m_y2));

}

int CDrawCircle::distance()
{
	return 100;

}