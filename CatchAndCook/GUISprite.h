#pragma once



struct GUISpriteRenderPack;

class GUISprite
{
public:
	std::shared_ptr<Texture> texture;
	DirectX::SimpleMath::Rectangle uvOffsetSize;

	void SetTexture(const std::shared_ptr<Texture>& texture);
	std::shared_ptr<Texture> GetTexture();
	static std::shared_ptr<GUISprite> Create(const std::shared_ptr<Texture>& texture);
	static std::vector<std::shared_ptr<GUISprite>> Create(const std::shared_ptr<Texture>& texture, Vector2 gridSize);
	static std::vector<std::shared_ptr<GUISprite>> Create(const std::shared_ptr<Texture>& texture, std::vector<DirectX::SimpleMath::Rectangle> rects);

	static DirectX::SimpleMath::Rectangle CalculateRect(Vector2 offset, Vector2 size);
	static DirectX::SimpleMath::Rectangle CalculateUV(Vector2 offset, Vector2 size);
};