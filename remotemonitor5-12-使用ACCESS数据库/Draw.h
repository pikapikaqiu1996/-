/*
 * 
 * Define Draw Objects
 *
 */


class CDrawObject : public CObject
{
private:
	COLORREF m_PenColor;//ͼԪ��ɫ
	COLORREF m_FillColor;//ͼԪ��ɫ
	COLORREF m_TextColor;//ͼԪ��ɫ
    int      m_nPenWidth;
    CPoint   m_Point;
public:
	bool m_bSelected;
	long m_nStyle;//ͼԪ����
	bool m_bAlarm;
	COLORREF m_FlashTextColor;//ͼԪ��ɫ
	CDrawObject(){m_bSelected = false;};

	void SetPenColor(COLORREF color);//����ͼԪ��ɫ
	void SetFillColor(COLORREF color);//����ͼԪ��ɫ
	void SetTextColor(COLORREF color);//����ͼԪ��ɫ
	void SetPenWidth(int nWidth);
	COLORREF GetPenColor();//���ͼԪ��ɫ
	COLORREF GetFillColor();//���ͼԪ��ɫ
	COLORREF GetTextColor();//���ͼԪ��ɫ
	int      GetPenWidth();
	void HotPoint(CDC* pDC, long x, long y);
	bool AtPoint(long x1, long y1, long x, long y);//�ж�����λ�ù�ϵ
	bool AtLine(long x1, long y1, long x2, long y2, long x, long y);//�жϵ��ֱ�ߵ�λ�ù�ϵ
    virtual int  distance(){return 0;};
    virtual void SetMark(int Mark){};
    virtual int  GetMark(){return 0;};
	virtual void Selected(CDC* pDC, bool select){};//���ƣ�ȡ����ѡ���־
	virtual void Draw(CDC* pDC){};//����ͼԪ
	virtual void FlashDraw(CDC* pDC){};//����ͼԪ
	virtual int  SelectAt(long x, long y){return 0;};//�ж��Ƿ�ѡ��ͼԪ
	virtual void MoveAt(CDC* pDC, int flags, long x, long y){};//��ָ����ʽ�ƶ�ͼԪ
	virtual int  NewPoint(long x, long y){return 0;};//ͼ�ζ����һ�����꣬�������false�������ͼ
	virtual int  AddPoint(long x, long y){return 0;};//ͼ�ζ�����һ�����꣬�������false�������ͼ
	virtual void HotPoints(CDC* pDC){};//�����ȵ�
	virtual void Serialize(CArchive& ar){};
	DECLARE_SERIAL(CDrawObject);//����ʵ�ִ��л��������
};

class CDrawLine : public CDrawObject
{
private:
	long m_x1, m_y1, m_x2, m_y2;
	long m_oldx, m_oldy;
    int  m_nN;
public:
	CDrawLine(COLORREF color = RGB(255,0,0),COLORREF textcolor=RGB(0,0,255),int nPenWidth=1);
    void SetMark(int Mark);
    int  GetMark();
	void Selected(CDC* pDC, bool select);
	void Draw(CDC* pDC);
	void FlashDraw(CDC* pDC);//����ͼԪ
    int  distance();
	int  SelectAt(long x, long y);
	void MoveAt(CDC* pDC, int flags, long x, long y);
	int  NewPoint(long x, long y);
	void HotPoints(CDC* pDC);//�����ȵ�
	void Serialize(CArchive& ar);
	DECLARE_SERIAL(CDrawLine);
};




class CDrawCircle : public CDrawObject
{
private:
	long m_x, m_y, m_r;
	long m_oldx, m_oldy;
    int  m_nN;

public:
	CDrawCircle(COLORREF color = RGB(255,0,0),COLORREF fillcolor=RGB(255,255,0),COLORREF textcolor=RGB(0,0,255));
    void SetMark(int Mark);
    int  GetMark();
	int  distance();
	void Selected(CDC* pDC, bool select);
	void Draw(CDC* pDC);
	void FlashDraw(CDC* pDC);//����ͼԪ
	int  SelectAt(long x, long y);
	void MoveAt(CDC* pDC, int flags, long x, long y);
	int  NewPoint(long x, long y);
	void HotPoints(CDC* pDC);//�����ȵ�
	bool AtinSide(long x, long y);//�жϵ��Ƿ���������
	void Serialize(CArchive& ar);
	DECLARE_SERIAL(CDrawCircle);
};



