
#include "pch.h"
#include "TextManager.h"

using namespace D2D1;

unique_ptr<TextManager> TextManager::main=nullptr;

void TextManager::Init()
{
    InitD2D();
    CreateSolidBrush();

}

void TextManager::UpdateToSysMemory(const wstring& text, shared_ptr<TextHandle>& handle, BYTE* memory, int dataSize)
{
    IDWriteTextLayout* textLayout = nullptr;

    //텍스트레이아웃 생성
    ThrowIfFailed(_factory->CreateTextLayout(text.c_str(), text.length(), handle->font.Get(),handle->width, handle->height, &textLayout));

    DWRITE_TEXT_METRICS metrics = {};

    //Rendering To 비트맵
    if (textLayout)
    {
        textLayout->GetMetrics(&metrics);

        // 타겟설정
        _context->SetTarget(handle->bitMapGpu.Get());

        // 안티앨리어싱모드 설정
        _context->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

        // 텍스트 렌더링
        _context->BeginDraw();

        if (handle->fontColor == FontColor::WHITE)
        {
            _context->Clear(D2D1::ColorF(D2D1::ColorF::Black));
        }

        else if (handle->fontColor == FontColor::BLACK)
        {
            _context->Clear(D2D1::ColorF(D2D1::ColorF::White));
        }
        else if(handle->fontColor == FontColor::CUSTOM)
        {
            _context->Clear(D2D1::ColorF(0,0,0,0));
        }

        _context->SetTransform(D2D1::Matrix3x2F::Identity());

        _context->DrawTextLayout(D2D1::Point2F(0.0f, 0.0f), textLayout, handle->brush.Get());

        // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
        // is lost. It will be handled during the next call to Present.
        _context->EndDraw();

        // 안티앨리어싱 모드 복구    
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
        memcpy(pDest, pSrc, width * dataSize);
        pDest += handle->width * dataSize;
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

shared_ptr<TextHandle> TextManager::AllocTextStrcture(int width, int height, const WCHAR* font, FontColor color, float fontsize, vec4 fontColor)
{
    shared_ptr<TextHandle> textHandle = make_shared<TextHandle>();

    textHandle->width = width;
    textHandle->height = height;
    textHandle->fontSize = fontsize;
    textHandle->fontColor = color;
    if(color == FontColor::CUSTOM)
    {
        ComPtr<ID2D1SolidColorBrush> brush;
        ThrowIfFailed(_context->CreateSolidColorBrush(D2D1::ColorF(fontColor.x,fontColor.y ,fontColor.z,fontColor.w), &brush));
        textHandle->brush = brush;
    }
    else
    {
        textHandle->brush = _brushMap[color];
    }
   
    uint32 dpi = ::GetDpiForWindow(Core::main->GetHandle());

    //비트맵 생성
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

    //폰트생성
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

        switch(textHandle->pivotX)
        {
        case TextPivot::Left:
        ThrowIfFailed(textHandle->font.Get()->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING));
        break;
        case TextPivot::Middle:
        ThrowIfFailed(textHandle->font.Get()->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER));
        break;
        case TextPivot::Right:
        ThrowIfFailed(textHandle->font.Get()->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING));
        break;
        }
        switch(textHandle->pivotY)
        {
        case TextPivot::Top:
        ThrowIfFailed(textHandle->font.Get()->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));
        break;
        case TextPivot::Middle:
        ThrowIfFailed(textHandle->font.Get()->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER));
        break;
        case TextPivot::Bottom:
        ThrowIfFailed(textHandle->font.Get()->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR));
        break;
        }
    };

    return textHandle;
}

void TextManager::InitD2D()
{
    // D3D12 장치 및 명령 큐 가져오기.
    auto d3d_device = Core::main->GetDevice().Get();
    auto d3d_cmdQueue = Core::main->GetCmdQueue().Get();

    uint32 d3d11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    D2D1_FACTORY_OPTIONS d2dFactoryOptions = {};  // D2D1 팩토리 옵션 (디버그 레벨 설정)

    // D2D1 팩토리 및 D3D11 관련 인터페이스 포인터 선언
    ID2D1Factory3* factory = nullptr;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* d3d11Context = nullptr;
    ID3D11On12Device* d3d11On12Device = nullptr;

    // D2D1 팩토리에서 디버그 레벨을 설정합니다.
    d3d11DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    d2dFactoryOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;

    // D3D11On12 장치를 생성합니다. D3D12 장치를 사용하여 D3D11 장치를 만든 후, D3D11On12Device를 얻습니다.
    ThrowIfFailed(D3D11On12CreateDevice(
        d3d_device,                           // 기존 D3D12 장치
        d3d11DeviceFlags,                     // D3D11 장치 플래그
        nullptr,                              // 추가적인 전역 리소스 (여기서는 사용하지 않음)
        0,                                     // 전역 리소스 개수
        (IUnknown**)&d3d_cmdQueue,             // D3D12 명령 큐
        1,                                     // 명령 큐 개수
        0,                                     // 큐의 타이밍 (사용하지 않음)
        &device,                               // 반환될 D3D11 장치
        &d3d11Context,                         // 반환될 D3D11 장치 컨텍스트
        nullptr                                // 반환될 DXGI 장치 (여기서는 사용하지 않음)
    ));

    // D3D11On12 장치에서 D3D11 장치 인터페이스를 가져옵니다.
    ThrowIfFailed(device->QueryInterface(IID_PPV_ARGS(&d3d11On12Device)));

    // D2D1 장치 생성 옵션 설정
    D2D1_DEVICE_CONTEXT_OPTIONS deviceOptions = D2D1_DEVICE_CONTEXT_OPTIONS_NONE;

    // D2D1 팩토리를 생성합니다.
    ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &d2dFactoryOptions, (void**)&factory));

    // D3D11On12 장치에서 IDXGIDevice를 얻어옵니다. (D2D가 D3D11On12 장치를 사용할 수 있도록 하기 위함)
    IDXGIDevice* pDXGIDevice = nullptr;
    ThrowIfFailed(d3d11On12Device->QueryInterface(IID_PPV_ARGS(&pDXGIDevice)));

    // D2D1 팩토리에서 D3D11On12 장치를 기반으로 D2D1 장치 생성
    ThrowIfFailed(factory->CreateDevice(pDXGIDevice, _device.GetAddressOf()));

    // D2D1 장치로부터 D2D1 디바이스 컨텍스트 생성
    ThrowIfFailed(_device->CreateDeviceContext(deviceOptions, _context.GetAddressOf()));

    ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory5), (IUnknown**)&_factory));

    // D3D11On12 장치 및 관련 객체들의 메모리 해제
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

