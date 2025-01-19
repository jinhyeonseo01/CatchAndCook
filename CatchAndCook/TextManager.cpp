#pragma once
#include "pch.h"
#include "TextManager.h"

using namespace D2D1;

unique_ptr<TextManager> TextManager::main=nullptr;

void TextManager::Init()
{
    InitD2D();

}

TextHandle* TextManager::AllocTextStrcture(int width, int height, FontColor color, float size)
{
    TextHandle* textHandle = new TextHandle();

    textHandle->width = width;
    textHandle->height = height;
    textHandle->fontSize = size;

    uint32 dpi = ::GetDpiForWindow(Core::main->GetHandle());

    D2D1_BITMAP_PROPERTIES1 bitmapProperties =
        BitmapProperties1(
            D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
            D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            dpi,
            dpi
        );


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

