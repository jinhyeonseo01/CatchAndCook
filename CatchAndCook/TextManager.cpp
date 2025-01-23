#pragma once
#include "pch.h"
#include "TextManager.h"

using namespace D2D1;

unique_ptr<TextManager> TextManager::main=nullptr;

void TextManager::Init()
{
    InitD2D();
    CreateSolidBrush();

}

void TextManager::UpdateToSysMemory(const wstring& text, shared_ptr<TextHandle>& handle , BYTE* memory )
{
    IDWriteTextLayout* textLayout = nullptr;

    //�ؽ�Ʈ���̾ƿ� ����
    ThrowIfFailed(_factory->CreateTextLayout(text.c_str(), text.length(), handle->font.Get(), 1024, handle->height, &textLayout));

    DWRITE_TEXT_METRICS metrics = {};

    //Rendering To ��Ʈ��
    if (textLayout)
    {
        textLayout->GetMetrics(&metrics);

        // Ÿ�ټ���
        _context->SetTarget(handle->bitMapGpu.Get());

        // ��Ƽ�ٸ���̸�� ����
        _context->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

        // �ؽ�Ʈ ������
        _context->BeginDraw();

        if (handle->fontcolor == FontColor::WHITE)
        {
            _context->Clear(D2D1::ColorF(D2D1::ColorF::Black));
        }

        else if (handle->fontcolor == FontColor::BLACK)
        {
            _context->Clear(D2D1::ColorF(D2D1::ColorF::White));
        }

        _context->SetTransform(D2D1::Matrix3x2F::Identity());

        _context->DrawTextLayout(D2D1::Point2F(0.0f, 0.0f), textLayout, handle->brush.Get());

        // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
        // is lost. It will be handled during the next call to Present.
        _context->EndDraw();

        // ��Ƽ�ٸ���� ��� ����    
        _context->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_DEFAULT);
        _context->SetTarget(nullptr);

        textLayout->Release();
        textLayout = nullptr;
    }

    int width = (int)ceil(metrics.width);
    int height = (int)ceil(metrics.height);
 
    D2D1_POINT_2U	destPos = {};
    D2D1_RECT_U		srcRect = { 0, 0, width, height };

    ThrowIfFailed(handle->bitMapRead.Get()->CopyFromBitmap(&destPos, handle->bitMapGpu.Get(), &srcRect));

    //////////////////////////////////////////////// 

    D2D1_MAPPED_RECT	mappedRect;
    if (FAILED(handle->bitMapRead.Get()->Map(D2D1_MAP_OPTIONS_READ, &mappedRect)))
        __debugbreak();

    BYTE* pDest = memory;
    char* pSrc = (char*)mappedRect.bits;

    for (DWORD y = 0; y < (DWORD)height; y++)
    {
        memcpy(pDest, pSrc, width * 4);
        pDest += handle->width*4;
        pSrc += mappedRect.pitch;
    }

    handle->bitMapRead.Get()->Unmap();
}


void TextManager::PrintFontAll()
{

    Microsoft::WRL::ComPtr<IDWriteFontCollection> fontCollection;
    _factory->GetSystemFontCollection(&fontCollection);

    UINT32 fontCount = fontCollection->GetFontFamilyCount();
    for (UINT32 i = 0; i < fontCount; i++) {
        Microsoft::WRL::ComPtr<IDWriteFontFamily> fontFamily;
        fontCollection->GetFontFamily(i, &fontFamily);

        Microsoft::WRL::ComPtr<IDWriteLocalizedStrings> familyNames;
        fontFamily->GetFamilyNames(&familyNames);

        UINT32 index = 0;
        BOOL exists = false;
        familyNames->FindLocaleName(L"en-us", &index, &exists);
        if (exists) {
            UINT32 length = 0;
            familyNames->GetStringLength(index, &length);

            std::wstring fontName(length + 1, L'\0');
            familyNames->GetString(index, &fontName[0], length + 1);
            wprintf(L"Font Family: %s\n", fontName.c_str());
        }
    }

}

shared_ptr<TextHandle> TextManager::AllocTextStrcture(int width, int height, const WCHAR* font, FontColor color, float fontsize)
{
    shared_ptr<TextHandle> textHandle = make_shared<TextHandle>();

    textHandle->width = width;
    textHandle->height = height;
    textHandle->fontSize = fontsize;
    textHandle->brush = _brushMap[color];
    textHandle->fontcolor = color;
   
    uint32 dpi = ::GetDpiForWindow(Core::main->GetHandle());

    //��Ʈ�� ����
    {
        D2D1_BITMAP_PROPERTIES1 bitmapProperties =
            BitmapProperties1(
                D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
                D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
                dpi,
                dpi
            );

        D2D1_SIZE_U	size_u;
        size_u.width = width;
        size_u.height = height;

        ThrowIfFailed(_context->CreateBitmap(size_u, nullptr, 0, &bitmapProperties, textHandle->bitMapGpu.GetAddressOf()));

        bitmapProperties.bitmapOptions = D2D1_BITMAP_OPTIONS_CANNOT_DRAW | D2D1_BITMAP_OPTIONS_CPU_READ;

        ThrowIfFailed(_context->CreateBitmap(size_u, nullptr, 0, &bitmapProperties, textHandle->bitMapRead.GetAddressOf()));
    }

    //��Ʈ����
    {
        ThrowIfFailed(_factory->CreateTextFormat(
            font,
            nullptr,
            DWRITE_FONT_WEIGHT_REGULAR,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            fontsize,
            L"ko-kr",
            &textHandle->font
        ));

        ThrowIfFailed(textHandle->font.Get()->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING));
        ThrowIfFailed(textHandle->font.Get()->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));
    };

    return textHandle;
}

void TextManager::InitD2D()
{
    // D3D12 ��ġ �� ��� ť ��������.
    auto d3d_device = Core::main->GetDevice().Get();
    auto d3d_cmdQueue = Core::main->GetCmdQueue().Get();

    uint32 d3d11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    D2D1_FACTORY_OPTIONS d2dFactoryOptions = {};  // D2D1 ���丮 �ɼ� (����� ���� ����)

    // D2D1 ���丮 �� D3D11 ���� �������̽� ������ ����
    ID2D1Factory3* factory = nullptr;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* d3d11Context = nullptr;
    ID3D11On12Device* d3d11On12Device = nullptr;

    // D2D1 ���丮���� ����� ������ �����մϴ�.
    d3d11DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    d2dFactoryOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;

    // D3D11On12 ��ġ�� �����մϴ�. D3D12 ��ġ�� ����Ͽ� D3D11 ��ġ�� ���� ��, D3D11On12Device�� ����ϴ�.
    ThrowIfFailed(D3D11On12CreateDevice(
        d3d_device,                           // ���� D3D12 ��ġ
        d3d11DeviceFlags,                     // D3D11 ��ġ �÷���
        nullptr,                              // �߰����� ���� ���ҽ� (���⼭�� ������� ����)
        0,                                     // ���� ���ҽ� ����
        (IUnknown**)&d3d_cmdQueue,             // D3D12 ��� ť
        1,                                     // ��� ť ����
        0,                                     // ť�� Ÿ�̹� (������� ����)
        &device,                               // ��ȯ�� D3D11 ��ġ
        &d3d11Context,                         // ��ȯ�� D3D11 ��ġ ���ؽ�Ʈ
        nullptr                                // ��ȯ�� DXGI ��ġ (���⼭�� ������� ����)
    ));

    // D3D11On12 ��ġ���� D3D11 ��ġ �������̽��� �����ɴϴ�.
    ThrowIfFailed(device->QueryInterface(IID_PPV_ARGS(&d3d11On12Device)));

    // D2D1 ��ġ ���� �ɼ� ����
    D2D1_DEVICE_CONTEXT_OPTIONS deviceOptions = D2D1_DEVICE_CONTEXT_OPTIONS_NONE;

    // D2D1 ���丮�� �����մϴ�.
    ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &d2dFactoryOptions, (void**)&factory));

    // D3D11On12 ��ġ���� IDXGIDevice�� ���ɴϴ�. (D2D�� D3D11On12 ��ġ�� ����� �� �ֵ��� �ϱ� ����)
    IDXGIDevice* pDXGIDevice = nullptr;
    ThrowIfFailed(d3d11On12Device->QueryInterface(IID_PPV_ARGS(&pDXGIDevice)));

    // D2D1 ���丮���� D3D11On12 ��ġ�� ������� D2D1 ��ġ ����
    ThrowIfFailed(factory->CreateDevice(pDXGIDevice, _device.GetAddressOf()));

    // D2D1 ��ġ�κ��� D2D1 ����̽� ���ؽ�Ʈ ����
    ThrowIfFailed(_device->CreateDeviceContext(deviceOptions, _context.GetAddressOf()));

    ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory5), (IUnknown**)&_factory));

    // D3D11On12 ��ġ �� ���� ��ü���� �޸� ����
    if (device)
    {
        device->Release();
        device = nullptr;
    }

    if (pDXGIDevice)
    {
        pDXGIDevice->Release();
        pDXGIDevice = nullptr;
    }

    if (factory)
    {
        factory->Release();
        factory = nullptr;
    }

    if (d3d11On12Device)
    {
        d3d11On12Device->Release();
        d3d11On12Device = nullptr;
    }

    if (d3d11Context)
    {
        d3d11Context->Release();
        d3d11Context = nullptr;
    }
}

void TextManager::CreateSolidBrush()
{
    {
        ComPtr<ID2D1SolidColorBrush> brush;
        ThrowIfFailed(_context->CreateSolidColorBrush(ColorF(ColorF::White), &brush));
        _brushMap[FontColor::WHITE] = brush;
    }

    {
        ComPtr<ID2D1SolidColorBrush> brush;
        ThrowIfFailed(_context->CreateSolidColorBrush(ColorF(ColorF::Black), &brush));
        _brushMap[FontColor::BLACK] = brush;
    }
}

