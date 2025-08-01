#pragma once

struct Light;
class Camera;

struct ShadowCasterParams
{
	std::array<Matrix, 4> lightViewMatrix;
	std::array<Matrix, 4> lightProjectionMatrix;
	std::array<Matrix, 4> lightVPMatrix;
	std::array<Matrix, 4> lightInvertViewMatrix;
	std::array<Matrix, 4> lightInvertProjectionMatrix;
	std::array<Matrix, 4> lightInvertVPMatrix;
	unsigned int cascadeCount = 1;
	Vector3 padding;
	Vector4 cascadeDistance;
};
struct ShadowCascadeIndexParams
{
	unsigned int cascadeIndex = 1;
	Vector3 padding;
};


class ShadowManager : public RenderCBufferSetter
{
public:
	bool _onOff = true;
	bool _bakedGIOnOff = true;
	static unique_ptr<ShadowManager> main;
	static const int _cascadeCount = 4;

	ShadowCasterParams _shadowCasterParams;
	ShadowCascadeIndexParams _shadowCascadeIndexParams;

	std::vector<pair<Vector3, Vector3>> _lightTransform; // pos lock

	TableContainer _shadowTable;

	ShadowManager();
	~ShadowManager();
	void Init();
	void SetShadowCasterParams();

	void RenderBegin();
	void RenderEnd();

	std::vector<BoundingFrustum> GetFrustums(Camera* camera, Light* light, const std::vector<float>& distances);
	std::vector<BoundingOrientedBox> CalculateBounds(Camera* camera, Light* light, const std::vector<float>& distances);
	std::vector<BoundingOrientedBox> SeaCalculateBounds(Camera* camera, Light* light, const std::vector<float>& distances);
	void SetData(Material* material) override;
};

