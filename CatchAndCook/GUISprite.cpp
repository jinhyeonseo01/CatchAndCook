#include "pch.h"
#include "GUISprite.h"




void GUISprite::SetTexture(const std::shared_ptr<Texture>& texture)
{
	Vector2 size = texture->GetSize();
	Vector4 offsetSize = Vector4(0, 0, size.x, size.y);
	this->texture = texture;
	//uvOffsetSize = ;
}

std::shared_ptr<Texture> GUISprite::GetTexture()
{
	return this->texture;
}

std::shared_ptr<GUISprite> GUISprite::Create(const std::shared_ptr<Texture>& texture)
{
	return nullptr;
}

std::vector<std::shared_ptr<GUISprite>> GUISprite::Create(const std::shared_ptr<Texture>& texture, Vector2 gridSize)
{
	std::vector<std::shared_ptr<GUISprite>> sprites;
	auto size = texture->GetSize();
	int w = std::round(gridSize.x / size.x);
	int h = std::round(gridSize.y / size.y);

	for (int j = 0; j < h; j++)
	{
		for (int i = 0; i < w; i++)
		{
			auto sprite = std::make_shared<GUISprite>();
			sprite->SetTexture(texture);
			sprite->_rectSize = DirectX::SimpleMath::Rectangle{i * (int)gridSize.x, j * (int)gridSize.y, (int)gridSize.x, (int)gridSize.y};
			sprites.push_back(sprite);
		}
	}
	return sprites;
}

std::vector<std::shared_ptr<GUISprite>> GUISprite::Create(const std::shared_ptr<Texture>& texture,
	std::vector<DirectX::SimpleMath::Rectangle> rects)
{
	std::vector<std::shared_ptr<GUISprite>> sprites;
	for (auto& a : rects)
	{
		auto sprite = std::make_shared<GUISprite>();
		sprite->SetTexture(texture);
		sprite->_rectSize = a;
		sprites.push_back(sprite);
	}
	return sprites;
}

DirectX::SimpleMath::Rectangle GUISprite::CalculateRect(const Vector2 offset, const Vector2& size)
{
	return DirectX::SimpleMath::Rectangle{ static_cast<long>(offset.x), static_cast<long>(offset.y), static_cast<long>(size.x), static_cast<long>(size.y) };
}


DirectX::SimpleMath::Vector4 GUISprite::GetST()
{
	auto size = texture->GetSize();
	Vector2 texSize = size;
	Vector2 rectOffset = Vector2(_rectSize.x, _rectSize.y);
	Vector2 rectSize = Vector2(_rectSize.width, _rectSize.height);


	Vector2 scale = rectSize / texSize;
	Vector2 ofs = rectOffset / texSize;
	//std::cout << to_string(Vector4(scale.x, scale.y, ofs.x, 1 - (ofs.y + scale.y))) << "\n";
	// vector4 에 담기
	return Vector4(scale.x, scale.y, ofs.x, 1 - (ofs.y + scale.y));
}

//uv min max,
