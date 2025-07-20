#include "pch.h"
#include "ImageRenderer.h"

#include "Canvas.h"
#include "MeshRenderer.h"
#include "RectTransform.h"

ImageRenderer::~ImageRenderer()
{
}

bool ImageRenderer::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void ImageRenderer::Init()
{
	Component::Init();

	auto meshRenderer = GetOwner()->GetComponent<MeshRenderer>();
	if (meshRenderer == nullptr) {
		meshRenderer = GetOwner()->AddComponent<MeshRenderer>();
	}

	_material = std::make_shared<Material>();
	_material->SetShadowCasting(false);
	_material->SetPreDepthNormal(false);
	_material->SetShader(ResourceManager::main->Get<Shader>(L"GUISpriteShader"));
	_material->SetPass(RENDER_PASS::Transparent);

	meshRenderer->AddCbufferSetter(GetCast<ImageRenderer>());
	meshRenderer->AddMesh(GeoMetryHelper::LoadRectCenterOneScaleMesh());
	meshRenderer->AddMaterials({ _material });
	meshRenderer->SetInstancing(false);
	meshRenderer->SetCulling(false);
}

void ImageRenderer::Start()
{
	Component::Start();
	if (auto canvas = GetOwner()->GetComponentWithParents<Canvas>())
	{
		if (canvas->type == CanvasType::Overlay)
		{
			_material->SetShader(ResourceManager::main->Get<Shader>(L"GUISpriteShader_Overlay"));
			_material->SetPass(RENDER_PASS::UI2);
		}
		else
		{
			_material->SetShader(ResourceManager::main->Get<Shader>(L"GUISpriteShader"));
			_material->SetPass(RENDER_PASS::Transparent);
		}
	}
	std::cout << "start" << "\n";
}

void ImageRenderer::Update()
{
	Component::Update();

	auto rect = GetOwner()->GetParent()->GetComponent<RectTransform>();
}

void ImageRenderer::Update2()
{
	Component::Update2();
}

void ImageRenderer::Enable()
{
	Component::Enable();
}

void ImageRenderer::Disable()
{
	Component::Disable();
}

void ImageRenderer::RenderBegin()
{
	Component::RenderBegin();
	if (_sprite->GetTexture())
		_material->SetTexture("_BaseMap", _sprite->GetTexture());
}

void ImageRenderer::RenderEnd()
{
	Component::RenderEnd();
}

void ImageRenderer::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void ImageRenderer::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void ImageRenderer::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void ImageRenderer::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
	Component::ChangeScene(currentScene, nextScene);
}

void ImageRenderer::SetDestroy()
{
	Component::SetDestroy();
}

void ImageRenderer::Destroy()
{
	Component::Destroy();
	GetOwner()->_renderer->RemoveCbufferSetter(GetCast<ImageRenderer>());
}

void ImageRenderer::SetData(Material* material)
{
	
	auto _rectCBuffer = Core::main->GetBufferManager()->GetBufferPool(BufferType::RectTransformParam)->Alloc(1);
	GUISpriteParam param;
	//param
	param.GUISprite_offsetSize = _sprite->GetST();
	param.GUISprite_border = _sprite->GetBorder();             // 그대로 픽셀 단위로 전송
	if (auto rect = GetOwner()->GetComponent<RectTransform>())
	{
		//auto a = rect->_computedRect.absoluteRect.max - rect->_computedRect.absoluteRect.min;
		param.GUISprite_targetSize = rect->_computedRect.absoluteRect.max - rect->_computedRect.absoluteRect.min;
		//param.GUISprite_targetSize = Vector2(10000, 10000);
		//std::cout << to_string(param.GUISprite_targetSize) << "\n";
	}
	// 메시라면 월드→픽셀 환산치
	memcpy(_rectCBuffer->ptr, &param, sizeof(param));

	int index = material->GetShader()->GetRegisterIndex("GUISpriteParam");
	if (index != -1)
		Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(index, _rectCBuffer->GPUAdress);
		
}

