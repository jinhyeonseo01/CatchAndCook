#include "pch.h"
#include "ResourceManager.h"
#include "Global.h"
unique_ptr<ResourceManager> ResourceManager::main =nullptr;

//ui[������]-> pass[1��] -> �ٴٱ���[������].
//modelLoad(unity��������) -> TerrianSystem -> SkinnedMesh(animation) [15��]-> //navymesh �ý��� 

void ResourceManager::Init()
{
	CreateDefaultShader();
	CreateDefaultMesh();
	CreateDefaultMaterial();
	CreateDefaultTexture();
}

void ResourceManager::CreateDefaultMesh()
{

}

void ResourceManager::CreateDefaultShader()
{
	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->Init(L"SpriteShader.hlsl", SpriteProp, ShaderArg{}, info);
		Add<Shader>(L"SpriteShader", shader);
	}

}

void ResourceManager::CreateDefaultMaterial()
{
}

void ResourceManager::CreateDefaultTexture()
{
	_noneTexture = Load<Texture>(L"None", L"Textures/Config/noneTexture.png");
}
