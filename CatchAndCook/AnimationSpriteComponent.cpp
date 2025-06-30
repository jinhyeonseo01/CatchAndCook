#include "pch.h"
#include "AnimationSpriteComponent.h"

void AnimationSpriteComponent::Init()
{
}

void AnimationSpriteComponent::Start()
{
	if (auto& renderer = GetOwner()->GetRenderer())
	{
		renderer->AddCbufferSetter(static_pointer_cast<AnimationSpriteComponent>(shared_from_this()));
	}

}

void AnimationSpriteComponent::Update()
{
	float dt = Time::main->GetDeltaTime();

	_currentTime += dt;

	float frameDuration = _frameRate / static_cast<float>(_maxFrameIndex);

	if (_currentTime >= frameDuration)
	{
		_currentTime -= frameDuration;

		_currentFrameIndex++;

		if (_currentFrameIndex >= _maxFrameIndex)
		{
			if (_roop == false)
				GetOwner()->SetActiveSelf(false);

			_currentFrameIndex = 0;
		}
	}

}

void AnimationSpriteComponent::Update2()
{
}

void AnimationSpriteComponent::Enable()
{
}

void AnimationSpriteComponent::Disable()
{
}

void AnimationSpriteComponent::RenderBegin()
{
}

void AnimationSpriteComponent::RenderEnd()
{
}

void AnimationSpriteComponent::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void AnimationSpriteComponent::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void AnimationSpriteComponent::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
}

void AnimationSpriteComponent::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
}

void AnimationSpriteComponent::SetDestroy()
{
}

void AnimationSpriteComponent::Destroy()
{
}

void AnimationSpriteComponent::SetData(Material* material)
{
	
	auto it = _spriteImages.find(_currentFrameIndex);

	if (it != _spriteImages.end())
	{
		material->SetTexture("_BaseMap", it->second);
	}
	else 
	{
		cout << "이미지없음" << endl;
	}
}

void AnimationSpriteComponent::SetTextures(vector<shared_ptr<Texture>>& textures)
{
	for (int i = 0; i < textures.size(); ++i)
	{
		_spriteImages[i] = textures[i];
	}

	_maxFrameIndex = textures.size();

	cout << "스프라이트 애니메이션 갯수:" << textures.size() << endl;
}

void AnimationSpriteComponent::Reset()
{
	GetOwner()->SetActiveSelf(true);
	_currentFrameIndex = 0;
	_currentTime = 0;
}
