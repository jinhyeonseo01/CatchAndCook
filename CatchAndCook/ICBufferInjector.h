#pragma once

#include <any>

#include "BufferManager.h"
#include "Core.h"
#include "Shader.h"

class Material;

//�̸��� ������ ��Ī�ϴ� Dynamic CBuffer�鿡 ���ؼ� CBuffer�� �����ϴ� ���.
class ICBufferInjector
{
private:
    int _staticIndex = -1;
public:

    virtual ~ICBufferInjector() = default;
    virtual void Inject(const std::any& source) = 0;
    virtual void SetData(const std::shared_ptr<Shader>& shader) = 0;
    //Shader ���� ��� ��ġ�� �־��ֱ� ���� ����

    void SetStaticRegisterIndex(int index) {_staticIndex = index; }
    int GetStaticRegisterIndex() { return _staticIndex; }
};

//�����͸� �����ϴ� ���� ������
#define CBUFFER_INJECTOR(CBUFFER_NAME, STRUCT_TYPE, CBUFFER_TYPE, SOURCE_TYPE, CODE) \
class STRUCT_TYPE##Injector : public ICBufferInjector { \
public: \
	~STRUCT_TYPE##Injector() override = default;\
	const char* cbufferName = CBUFFER_NAME;\
    STRUCT_TYPE data;\
    CBufferContainer* _cbufferContainer; \
    void Inject(const std::any& originalSource) override { \
		STRUCT_TYPE& param = data; \
        const SOURCE_TYPE& source = std::any_cast<const SOURCE_TYPE&>(originalSource); \
        CODE \
        _cbufferContainer = Core::main->GetBufferManager()->GetBufferPool(CBUFFER_TYPE)->Alloc(1);\
        memcpy(_cbufferContainer->ptr, &param, sizeof(STRUCT_TYPE));\
    } \
	void SetData(const std::shared_ptr<Shader>& shader) override { \
		int index = GetStaticRegisterIndex();\
        if(shader)\
			index = shader->GetRegisterIndex(cbufferName); \
        if (index != -1) \
			Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(index, _cbufferContainer->GPUAdress);\
	}\
};\

//��� ����
/*
	CBUFFER_INJECTOR("UserName", TestSubMaterialParam, BufferType::TransformParam, std::shared_ptr<Material> source,
	    source->GetShader();
	)


        const SOURCE_TYPE& source;\
        if(originalSource.a.has_value()) \
            source = const_cast<SOURCE_TYPE&>(std::any_cast<const SOURCE_TYPE&>(originalSource)); \
*/

// �����͵��� ��Ƶδ� �Լ�
class InjectorManager
{
public:
    static std::unique_ptr<InjectorManager> main;

    std::unordered_map<BufferType, std::shared_ptr<ICBufferInjector>> _injectTable;

    void Init() { _injectTable.reserve(256); }

    template <class T, class = std::enable_if_t<std::is_base_of_v<ICBufferInjector, T>>>
    void Register(BufferType type)
    {
        std::shared_ptr<ICBufferInjector> inject = static_pointer_cast<ICBufferInjector>(std::make_shared<T>());
        if (!_injectTable.contains(type))
            _injectTable[type] = inject;
    };

    std::shared_ptr<ICBufferInjector> Get(BufferType type) { return _injectTable[type]; };
};