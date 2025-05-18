#pragma once

class Texture;
class Shader;


class ComputeBase
{
public:
	ComputeBase();
	virtual ~ComputeBase();

public:
	virtual void Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture) = 0;
	virtual void DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList) = 0;
	virtual void Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList , int x, int y ,int z)=0;
	virtual void DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList) = 0;

protected:
	virtual void Resize()=0;

protected:
	TableContainer _tableContainer{};

};


class Blur : public ComputeBase
{
public:


	Blur();
	virtual ~Blur();

public:
	virtual void Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture);
	virtual void Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);

private:
	virtual void DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList);
	virtual void DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList);

private:
	virtual void Resize();
private:
	void XBlur(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);
	void YBlur(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x ,int y ,int z);

private:
	int32 _blurCount = 6;

	bool _on =false;

	shared_ptr<Texture> _pingtexture;
	shared_ptr<Texture> _pongtexture;
	shared_ptr<Shader> _XBlurshader;
	shared_ptr<Shader> _YBlurshader;

	friend class ComputeManager;
};

class Bloom : public ComputeBase
{

public:
	Bloom();
	virtual ~Bloom();

public:
	virtual void Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture);
	virtual void Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);

private:
	virtual void DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList);
	virtual void DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList);

private:
	virtual void Resize(); 
private:
	void Black(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);
	void XBlur(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);
	void YBlur(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);
	void Blooming(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);

	bool _on = true;

private:
	int32 _blurCount = 10;
	shared_ptr<Texture> _bloomTexture;
	shared_ptr<Texture> _pingtexture;
	shared_ptr<Texture> _pongtexture;

	shared_ptr<Shader> _XBlurshader;
	shared_ptr<Shader> _YBlurshader;
	shared_ptr<Shader> _BlackShader;
	shared_ptr<Shader> _Bloomshader;
	friend class ComputeManager;
};

struct GodRayParam
{
	Vector4 lightWorldPos;       // 월드 공간의 광원 위치 (dir light 는 far 포인트)
	Vector2 lightScreenUV;       // 미리 계산해 전달: WorldPos → NDC → UV
	int   sampleCount;          // e.g. 50
	float decay;                // e.g. 0.95
	float exposure;             // e.g. 0.3
	Vector2 padding_ray;
};

class GodRay : public ComputeBase
{

public:
	GodRay();
	virtual ~GodRay();

public:
	virtual void Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture);
	virtual void Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);

private:
	virtual void DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList);
	virtual void DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList);

private:
	virtual void Resize();
private:
	void Black(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);
	void Ray(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);

	bool _on = false;

private:
	GodRayParam param;

	shared_ptr<Texture> _rayEmissionTexture;
	shared_ptr<Texture> _pingtexture;
	shared_ptr<Texture> _pongtexture;

	shared_ptr<Shader> _BlackShader;
	shared_ptr<Shader> _RayShader;

	friend class ComputeManager;
};

struct FXAAParams
{
	float  uQualitySubpix;          
	float  uQualityEdge;            
	float  uQualityEdgeThreshold;   
	float  uQualityEdgeThresholdMin;
};


class FXAA : public ComputeBase
{

public:
	FXAA();
	virtual ~FXAA();

public:
	virtual void Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture);
	virtual void Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);

private:
	virtual void DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList);
	virtual void DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList);

private:
	virtual void Resize();
private:

	bool _on = true;

private:

	shared_ptr<Texture> _pingtexture;
	FXAAParams params;
	std::shared_ptr<Shader> _AAShader;

	friend class ComputeManager;
};

struct DOFParam
{
	float g_fogMin = 20;
	float g_fogMax = 100.0f;
	Vector2 padding_dof;

};

class DOF : public ComputeBase
{
public:


	DOF();
	virtual ~DOF();

public:
	virtual void Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture);
	virtual void Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);

private:
	virtual void DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList);
	virtual void DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList);

private:
	virtual void Resize();
private:
	void XBlur(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);
	void YBlur(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);

private:
	int32 _blurCount = 3;

	bool _on = false;

	shared_ptr<Texture> _pingtexture;
	shared_ptr<Texture> _pongtexture;
	shared_ptr<Shader> _XBlurshader;
	shared_ptr<Shader> _YBlurshader;

	friend class ComputeManager;
};




struct FogParam
{
	

	vec3 g_fogColor = vec3(1, 1, 1);
	float power = 1.0f;

	float g_fogMin = 0;
	float g_fogMax = 1000.0f;
	int depthRendering = 0;
	float padding = 0;

};

class DepthRender : public ComputeBase
{
	


public:
	DepthRender();
	virtual ~DepthRender();

public:
	virtual void Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture);
	virtual void Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);

private:
	virtual void DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList);
	virtual void DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList);

private:
	virtual void Resize();

private:
	shared_ptr<Texture> _pingTexture;
	shared_ptr<Shader> _shader;
	FogParam _fogParam;

	friend class ComputeManager;
};

class FieldFogRender : public ComputeBase
{

public:
	FieldFogRender();
	virtual ~FieldFogRender();

public:
	virtual void Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture);
	virtual void Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);

private:
	virtual void DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList);
	virtual void DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList);

private:
	virtual void Resize();

private:
	shared_ptr<Texture> _pingTexture;
	shared_ptr<Shader> _shader;
	FogParam _fogParam;

	bool _onOff = true;

	friend class ComputeManager;
};

struct UnderWaterParam
{
	/*vec3 g_fogColor = vec3(0, 0.256f, 0.350f);*/

	vec3 g_fogColor = vec3(0, 0.269f, 0.438f);
	float g_fog_power = 5.109f;

	vec3 g_underWaterColor = vec3(0.526f, 0.603f, 1.0f);
	float g_fogMin = 0;

	vec2 padding;
	int g_on = 1;
	float g_fogMax = 1467.f;
};

class UnderWaterEffect : public ComputeBase
{

public:
	UnderWaterEffect();
	virtual ~UnderWaterEffect();

public:
	virtual void Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture);
	virtual void Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);

private:
	virtual void DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList);
	virtual void DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList);

private:
	virtual void Resize();

private:
	shared_ptr<Texture> _pingTexture;
	shared_ptr<Texture> _colorGrading;
	shared_ptr<Shader> _shader;

	UnderWaterParam	_underWaterParam;

	friend class ComputeManager;

};

struct VignetteParam
{
	float power = 1;
	Vector3 color = Vector3(0.1,0.04,0.08);
	float startRound = 0.85;
	float Range = 1.4;
	Vector2 padding;

};
class VignetteRender : public ComputeBase
{

public:
	VignetteRender();
	virtual ~VignetteRender();

public:
	virtual void Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture);
	virtual void Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);

private:
	virtual void DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList);
	virtual void DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList);

private:
	virtual void Resize();

private:
	shared_ptr<Texture> _pingTexture;
	shared_ptr<Shader> _shader;

	VignetteParam _vignetteParam;
	Vector3 defualtColor = Vector3(0.1, 0.04, 0.08);
	bool _onOff = true;

	friend class ComputeManager;
};

class SSAORender : public ComputeBase
{

public:
	SSAORender();
	virtual ~SSAORender();

public:
	virtual void Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture);
	virtual void Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);

private:
	virtual void DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList);
	virtual void DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList);

private:
	virtual void Resize();


public:
	std::shared_ptr<Texture> _ssaoTexture;

private:
	shared_ptr<Texture> _pingTexture;
	shared_ptr<Shader> _shader;

	bool ssaoOnOff = true;

	friend class ComputeManager;
};

class ColorGradingRender : public ComputeBase
{

public:
	ColorGradingRender();
	virtual ~ColorGradingRender();

public:
	virtual void Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture);
	virtual void Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);

private:
	virtual void DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList);
	virtual void DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList);

private:
	virtual void Resize();

private:
	shared_ptr<Texture> _pingTexture;
	std::shared_ptr<Texture> _colorGradingTexture;
	shared_ptr<Shader> _shader;

	bool colorGradingOnOff = true;

	friend class ComputeManager;
};



struct ScatteringData
{
	float phaseG=0.453f;
	float absorption =3.6f;
	vec2 Padding;

	float density=15.0f;
	vec3 scatterColor= vec3(0,1,0.375f);

	//vec3 MainlightPos;
	//float padding2;
};

class Scattering : public ComputeBase
{

public:
	Scattering();
	virtual ~Scattering();

public:
	virtual void Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture);
	virtual void Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);

private:
	virtual void DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList);
	virtual void DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList);

private:
	virtual void Resize();

private:
	shared_ptr<Texture> _pingTexture;
	std::shared_ptr<Texture> _ssaoTexture;
	shared_ptr<Shader> _shader;

	bool _scattering = true;
	ScatteringData _scatteringData;

	friend class ComputeManager;
};



class ComputeManager 
{
public:
	static unique_ptr<ComputeManager> main;

public:
	void Init();
	void DispatchAfterDeferred(ComPtr<ID3D12GraphicsCommandList>& cmdList);
	void DispatchMainField(ComPtr<ID3D12GraphicsCommandList>& cmdList);
	void Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList);
public:
	void Resize();
public:

	shared_ptr<Texture> _pingTexture;
	shared_ptr<Texture> _pongTexture;

	shared_ptr<Blur> _blur;
	shared_ptr<Bloom> _bloom;
	shared_ptr<DepthRender> _depthRender;
	shared_ptr<UnderWaterEffect> _underWaterEffect;
	shared_ptr<VignetteRender> _vignetteRender;
	shared_ptr<SSAORender> _ssaoRender;
	shared_ptr<FieldFogRender> _fieldFogRender;
	shared_ptr<ColorGradingRender> _colorGradingRender;
	shared_ptr<GodRay> _godrayRender;
	shared_ptr<FXAA> _fxaaRender;
	shared_ptr<DOF> _dofRender;
	shared_ptr<Scattering> _colorGradingSea;

	bool _mainFieldTotalOn = true;
	// color grading
};

