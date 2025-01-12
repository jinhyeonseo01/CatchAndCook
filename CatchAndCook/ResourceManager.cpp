#include "pch.h"
#include "ResourceManager.h"

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
	_meshMap.reserve(16);
}

void ResourceManager::CreateDefaultShader()
{
}

void ResourceManager::CreateDefaultMaterial()
{
}

void ResourceManager::CreateDefaultTexture()
{
	_meshMap.reserve(2048);
	_noneTexture = Load<Texture>(L"None", L"Textures/Config/noneTexture.png");
}
