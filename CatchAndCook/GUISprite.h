#pragma once



struct GUISpriteRenderPack;

class GUISprite
{
public:
	std::shared_ptr<Texture> texture;
	DirectX::SimpleMath::Rectangle _rectSize;
	Vector4 _border;

	void SetTexture(const std::shared_ptr<Texture>& texture);
	std::shared_ptr<Texture> GetTexture();
	static std::shared_ptr<GUISprite> Create(const std::shared_ptr<Texture>& texture);
	static std::vector<std::shared_ptr<GUISprite>> Create(const std::shared_ptr<Texture>& texture, Vector2 gridSize);
	static std::vector<std::shared_ptr<GUISprite>> Create(const std::shared_ptr<Texture>& texture, std::vector<DirectX::SimpleMath::Rectangle> rectSizes);

	DirectX::SimpleMath::Rectangle CalculateRect(const Vector2 offset, const Vector2& size);
	DirectX::SimpleMath::Vector4 GetST();
	DirectX::SimpleMath::Vector4 GetBorder();
};