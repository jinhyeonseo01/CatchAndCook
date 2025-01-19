#pragma once
class TextManager
{
public:
	static unique_ptr<TextManager> main;

public:
	void Init();

private:
	void InitD2D();
	void InitDWrite();



private:
	ComPtr<ID2D1Device2> _device = nullptr;
	ComPtr<ID2D1DeviceContext2> _context = nullptr;

	ComPtr<ID2D1Bitmap1> _bitMapGpu = nullptr;     //����Ÿ�� (GPU �޸�)
	ComPtr<ID2D1Bitmap1> _bitMapReadBack = nullptr; //����Ÿ�� (ReadBack :�ý��� �޸�)

	//IDWriteFontCollection1* m_pFontCollection = nullptr;  //��Ʈ����.
	ComPtr<ID2D1SolidColorBrush> _brush = nullptr;	  //��Ʈ����

	ComPtr<IDWriteFactory5> _factory = nullptr;
	//DWRITE_LINE_METRICS*	m_pLineMetrics = nullptr;
	//DWORD					m_dwMaxLineMetricsNum = 0;
};

