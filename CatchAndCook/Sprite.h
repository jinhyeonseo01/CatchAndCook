#pragma once
#include "Component.h"
#include "RendererBase.h"
#include "SpriteAction.h"

class ActionCommand;

struct SpriteWorldParam
{
	vec3 ndcPos = { 0.0f,0.0f,0.1f };
	float alpha = 1.0f;

	vec2 ndcScale = { 1.0f,1.0f };
	vec2 padding = {};

	vec4 clipingColor;
};

struct SprtieTextureParam
{
	vec2 origintexSize{};
	vec2 texSamplePos{};
	vec2 texSampleSize{};
	vec2 padding;
};

struct SpriteRect
{
	float left;
	float top;
	float right;
	float bottom;
};

class Sprite :public Component, public RenderCBufferSetter
{
public:
	Sprite();
	virtual ~Sprite();

public:
	void Init() override;
	void Start() override;
	void Update()override;
	void Update2()override;
	void Enable()override;
	void Disable()override;
	void RenderBegin() override;
	void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void SetDestroy() override;
	void Destroy() override;
	void SetData(Material* material = nullptr) override;
	void CalculateWorldPos();
public:
	void SetTexture(shared_ptr<Texture> texture);
	void SetSize(const vec2& size);

	void SetLocalPos(const vec3& screenPos);
	void SetWorldPos(const vec3& worldScreenPos);

	void SetUVCoord(const SpriteRect& rect);
	void SetClipingColor(const vec4& color);  // https://imagecolorpicker.com/
	void AddAction(shared_ptr<ActionCommand> action) { _actions.emplace_back(action); };

protected:
	SpriteWorldParam _spriteWorldParam;
	SprtieTextureParam _sprtieTextureParam;

	vec3 _screenLocalPos;
	vec3 _ndcWorldPos;

	vec2 _screenSize;
	vec2 _ndcSize;

	vec2 _firstWindowSize;

	shared_ptr<Texture> _spriteImage;

	friend class ActionFunc;
	vector<shared_ptr<ActionCommand>> _actions;

};


/*****************************************************************
*                                                                *
*                         TextSprite                             *
*                                                                *
******************************************************************/
enum class FontColor;
class TextHandle;

class TextSprite : public Sprite
{

public:
	TextSprite();
	virtual ~TextSprite();

	void Init() override;
	void Start() override;
	void Update()override;
	void Update2()override;
	void Enable()override;
	void Disable()override;
	void RenderBegin() override;
	void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void SetDestroy() override;
	void Destroy() override;
	void SetData(Material* material = nullptr) override;

	void SetTexture(shared_ptr<Texture> texture) = delete;
public:
	void SetText(const wstring& text) { _text = text; _textChanged = true; }
	void CreateObject(int width, int height, const WCHAR* font, FontColor color, float fontsize);

private:
	bool _textChanged = true;
	wstring _text = L"NULL";
	shared_ptr<TextHandle> _textHandle;
	SprtieTextureParam _sprtieTextureParam;
	BYTE* _sysMemory = nullptr;
};


/*****************************************************************
*                                                                *
*                         AnimationSprite                        *
*                                                                *
******************************************************************/

class AnimationSprite : public Sprite
{
public:
	AnimationSprite();
	virtual ~AnimationSprite();

private:
	void SetUVCoord(const SpriteRect& rect) = delete;

public:
	void Init() override;
	void Start() override;
	void Update()override;
	void Update2()override;
	void Enable()override;
	void Disable()override;
	void RenderBegin() override;
	void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void SetDestroy() override;
	void Destroy() override;
	void SetData(Material* material = nullptr) override;

public:
	void PushUVCoord(SpriteRect& rect);
	void SetFrameRate(float frameRate) { _frameRate = frameRate; }

private:
	void AnimationUpdate();

private:
	vector<SprtieTextureParam> _sprtieTextureParam;

private:
	float _frameRate{ 1.0f }; //  애니메이션 진행 속도
	float _currentTime{}; //현재 애니메이션 진행 시간
	int32 _currentFrameIndex{}; // 현재 애니메이션 인덱스
	int32 _maxFrameIndex = 0; // 최대 애니메이션 프레임
};