﻿#pragma once

enum class FontColor
{
	WHITE,
	BLACK,
};


class TextHandle
{
public:
	TextHandle() {};
	~TextHandle() { }
	
	ComPtr<ID2D1Bitmap1> bitMapGpu{};
	ComPtr<ID2D1Bitmap1> bitMapRead{};
	ComPtr<ID2D1SolidColorBrush> brush{};
	ComPtr<IDWriteTextFormat> font{};
	FontColor fontcolor;
	uint32 width{};
	uint32 height{};
	float fontSize{};
};


class TextManager
{
public:
	static unique_ptr<TextManager> main;

public:
	void Init();
	void UpdateToSysMemory(const wstring& text, shared_ptr<TextHandle>& handle , BYTE* memory );
	void PrintFontAll(); //사용할수있는 모든 폰트 콘솔로 출력.

	shared_ptr<TextHandle> AllocTextStrcture(int width ,int height , const WCHAR* font, FontColor color , float fontsize );

private:
	void InitD2D();
	void CreateSolidBrush();


private:

	ComPtr<ID2D1Device2> _device = nullptr;
	ComPtr<ID2D1DeviceContext2> _context = nullptr;

	ComPtr<ID2D1Bitmap1> _bitMapGpu = nullptr;     //렌더타겟 (GPU 메모리)
	ComPtr<ID2D1Bitmap1> _bitMapReadBack = nullptr; //렌더타겟 (ReadBack :시스템 메모리)

	ComPtr<IDWriteFactory5> _factory = nullptr;

	unordered_map<FontColor, ComPtr<ID2D1SolidColorBrush>> _brushMap;

	//추가적인 공부필요
	 
	//IDWriteFontCollection1* m_pFontCollection = nullptr;  //폰트관리.
	//DWRITE_LINE_METRICS*	m_pLineMetrics = nullptr;
	//DWORD					m_dwMaxLineMetricsNum = 0;
};

