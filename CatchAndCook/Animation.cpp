#include "pch.h"
#include "Animation.h"

#include "AnimationNode.h"
#include "AnimationKeyFrame.h"
#include "ModelNode.h"


Animation::Animation()
{
}

Animation::~Animation()
{
}

void Animation::Init(std::shared_ptr<Model> model, aiAnimation* anim, aiNode* root)
{
	_model = model;

	//root
	SetName(to_wstring(convert_assimp::Format(anim->mName)));
	SetModelName(model->GetName());

	_duration = anim->mDuration;
	_ticksPerSecond = (anim->mTicksPerSecond != 0) ? anim->mTicksPerSecond : 25.0;
	_totalTime = _duration / _ticksPerSecond;

	for (int i = 0; i < anim->mNumChannels; i++)
	{

		auto name = convert_assimp::Format(anim->mChannels[i]->mNodeName);
		string originalName = name;

		if (name.find("$AssimpFbx$") != std::string::npos)
		{
			vector<string> strList = str::split(name, "_");
			vector<string> originalList = str::split(name, "_$AssimpFbx$");
			originalName = originalList[0];
		}

		auto part = _nodeTables[originalName];
		if (part == nullptr)
		{
			part = std::make_shared<AnimationNode>();
			part->SetNodeName(originalName);
			part->Init(shared_from_this());

			_nodeTables[originalName] = part;
			_nodeLists.push_back(part);
		}

		if (model && model->_rootBoneNode && model->_rootBoneNode->GetOriginalName() == originalName)
		{
			_rootBoneNode = part;
			part->SetRoot(true);
		}

		part->SetKeyFrames(anim, anim->mChannels[i]);
	}

	for (auto& part : _nodeTables)
	{
		Vector3 pos, scale;
		Quaternion rot;

		auto findNode = root->FindNode((part.first + "_$AssimpFbx$_Translation").c_str());
		if (findNode != nullptr)
		{
			auto trs = convert_assimp::Format(findNode->mTransformation);
			trs.Decompose(scale, rot, pos);
			part.second->SetOffsetPosition(pos);
		}
		findNode = root->FindNode((part.first + "_$AssimpFbx$_Scaling").c_str());
		if (findNode != nullptr)
		{
			auto trs = convert_assimp::Format(findNode->mTransformation);
			trs.Decompose(scale, rot, pos);
			part.second->SetOffsetScale(scale);
		}
		findNode = root->FindNode((part.first + "_$AssimpFbx$_PreRotation").c_str());
		if (findNode != nullptr)
		{
			auto trs = convert_assimp::Format(findNode->mTransformation);
			trs.Decompose(scale, rot, pos);
			part.second->SetOffsetPreRotation(rot);
		}
		findNode = root->FindNode((part.first + "_$AssimpFbx$_PostRotation").c_str());
		if (findNode != nullptr)
		{
			auto trs = convert_assimp::Format(findNode->mTransformation);
			trs.Decompose(scale, rot, pos);
			part.second->SetOffsetPostRotation(rot);
		}
	}
	
	for (auto& animNode : _nodeLists) {
		if (animNode->IsRoot()) {
			_rootBoneNode = animNode;
			break;
		}
	}
	if (_rootBoneNode == nullptr)
		if (model->_rootBoneNode != nullptr)
		{
			_rootBoneNode = _nodeTables[_model.lock()->_rootBoneNode->GetOriginalName()];

			_rootBoneNode->SetRoot(true);
		}
}


double Animation::CalculateTime(double time) const
{
	if(_totalTime == 0)
		return 0;

	if (_isLoop)
		return std::fmod(time,_totalTime);

	return std::clamp(time, 0.0, _totalTime);
}

bool Animation::IsEndTime(double time)
{
	return (!_isLoop) && _totalTime <= time;
}
