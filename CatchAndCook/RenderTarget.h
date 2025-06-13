#pragma once
class Texture;

namespace RENDER_PASS
{
	enum PASS
	{
		Shadow = 1 << 0,
		Deferred = 1 << 1,
		Forward = 1 << 2,
		Transparent = 1 << 3,
		PostProcessing = 1 << 4,
		NoEffectPostProcessing = 1 << 5,
		UI = 1 << 6,
		ParticlePass = 1<<7,
		Debug = 1 << 8,
	};

	const int Count = 9;

	inline PASS operator|(PASS a, PASS b) {
		return static_cast<PASS>(static_cast<int>(a) | static_cast<int>(b));
	}


	inline bool HasFlag(PASS value, PASS flag) {
		return (static_cast<int>(value) & static_cast<int>(flag)) != 0;
	}

	inline int ToIndex(PASS pass) 
	{
		switch (pass) {
		case Shadow: return 0;
		case Deferred: return 1;
		case Forward: return 2;
		case Transparent: return 3;
		case PostProcessing: return 4;
		case NoEffectPostProcessing: return 5;
		case UI: return 6;
		case ParticlePass: return 7;
		case Debug: return 8;
		default: assert(false && "Invalid PASS value");
		}
	}
}

class RenderTarget
{

public:
	RenderTarget();
	~RenderTarget();

public:
	void Init(ComPtr<IDXGISwapChain3>& swapchain);
	void ResizeWindowSize(ComPtr<IDXGISwapChain3> swapchain, uint32 swapChainFlags);
	void ChangeIndex();
	void RenderBegin();
	void RenderEnd();
	void ClearDepth();
	void SetRenderTarget();

	shared_ptr<Texture>& GetRenderTarget() { return _RenderTargets[_RenderTargetIndex]; }
	shared_ptr<Texture>& GetDSTexture() { return _DSTexture; };
	//shared_ptr<Texture>& GetInterMediateTexture() {return _intermediateTexture;}
private:
	//[디퍼드렌더링,포워드렌더링] -> [ 중간텍스쳐 ] -> [후처리] -> [렌더타겟]
	ComPtr<IDXGISwapChain3> _swapChain;
	array<shared_ptr<Texture>, SWAP_CHAIN_FRAME_COUNT> _RenderTargets;

	//shared_ptr<Texture> _intermediateTexture;
	shared_ptr<Texture> _DSTexture;
	D3D12_VIEWPORT	_viewport = {};
	D3D12_RECT		_scissorRect = {};
	uint32	_RenderTargetIndex = 0;
};



/*************************
*                        *
*         GBuffer        *
*                        *
**************************/
class Texture;

class GBuffer
{
public:
	static const uint32 _count = 4;

	GBuffer();
	~GBuffer();

	void Init();
	void RenderBegin();
	void RenderEnd();

	shared_ptr<Texture>& GetTexture(int32 index);

private:
	array<shared_ptr<Texture>,_count> _textures = {nullptr,};

	D3D12_VIEWPORT _viewport;
	D3D12_RECT _scissorRect;
};


/*************************
*                        *
*         Shadow         *
*                        *
**************************/

class ShadowBuffer
{
public:
	static const uint32 _count = 4;

	int currentIndex = 0;
	D3D12_VIEWPORT	_viewport = {};
	D3D12_RECT		_scissorRect = {};

	ShadowBuffer();
	~ShadowBuffer();

	void Init();
	void RenderBegin(int index);
	void RenderEnd();

	shared_ptr<Texture>& GetDSTexture(int32 index) { return _DSTextures[index]; };
private:
	array<shared_ptr<Texture>, _count> _DSTextures = { nullptr, };
};



//class VolumetricPass
//{
//public:
//
//	VolumetricPass();
//	~VolumetricPass();
//
//	void Init();
//	void RenderBegin();
//	void RenderEnd();
//
//	shared_ptr<Texture>& GetTexture() { return _texture; }
//
//private:
//	shared_ptr<Texture> _texture;
//
//	D3D12_VIEWPORT _viewport;
//	D3D12_RECT _scissorRect;
//};