#pragma once

#include "BufferManager.h"
#include "IGuid.h"
#include "BufferPool.h"
#include "ICBufferInjector.h"
#include "RenderTarget.h"

class Shader;
class Texture;


class Material : public IGuid
{
public:
	Material() ;
	Material(bool useMaterialParams) : _useMaterialParams(useMaterialParams) {};
	virtual ~Material();

	void SetShader(std::shared_ptr<Shader> shader)
	{
		_shader = shader;
		_shaderInjectors.clear();

		for (auto& type : shader->_cbufferInjectorTypes)
			_shaderInjectors.push_back(InjectorManager::main->Get(type));
	}

	void SetPass(RENDER_PASS::PASS pass) { _pass = pass; };
	RENDER_PASS::PASS& GetPass() { return _pass; };

	void SetStencilIndex(int index) { _stencilIndex = index; };
	int GetStencilIndex() const { return _stencilIndex; };

	/*void SetHandle(std::string name, D3D12_CPU_DESCRIPTOR_HANDLE& handle);*/
	void SetTexture(std::string name, std::shared_ptr<Texture> texture);
	shared_ptr<Shader>& GetShader() { return _shader; }

public:
	void AllocTextureTable();
	void AllocTextureLongTable();
	void PushData();
	void SetData();
	void PushHandle();
	void PushLongHandle();
	bool CheckHandle();
	bool CheckLongHandle();
	void SetUseMaterialParams(bool use) { _useMaterialParams = use; }

	void SetInjector(const std::vector<std::shared_ptr<ICBufferInjector>>& injectors) { _customInjectors = injectors; }
	void SetPreDepthNormal(bool preDepthNormal) { _preDepthNormal = preDepthNormal; }
	bool GetPreDepthNormal() const { return _preDepthNormal; }
	void SetShadowCasting(bool shadowCasting) { _shadowCasting = shadowCasting; }
	bool GetShadowCasting() const { return _shadowCasting; }
	void SetSetDataOff(bool setDataOff) { _setDataOff = setDataOff; }
	bool GetSetDataOff() const { return _setDataOff; }

	int GetPropertyInt(const std::string& name) { return _propertyInts[name]; };
	void SetPropertyInt(const std::string& name, int data) { _propertyInts[name] = data; };
	float GetPropertyFloat(const std::string& name) { return _propertyFloats[name]; };
	void SetPropertyFloat(const std::string& name, float data) { _propertyFloats[name] = data; };
	vec4 GetPropertyVector(const std::string& name);
	void SetPropertyVector(const std::string& name, const vec4& data) { _propertyVectors[name] = data; };
	Matrix GetPropertyMatrix(const std::string& name) { return _propertyMatrixs[name]; };
	void SetPropertyMatrix(const std::string& name, const Matrix& data) { _propertyMatrixs[name] = data; };

	shared_ptr<Texture>& GetPropertyTexture(const std::string& name) { return _propertyTexture[name]; };

	bool HasPropertyInt(const std::string& name);
	bool HasPropertyFloat(const std::string& name);
	bool HasPropertyVector(const std::string& name);
	bool HasPropertyMatrix(const std::string& name);

	static void AllocDefualtTextureHandle();

	void SetBlendFactor(const std::array<float, 4>& factor) { _blendFactor = factor; }
	const std::array<float, 4>& GetBlendFactor() const { return _blendFactor; }

	std::shared_ptr<Material> Clone();
	void CopyProperties(std::shared_ptr<Material>& dest);

	TableContainer _tableContainer;
	TableContainer _tableLongContainer;
	static TableContainer _defualtTableContainer;

	uint32 GetID() {
		return _instanceID;
	}

public:
	std::array<float, 4> _blendFactor = { 0.8f, 0.8f, 0.8f, 0.8f };
	uint32 _instanceID = 0;
private:
	shared_ptr<Shader> _shader;

	std::unordered_map<std::string, int> _propertyInts; // Bool 값
	std::unordered_map<std::string, float> _propertyFloats; // 0~1 스,무스니스 정보
	std::unordered_map<std::string, vec4> _propertyVectors; // Color 
	std::unordered_map<std::string, Matrix> _propertyMatrixs; // 
	std::unordered_map<std::string, D3D12_CPU_DESCRIPTOR_HANDLE> _propertyHandle;
	std::unordered_map<std::string, std::shared_ptr<Texture>> _propertyTexture;

	CBufferContainer* _cbufferContainer;
	bool _useMaterialParams = false;
	RENDER_PASS::PASS _pass = RENDER_PASS::Forward;
	bool _preDepthNormal = false;
	bool _shadowCasting = true;
	int _stencilIndex = 0;

	bool _setDataOff = false;

	static uint32 _instanceIDGenator;

	std::vector<std::shared_ptr<ICBufferInjector>> _customInjectors;
	std::vector<std::shared_ptr<ICBufferInjector>> _shaderInjectors;
};

//
//class TestSubMaterialParamInjector : public ICBufferInjector
//{
//public:
//	~TestSubMaterialParamInjector() override = default;
//	const char* cbufferName = "TestSubMaterialParam";
//	TestSubMaterialParam data;
//	CBufferContainer* _cbufferContainer;
//
//	void Inject(const std::any& originalSource) override
//	{
//		TestSubMaterialParam& param = data;
//		const std::shared_ptr<Material>& source = std::any_cast<const std::shared_ptr<Material>&>(originalSource);
//		param.uv = Vector2(source->GetPropertyVector("uv"));
//		_cbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::MateriaSubParam)->Alloc(1);
//		memcpy(_cbufferContainer->ptr, &param, sizeof(TestSubMaterialParam));
//	}
//
//	void AddAtData(const std::shared_ptr<Shader>& shader) override
//	{
//		int textAllocator = GetStaticRegisterIndex();
//		if (shader) textAllocator = shader->GetRegisterIndex(cbufferName);
//		if (textAllocator != -1) Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(
//			textAllocator, _cbufferContainer->GPUAdress);
//	}
//};