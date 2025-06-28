#include "pch.h"
#include "SkinnedHierarchy.h"

#include "Animation.h"
#include "AnimationListComponent.h"
#include "AnimationNode.h"
#include "Bone.h"
#include "Gizmo.h"
#include "ModelNode.h"
#include "SkinnedMeshRenderer.h"
#include "Transform.h"



std::unordered_map<std::string, std::string> SkinnedHierarchy::_boneNameToHumanMappingTable;

SkinnedHierarchy::~SkinnedHierarchy()
{
}

bool SkinnedHierarchy::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void SkinnedHierarchy::Init()
{
	Component::Init();
	for(auto&matrix : _boneOffsetMatrixList)
		matrix = Matrix::Identity;
	for (auto& matrix : _finalMatrixList)
		matrix = Matrix::Identity;
}

void SkinnedHierarchy::Start()
{
	Component::Start();



	//for (auto& renderer : GetOwner()->GetComponentsWithChilds<SkinnedMeshRenderer>())
	//	renderer->AddCbufferSetter(GetCast<SkinnedHierarchy>());
	for (auto& renderer : GetOwner()->GetComponentsWithChilds<SkinnedMeshRenderer>())
		renderer->AddStructuredSetter(GetCast<SkinnedHierarchy>(), BufferType::BoneParam);

	//SetAnimation(GetOwner()->GetComponentsWithChilds<SkinnedMeshRenderer>()[0]->_model->_animationList[0]);
}

void SkinnedHierarchy::Update()
{
	Component::Update();
	//auto o = _rootBone.lock();
}


void SkinnedHierarchy::Update2()
{
	Component::Update2();


	if (IsPlay())
	{
		if (_animation)
		{
			AnimateBlend(_animation, _nextAnimation, _duration);
		}

		// 후처리
		if (_animation)
		{
			_prevAnimationTime = _animationTime;
			_animationTime += Time::main->GetDeltaTime() * _animation->_speedMultiplier * _speedMultiple;

			if (_animation->IsEndTime(_animation->CalculateTime(_animationTime))) // 루프가 아닐때 멈추기
				Stop();
		}

		if (_nextAnimation)
		{
			_prevAnimationBlendTime = _animationBlendTime;
			_animationBlendTime += Time::main->GetDeltaTime() * _nextAnimation->_speedMultiplier * _speedMultiple; //애니메이션 타임 흐름 재생
		}
	}
}


void SkinnedHierarchy::Animate(const std::shared_ptr<Animation>& animation, double time)
{
	if (animation == nullptr)
		return;

	animation->_isLoop = true;
	auto finalTime = animation->CalculateTime(time);

	for (auto& animNode : animation->_nodeLists)
	{
		auto it = FindByName(animNode->GetNodeName(), nodeObjectTable);
		if (it != nodeObjectTable.end())
		{
			auto obj = it->second.lock();
			
			auto q1 = animNode->CalculateRotation(finalTime);
			auto p1 = animNode->CalculatePosition(finalTime);
			auto s1 = animNode->CalculateScale(finalTime);

			bool finalRoot = animNode->IsRoot();
			if (animation->_isApplyTransform && ((animNode->GetNodeName() == _model->_rootBoneNode->GetOriginalName())))
			{
				p1 = Vector3(0, p1.y, 0);
				//q1 = Quaternion::Identity;
			}

			Matrix finalMatrix = Matrix::CreateScale(s1) *
				Matrix::CreateFromQuaternion(q1 * animNode->offsetPreRotation) *
				Matrix::CreateTranslation(p1);
			obj->_transform->SetLocalSRTMatrix(finalMatrix);
		}
	}


	/*
	if (_animation == nullptr)
		return;
	auto finalTime = _animation->CalculateTime(time);
	for (auto& animNode : _animation->_nodeLists)
	{

		//auto it = nodeObjectTable.find(animNode->GetNodeName());
		auto it = FindByName(animNode->GetNodeName(), nodeObjectTable);
		auto it2 = FindByName(animNode->GetNodeName(), _model->_nameToOriginalNodeTable);
		auto it3 = FindByName(animNode->GetNodeName(), animNode->_animation.lock()->_model.lock()->_nameToOriginalNodeTable);
		if (it != nodeObjectTable.end())
		{
			auto obj = it->second.lock();
			std::shared_ptr<ModelNode> currentModelNode;
			std::shared_ptr<ModelNode> animModelNode;
			if (it2 != _model->_nameToOriginalNodeTable.end())
				currentModelNode = it2->second;
			if (it3 != animNode->_animation.lock()->_model.lock()->_nameToOriginalNodeTable.end())
				animModelNode = it3->second;
			
			if (obj != nullptr) {
				auto transform = obj->_transform;
				auto matrix = animNode->CalculateTransformMatrixMapping(currentModelNode, animModelNode, finalTime, _animation->_isApplyTransform && animNode->IsRoot());

				animNode->CalculatePosition(finalTime);

				transform->SetLocalSRTMatrix(matrix);
			}
		}
	}
	*/
}

double SkinnedHierarchy::AnimateBlend(const std::shared_ptr<Animation>& currentAnim, const std::shared_ptr<Animation>& nextAnim, const double& duration)
{
	if (!currentAnim)
		return 0;

	if (nextAnim != nullptr)
	{
		_blendEnd = false;
	}

	double currentAnimTime;
	double nextAnimTime;
	auto blendInterpolValue = 0.0;

	if (currentAnim) {
		currentAnimTime = currentAnim->CalculateTime(_animationTime);
	}
	if (nextAnim) {
		nextAnimTime = nextAnim->CalculateTime(_animationBlendTime);
		blendInterpolValue = std::clamp(_animationBlendTime / duration, 0.0, 1.0);
	}

	auto modelRootBoneName = _model->_rootBoneNode->GetOriginalName();
	auto rootBoneName = modelRootBoneName;
	string nextRootBoneName;
	if (_animation != nullptr && _animation->_rootBoneNode)
		rootBoneName = _animation->_rootBoneNode->GetNodeName();
	if (_nextAnimation != nullptr && _nextAnimation->_rootBoneNode)
		nextRootBoneName = _nextAnimation->_rootBoneNode->GetNodeName();

	for (auto& animNode : currentAnim->_nodeLists)
	{
		auto it = FindByName(animNode->GetNodeName(), nodeObjectTable);

		Quaternion finalAnim_interpolatedRotation;
		Vector3 finalAnim_interpolatedPosition;
		Vector3 finalAnim_interpolatedScale;

		finalAnim_interpolatedRotation = animNode->CalculateRotation(currentAnimTime);
		finalAnim_interpolatedPosition = animNode->CalculatePosition(currentAnimTime);
		finalAnim_interpolatedScale = animNode->CalculateScale(currentAnimTime);

		if (nextAnim)
		{
			auto nextAnimNodeIter = FindByName(animNode->GetNodeName(), nextAnim->_nodeTables);
			if (nextAnimNodeIter != nextAnim->_nodeTables.end())
			{
				auto nextAnimNode = nextAnimNodeIter->second;

				Quaternion nextAnim_interpolatedRotation = nextAnimNode->CalculateRotation(nextAnimTime);
				Vector3 nextAnim_interpolatedPosition = nextAnimNode->CalculatePosition(nextAnimTime);
				Vector3 nextAnim_interpolatedScale = nextAnimNode->CalculateScale(nextAnimTime);

				Vector3::Lerp(finalAnim_interpolatedPosition, nextAnim_interpolatedPosition, blendInterpolValue, finalAnim_interpolatedPosition);
				Vector3::Lerp(finalAnim_interpolatedScale, nextAnim_interpolatedScale, blendInterpolValue, finalAnim_interpolatedScale);
				Quaternion::Slerp(finalAnim_interpolatedRotation, nextAnim_interpolatedRotation, blendInterpolValue,	finalAnim_interpolatedRotation);
			}
		}
		
		bool finalRoot = animNode->IsRoot();
		bool isFinalBone = currentAnim->_isApplyTransform && ((animNode->GetNodeName() == rootBoneName) || (animNode->GetNodeName() == nextRootBoneName));
		if (isFinalBone)
		{
			finalAnim_interpolatedPosition = finalAnim_interpolatedPosition * rootMoveLock;
		}

		Matrix finalMatrix = Matrix::CreateScale(finalAnim_interpolatedScale) *
			Matrix::CreateFromQuaternion(finalAnim_interpolatedRotation * animNode->offsetPreRotation) *
			Matrix::CreateTranslation(finalAnim_interpolatedPosition);

		if (it != nodeObjectTable.end())
		{
			it->second.lock()->_transform->SetLocalSRTMatrix(finalMatrix);
		}
		else
		{
			auto it2 = FindByName(modelRootBoneName, nodeObjectTable);
			if (it2 != nodeObjectTable.end()) //animNode->GetNodeName() == rootBoneName && 
			{
				it2->second.lock()->_transform->SetLocalSRTMatrix(finalMatrix);
			}
		}
		
	}

   
	if (blendInterpolValue >= 1)
	{
		_animationTime = _animationBlendTime;
		_prevAnimationTime = _prevAnimationBlendTime;
		_animation = _nextAnimation;

		_nextAnimation = nullptr;
		_animationBlendTime = 0;
		_prevAnimationBlendTime = 0;
		_blendEnd = true;
	}

	return blendInterpolValue;
}

Vector3 SkinnedHierarchy::BlendDeltaPosition(const std::string& name,const std::shared_ptr<Animation>& currentAnim, const std::shared_ptr<Animation>& nextAnim, const double& duration)
{
	Vector3 finalAnim_interpolatedPosition = Vector3::Zero;

	if (!currentAnim)
		return finalAnim_interpolatedPosition;

	double currentAnimTime;
	double nextAnimTime;
	double currentAnimPrevTime;
	double nextAnimPrevTime;

	auto blendInterpolValue = 0.0;

	string modelRootBoneName = name;
	string rootBoneName = name;
	string nextRootBoneName = name;

	if (currentAnim != nullptr && currentAnim->_rootBoneNode)
		rootBoneName = currentAnim->_rootBoneNode->GetNodeName();
	if (nextAnim != nullptr && nextAnim->_rootBoneNode)
		nextRootBoneName = nextAnim->_rootBoneNode->GetNodeName();


	if (currentAnim) {
		currentAnimTime = currentAnim->CalculateTime(_animationTime);
		currentAnimPrevTime = currentAnim->CalculateTime(_prevAnimationTime);
	}
	if (nextAnim) {
		nextAnimTime = nextAnim->CalculateTime(_animationBlendTime);
		nextAnimPrevTime = currentAnim->CalculateTime(_prevAnimationBlendTime);
		blendInterpolValue = std::clamp(_animationBlendTime / duration, 0.0, 1.0);
	}
	//std::cout << blendInterpolValue << "\n";

	auto animNodeIter = FindByName(rootBoneName, currentAnim->_nodeTables);
	auto it = FindByName(rootBoneName, nodeObjectTable);
	
	if (animNodeIter != currentAnim->_nodeTables.end())
	{
		auto animNode = animNodeIter->second;

		finalAnim_interpolatedPosition = animNode->CalculateDeltaPosition(currentAnimPrevTime, currentAnimTime);

		if (nextAnim)
		{
			std::unordered_map<std::string, shared_ptr<AnimationNode>>::iterator nextAnimNodeIter;

			nextAnimNodeIter = FindByName(nextRootBoneName, nextAnim->_nodeTables);
			
			if (nextAnimNodeIter != nextAnim->_nodeTables.end())
			{
				auto nextAnimNode = nextAnimNodeIter->second;

				Vector3 nextAnim_interpolatedPosition = nextAnimNode->CalculateDeltaPosition(nextAnimPrevTime, nextAnimTime);
				Vector3::Lerp(finalAnim_interpolatedPosition, nextAnim_interpolatedPosition, blendInterpolValue, finalAnim_interpolatedPosition);
			}
		}
	}
	return finalAnim_interpolatedPosition;
}

void SkinnedHierarchy::Play(const std::shared_ptr<Animation>& animation, const double& duration)
{
	if (!_isPlaying || _animation == nullptr)
	{
		_animation = animation;
		_animationBlendTime = 0;
		_prevAnimationBlendTime = 0;

		_duration = std::max(duration, 0.001);
		_isPlaying = true;
		return;
	}

	if (_animation == animation)
		return;

	if (_nextAnimation == animation)
		return;

	_nextAnimation = animation;
	_animationBlendTime = 0;
	_prevAnimationBlendTime = 0;

	_duration = std::max(duration, 0.001);
	_isPlaying = true;
}

void SkinnedHierarchy::Play()
{
	_isPlaying = true;
}

void SkinnedHierarchy::Stop()
{
	_isPlaying = false;
	_animationTime = 0;
	_animationBlendTime = 0;
	_prevAnimationTime = 0;
	_prevAnimationBlendTime = 0;
}

void SkinnedHierarchy::Pause()
{
	_isPlaying = false;
}




void SkinnedHierarchy::Enable()
{
	Component::Enable();
}

void SkinnedHierarchy::Disable()
{
	Component::Disable();
}

void SkinnedHierarchy::RenderBegin()
{
	Component::RenderBegin();

	//GetOwner()->_transform->TopDownLocalToWorldUpdate(Matrix::Identity, false);

	Matrix currentMatrix;
	Matrix parentMatrix = Matrix::Identity;
	bool parentUpdated = false;
	if (auto parent = GetOwner()->GetParent())
	{
		parentUpdated = parent->_transform->GetLocalToWorldMatrix_BottomUp(currentMatrix);
		parentMatrix = parent->_transform->_localToWorldMatrix;
	}
	GetOwner()->_transform->TopDownLocalToWorldUpdate(parentMatrix, parentUpdated);

	int boneCount = _boneNameList.size();
	for (int i = 0; i < boneCount; i++)
	{
		_finalMatrixList[i] = _boneOffsetMatrixList[i];
		if (auto current = _boneNodeList[i].lock()) {
			_finalMatrixList[i] = _finalMatrixList[i] * current->_transform->_localToWorldMatrix;
			_finalMatrixList[i].Invert(_finalInvertMatrixList[i]);
		}
	}
	_boneCBuffer = Core::main->GetBufferManager()->GetBufferPool(BufferType::BoneParam)->Alloc(1);
	memcpy(&(static_cast<BoneParam*>(_boneCBuffer->ptr)->boneMatrixs), _finalMatrixList.data(), sizeof(Matrix) * boneCount);
	memcpy(&(static_cast<BoneParam*>(_boneCBuffer->ptr)->boneInvertMatrixs), _finalInvertMatrixList.data(),sizeof(Matrix) * boneCount);
}
void SkinnedHierarchy::SetData(StructuredBuffer* buffer, Material* material)
{
	int boneCount = _boneNameList.size();
	auto matrixAddress = buffer->GetDataAddress<Matrix>(_finalMatrixList.size() + _finalInvertMatrixList.size());
	memcpy(matrixAddress, _finalMatrixList.data(), sizeof(Matrix) * boneCount);
	memcpy(matrixAddress + _finalMatrixList.size(), _finalInvertMatrixList.data(), sizeof(Matrix) * boneCount);
}

void SkinnedHierarchy::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider,other);
}

void SkinnedHierarchy::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider,other);
}

void SkinnedHierarchy::SetDestroy()
{
	Component::SetDestroy();
}

void SkinnedHierarchy::Destroy()
{
	Component::Destroy();
	for (auto& renderer : GetOwner()->GetComponentsWithChilds<SkinnedMeshRenderer>())
		renderer->RemoveCbufferSetter(GetCast<SkinnedHierarchy>());
	for (auto& renderer : GetOwner()->GetComponentsWithChilds<SkinnedMeshRenderer>())
		renderer->RemoveStructuredSetter(GetCast<SkinnedHierarchy>());
}

Vector3 SkinnedHierarchy::GetDeltaPosition()
{
	if (IsPlay())
	{
		auto name = _model->_rootBoneNode->GetOriginalName();
		if (_animation != nullptr && _animation->_rootBoneNode != nullptr)
			name = _animation->_rootBoneNode->GetNodeName();

		auto pos = BlendDeltaPosition(name, _animation, _nextAnimation, _duration);
		pos *= Vector3::One - rootMoveLock;
		pos = Vector3::Transform(pos, GetOwner()->_transform->GetWorldRotation());
		return pos;
	}
	return Vector3::Zero;
}

void SkinnedHierarchy::SetBoneList(const std::vector<std::shared_ptr<Bone>>& bones)
{
	_boneNameList.clear();
	for (int i = 0; i < bones.size(); i++)
	{
		auto& bone = bones[i];
		_boneNameList.push_back(to_wstring(bone->GetName()));
		_boneOffsetMatrixList[i] = bone->GetTransformMatrix();
	}
}

void SkinnedHierarchy::SetNodeList(const std::vector<std::shared_ptr<ModelNode>>& nodes)
{
	_nodeNameList.clear();
	for(auto& name : nodes)
		_nodeNameList.push_back(to_wstring(name->GetName()));
}

void SkinnedHierarchy::SetModel(const std::shared_ptr<Model>& model)
{
	if (_model != model)
	{
		_model = model;
		SetNodeList(model->GetNodeList());
		SetBoneList(model->GetBoneList());
		FindNodeObjects();
	}
}

void SkinnedHierarchy::SetMappingTable(const std::unordered_map<std::string, std::string>& table)
{
	_boneHumanNameTable = table;
}

//void SkinnedHierarchy::PushData()
//{
	//GetOwner()->_transform->TopDownLocalToWorldUpdate(Matrix::Identity, false);

	//_boneCBuffer = Core::main->GetBufferManager()->GetBufferPool(BufferType::BoneParam)->Alloc(1);
	//int boneCount = _boneNameList.size();
	//for (int i = 0; i < boneCount; i++)
	//{
	//	_finalMatrixList[i] = _boneOffsetMatrixList[i];
	//	if (auto current = _boneNodeList[i].lock()) {
	//		_finalMatrixList[i] = _finalMatrixList[i] * current->_transform->_localToWorldMatrix;
	//		_finalMatrixList[i].Invert(_finalInvertMatrixList[i]);
	//	}
	//}
	//memcpy(&(static_cast<BoneParam*>(_boneCBuffer->ptr)->boneMatrixs), _finalMatrixList.data(), sizeof(Matrix) * boneCount);
	//memcpy(&(static_cast<BoneParam*>(_boneCBuffer->ptr)->boneInvertMatrixs), _finalInvertMatrixList.data(),sizeof(Matrix) * boneCount);
//}

void SkinnedHierarchy::SetData(Material* material)
{
	int index = material->GetShader()->GetRegisterIndex("BoneParam");
	if(index != -1)
		Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(index, _boneCBuffer->GPUAdress);
}

void SkinnedHierarchy::SetAnimation(const std::shared_ptr<Animation>& animation)
{
	this->_animation = animation;
}

std::shared_ptr<Animation> SkinnedHierarchy::GetAnimation()
{
	return _animation;
}

void SkinnedHierarchy::FindNodeObjects()
{
	//�̸����� ��Ī�ؼ� ã�ƿ���
	std::vector<std::shared_ptr<GameObject>> obj;
	for (int index = 0; index < _boneNameList.size(); index++)
	{
		auto& boneName = _boneNameList[index];
		{
			obj.clear();
			GetOwner()->GetChildsAllByName(boneName, obj);
			if (!obj.empty())
				_boneNodeList[index] = obj[0];
		}
	}
	for (int index = 0; index < _nodeNameList.size(); index++)
	{
		auto name = _nodeNameList[index];
		obj.clear();
		GetOwner()->GetChildsAllByName(name, obj);
		if (!obj.empty())
		{
			nodeObjectTable[to_string(name)] = obj[0];
			nodeObjectList.push_back(obj[0]);
		}
	}
	for (auto& node : nodeObjectList)
		if (node.lock() != nullptr && node.lock()->GetParent() != nullptr)
			if (node.lock()->GetParent() == GetOwner())
				_rootBone = node;
}
