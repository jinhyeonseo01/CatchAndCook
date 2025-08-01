#pragma once

#include <any>

#include "BufferManager.h"
#include "Core.h"
#include "Shader.h"

class Material;

//이름을 가지고 매칭하는 Dynamic CBuffer들에 대해서 CBuffer를 주입하는 기능.
class ICBufferInjector
{
private:
    int _staticIndex = -1;
    std::any source;
public:

    virtual ~ICBufferInjector() = default;
    virtual void Inject(const std::any& source){ };
    virtual void SetData(const std::shared_ptr<Shader>& shader){};
    //Shader 없이 상수 위치에 넣어주기 위한 설정

    void SetStaticRegisterIndex(int index) {_staticIndex = index; }
    int GetStaticRegisterIndex() { return _staticIndex; }
};


//인젝터를 생성하는 헬퍼 디파인
#define CBUFFER_INJECTOR(CBUFFER_NAME, STRUCT_TYPE, STRUCT_COUNT, CBUFFER_TYPE, SOURCE_TYPE, CODE) \
class STRUCT_TYPE##Injector : public ICBufferInjector { \
public: \
	~STRUCT_TYPE##Injector() override = default;\
	const char* cbufferName = CBUFFER_NAME;\
    CBufferContainer* _cbufferContainer; \
    void Inject(const std::any& originalSource) override { \
		STRUCT_TYPE data; \
        const SOURCE_TYPE& source = std::any_cast<const SOURCE_TYPE&>(originalSource); \
        CODE \
        _cbufferContainer = Core::main->GetBufferManager()->CreateAndGetBufferPool(CBUFFER_TYPE, sizeof(STRUCT_TYPE), STRUCT_COUNT)->Alloc(1);\
        memcpy(_cbufferContainer->ptr, &data, sizeof(STRUCT_TYPE));\
    } \
	void SetData(const std::shared_ptr<Shader>& shader) override { \
		int textAllocator = GetStaticRegisterIndex();\
        if(shader)\
			textAllocator = shader->GetRegisterIndex(cbufferName); \
        if (textAllocator != -1) \
			Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(textAllocator, _cbufferContainer->GPUAdress);\
	}\
};\
class STRUCT_TYPE##InjectRegistrar { \
public:\
    STRUCT_TYPE##InjectRegistrar() { \
		InjectorManager::_injectTable[CBUFFER_TYPE] = [](){ return std::make_shared<STRUCT_TYPE##Injector>();};\
	}\
}; \
static STRUCT_TYPE##InjectRegistrar global_##STRUCT_TYPE##InjectRegistrar; \

//사용 예제
/*
CBUFFER_INJECTOR("DefaultMaterialParam", DefaultMaterialParam, 256, BufferType::DefaultMaterialParam, std::shared_ptr<Material>,
    // data <- source
    data.color = Vector4(source->GetPropertyVector("_Color"));
    data._baseMapST = Vector4(source->GetPropertyVector("_BaseMap_ST"));
)
*/

// 인젝터들을 모아두는 함수
class InjectorManager
{
public:
    static std::unique_ptr<InjectorManager> main;

    static std::unordered_map<BufferType,std::function<std::shared_ptr<ICBufferInjector>()>> _injectTable;

    void Init() { _injectTable.reserve(256); }
    ~InjectorManager(){
        _injectTable.clear();
    }

    std::shared_ptr<ICBufferInjector> Get(BufferType type) { return _injectTable[type](); };
};