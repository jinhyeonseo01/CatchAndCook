#pragma once
#include "Component.h"

struct ProgressCycleData
{
	float answer = 0.5f;
	float answerRange = 0.13f;          
	float currentUV = 0.0f;            
	float answerCorrectRange = 0.1f;  

	bool IsCorrect() const 
	{
		float lower = answer - answerRange * 0.5f;
		float upper = answer + answerRange * 0.5f;
		return currentUV- answerCorrectRange*0.5f >= lower && currentUV + answerCorrectRange *0.5f <= upper;
	}
};
class ProgressCycleComponent : public Component , public RenderCBufferSetter
{

public:

	void Init() override;
	void Start() override;
	void Update() override;
	void Update2() override;
	void Enable() override;
	void Disable() override;
	void RenderBegin() override;
	void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current) override;
	void SetDestroy() override;
	void Destroy() override;

	void SetData(Material* material = nullptr) override;
	
public:
	bool isInAnswer();
	void CalculateCurrentUV();
	void Reset();

private:
	bool _isGoingUp = true;
	ProgressCycleData _data;

};

