#include "pch.h"
#include "AnimationNode.h"


#include <algorithm>

#include "Animation.h"
#include "AnimationKeyFrame.h"
#include "Gizmo.h"
#include "ModelNode.h"
#include "simple_mesh_ext.h"


AnimationNode::AnimationNode()
{

}

AnimationNode::~AnimationNode()
{

}

void AnimationNode::Init(std::shared_ptr<Animation> animation)
{
    _animation = animation;
}

void AnimationNode::SetKeyFrames(aiAnimation* anim, aiNodeAnim* animNode)
{
	SetPosition(anim, animNode);
	SetRotation(anim, animNode);
	SetScale(anim, animNode);

    CheckMoveNode();
}

void AnimationNode::SetPosition(aiAnimation* anim, aiNodeAnim* animNode)
{
    AnimationKeyFrame keyFrame;

	if(animNode->mNumPositionKeys <= 1)
        return;

    for(int i=0;i<animNode->mNumPositionKeys;i++)
    {
        keyFrame._tick = animNode->mPositionKeys[i].mTime;
        keyFrame._time = keyFrame._tick / anim->mTicksPerSecond;
        keyFrame.position = vec3(animNode->mPositionKeys[i].mValue.x,animNode->mPositionKeys[i].mValue.y,animNode->mPositionKeys[i].mValue.z);
		_keyFrame_positions.push_back(keyFrame);
    }
}

void AnimationNode::SetRotation(aiAnimation* anim, aiNodeAnim* animNode)
{
    AnimationKeyFrame keyFrame;

    if(animNode->mNumRotationKeys <= 1)
        return;

    for(int i = 0; i < animNode->mNumRotationKeys; i++)
    {
        keyFrame._tick = animNode->mRotationKeys[i].mTime;
        keyFrame._time = keyFrame._tick / anim->mTicksPerSecond;
        
        keyFrame.rotation = Quaternion(animNode->mRotationKeys[i].mValue.x,
                                       animNode->mRotationKeys[i].mValue.y,
                                       animNode->mRotationKeys[i].mValue.z,
                                       animNode->mRotationKeys[i].mValue.w);
		_keyFrame_rotations.push_back(keyFrame);
    }
}

void AnimationNode::SetScale(aiAnimation* anim, aiNodeAnim* animNode)
{
    AnimationKeyFrame keyFrame;

    if(animNode->mNumScalingKeys <= 1)
        return;

    for(int i = 0; i < animNode->mNumScalingKeys; i++)
    {
        keyFrame._tick = animNode->mScalingKeys[i].mTime;
        keyFrame._time = keyFrame._tick / anim->mTicksPerSecond;
        keyFrame.scale = vec3(animNode->mScalingKeys[i].mValue.x,
                              animNode->mScalingKeys[i].mValue.y,
                              animNode->mScalingKeys[i].mValue.z);
		_keyFrame_scales.push_back(keyFrame);
    }
}

void AnimationNode::SetPose(Matrix& matrix)
{
    matrix.Decompose(offsetScale, offsetRotation, offsetPosition);
    hasPosition = true;
    hasScale = true;
}

void AnimationNode::SetOffsetPosition(const Vector3& position)
{
	offsetPosition = position;
    hasPosition = true;
}

void AnimationNode::SetOffsetScale(const Vector3& scale)
{
	offsetScale = scale;
    hasScale = true;
}

void AnimationNode::SetOffsetPreRotation(const Quaternion& quat)
{
	offsetPreRotation = quat;
    hasPreRotation = true;
}

void AnimationNode::SetOffsetPostRotation(const Quaternion& quat)
{
	offsetPostRotation = quat;
    hasPostRotation = true;
}

void AnimationNode::CheckMoveNode()
{
    const float SOME_THRESHOLD = 0.1f;

    if (_keyFrame_positions.size() != 0)
    {
        auto firstPos = _keyFrame_positions[0].position;
        auto lastPos = _keyFrame_positions[_keyFrame_positions.size() - 1].position;

        // 좌표 간의 차이가 일정 기준 이상이면 움직임이 있다고 판단
        if ((lastPos - firstPos).Length() > SOME_THRESHOLD) {
            _isRoot = true;
        }
    }
}

int AnimationNode::FindKeyFrameIndex(const vector<AnimationKeyFrame>& keyFrames, const double& time)
{
    if(keyFrames.empty())
        return -1;
    auto it = std::lower_bound(keyFrames.begin(),keyFrames.end(), time,
        [&](const AnimationKeyFrame& key, const double& t) {
        return key._time < t;
    });

    if(it == keyFrames.begin())
        return -1;

    return static_cast<int>(std::distance(keyFrames.begin(), it)) - 1;
}

Matrix AnimationNode::CalculateTransformMatrix(const double& time, bool xyLock) const
{
    Vector3 interpolatedPosition = offsetPosition;
    Quaternion interpolatedRotation = Quaternion::Identity;
    Vector3 interpolatedScale = offsetScale;

    {
        const size_t keyCount = _keyFrame_positions.size();
        if (keyCount != 0)
        {
            if (keyCount == 1)
                interpolatedPosition = _keyFrame_positions[0].position;
            int index = FindKeyFrameIndex(_keyFrame_positions, time);
            index = std::max(index, 0);
            int nextIndex = (index + 1 < keyCount) ? index + 1 : 0;
            const auto& key0 = _keyFrame_positions[index];
            const auto& key1 = _keyFrame_positions[nextIndex];
            const double dt = key1._time - key0._time;
            const double t = (dt != 0.f) ? (time - key0._time) / dt : 0.f;
            Vector3::Lerp(key0.position, key1.position, t, interpolatedPosition);
        }
    }
    {
        const size_t keyCount = _keyFrame_rotations.size();
        if (keyCount != 0)
        {
            if (keyCount == 1)
                interpolatedRotation = _keyFrame_rotations[0].rotation;
            int index = FindKeyFrameIndex(_keyFrame_rotations, time);
            index = std::max(index, 0);
            int nextIndex = (index + 1 < keyCount) ? index + 1 : 0;
            const auto& key0 = _keyFrame_rotations[index];
            const auto& key1 = _keyFrame_rotations[nextIndex];
            const double dt = key1._time - key0._time;
            const double t = (dt != 0.f) ? (time - key0._time) / dt : 0.f;
            Quaternion::Slerp(key0.rotation, key1.rotation, t, interpolatedRotation);
            interpolatedRotation.Normalize();
        }
    }

    {
        const size_t keyCount = _keyFrame_scales.size();
        if (keyCount != 0)
        {
            if (keyCount == 1)
                interpolatedScale = _keyFrame_scales[0].scale;
            int index = FindKeyFrameIndex(_keyFrame_scales, time);
            index = std::max(index, 0);
            int nextIndex = (index + 1 < keyCount) ? index + 1 : 0;
            const auto& key0 = _keyFrame_scales[index];
            const auto& key1 = _keyFrame_scales[nextIndex];
            const double dt = key1._time - key0._time;
            const double t = (dt != 0.f) ? (time - key0._time) / dt : 0.f;
            Vector3::Lerp(key0.scale, key1.scale, t, interpolatedScale);
        }
    }

    if (xyLock)
        interpolatedPosition = Vector3(0, interpolatedPosition.y, 0);

    return Matrix::CreateScale(interpolatedScale) *
        Matrix::CreateFromQuaternion(interpolatedRotation * offsetPreRotation) *
        Matrix::CreateTranslation(interpolatedPosition);
}




Matrix AnimationNode::CalculateTransformMatrixMapping(const std::shared_ptr<ModelNode>& _originModelNode, const std::shared_ptr<ModelNode>& _animModelNode, const double& time, bool xyLock) const
{
    Vector3 interpolatedPosition = offsetPosition;
    Quaternion interpolatedRotation = Quaternion::Identity;
    Vector3 interpolatedScale = offsetScale;

    {
        const size_t keyCount = _keyFrame_positions.size();
        if (keyCount != 0)
        {
            if (keyCount == 1)
                interpolatedPosition = _keyFrame_positions[0].position;
            int index = FindKeyFrameIndex(_keyFrame_positions, time);
            index = std::max(index, 0);
            int nextIndex = (index + 1 < keyCount) ? index + 1 : 0;
            const auto& key0 = _keyFrame_positions[index];
            const auto& key1 = _keyFrame_positions[nextIndex];
            const double dt = key1._time - key0._time;
            const double t = (dt != 0.f) ? (time - key0._time) / dt : 0.f;
            Vector3::Lerp(key0.position, key1.position, t, interpolatedPosition);
        }
    }
    {
        const size_t keyCount = _keyFrame_rotations.size();
        if (keyCount != 0)
        {
            if (keyCount == 1)
                interpolatedRotation = _keyFrame_rotations[0].rotation;
            int index = FindKeyFrameIndex(_keyFrame_rotations, time);
            index = std::max(index, 0);
            int nextIndex = (index + 1 < keyCount) ? index + 1 : 0;
            const auto& key0 = _keyFrame_rotations[index];
            const auto& key1 = _keyFrame_rotations[nextIndex];
            const double dt = key1._time - key0._time;
            const double t = (dt != 0.f) ? (time - key0._time) / dt : 0.f;
            Quaternion::Slerp(key0.rotation, key1.rotation, t, interpolatedRotation);
            interpolatedRotation.Normalize();
        }
    }

    {
        const size_t keyCount = _keyFrame_scales.size();
        if (keyCount != 0)
        {
            if (keyCount == 1)
                interpolatedScale = _keyFrame_scales[0].scale;
            int index = FindKeyFrameIndex(_keyFrame_scales, time);
            index = std::max(index, 0);
            int nextIndex = (index + 1 < keyCount) ? index + 1 : 0;
            const auto& key0 = _keyFrame_scales[index];
            const auto& key1 = _keyFrame_scales[nextIndex];
            const double dt = key1._time - key0._time;
            const double t = (dt != 0.f) ? (time - key0._time) / dt : 0.f;
            Vector3::Lerp(key0.scale, key1.scale, t, interpolatedScale);
        }
    }

    if (xyLock)
        interpolatedPosition = Vector3(0, interpolatedPosition.y, 0);

    return Matrix::CreateScale(_originModelNode->GetLocalScale()) *
        Matrix::CreateFromQuaternion(interpolatedRotation * _originModelNode->GetLocalPreRotation()) *
        Matrix::CreateTranslation(_originModelNode->GetLocalPosition());

    //hasScale
    Matrix matrix = Matrix::Identity;

    //if (hasPosition)
    //    matrix = Matrix::CreateTranslation(interpolatedPosition);
    //else
    matrix = Matrix::CreateTranslation(_originModelNode->GetLocalPosition());
    //prevTRSMatrix

    //if(hasPreRotation)
    //    interpolatedRotation = interpolatedRotation * _originalNodeData->GetLocalPreRotation();
    //if(hasPostRotation)
    //    interpolatedRotation = _originalNodeData->GetLocalPostRotation() * interpolatedRotation;

    //matrix = Matrix::CreateFromQuaternion(interpolatedRotation) * matrix;
    Quaternion test = _originModelNode->GetLocalRotation();
    //matrix = Matrix::CreateFromQuaternion(_originalNodeData->GetLocalRotation()) * matrix;
    //if (_originalNodeData->hasPreRotation)
    test = test * _originModelNode->GetLocalPreRotation();
    //if(hasPreRotation)
//    interpolatedRotation = interpolatedRotation * _originalNodeData->GetLocalPreRotation();
    //interpolatedRotation = test;
    //if (hasPreRotation)
        //interpolatedRotation = test;
    //if(hasPreRotation)

    //interpolatedRotation = test;
    //if(hasPreRotation)
    //test = interpolatedRotation * test;

    //interpolatedPosition

    matrix = Matrix::CreateFromQuaternion(test) * matrix;

    Matrix d = _originModelNode->_localTPose;
    Matrix e;
    _animModelNode->_localTPose.Invert(e);


    //if (_animModelNode != _originModelNode)
    {
        Matrix finalMatrix = _animModelNode->_globalTPose * _originModelNode->_globalInvertTPose;
        Matrix::CreateFromQuaternion(_originModelNode->_globalTPoseQuat);
        Matrix test;
        _originModelNode->_globalTPose.Invert(test);
        Vector3::Transform(Vector3::Forward, _originModelNode->_globalTPoseQuat);


        auto a = Vector3::Transform(Vector3::Zero, Matrix::CreateFromQuaternion(interpolatedRotation) * _originModelNode->_globalTPose);

        auto g = d * _animModelNode->_localInvertTPose;// * _animModelNode->_localTPose

        g =
            Matrix::CreateFromQuaternion(
                Quaternion::CreateFromAxisAngle(Vector3::Right, 30 * D2R) *
                Quaternion::CreateFromAxisAngle(Vector3::Up, 30 * D2R)
            );;

        g = Matrix::CreateFromQuaternion(_originModelNode->GetLocalPreRotation());


        Gizmo::Width(0.001);
        Gizmo::Ray(a + Vector3::Forward * ((_animModelNode != _originModelNode) ? 1 : 0),
            Vector3::TransformNormal(Vector3::Forward, g), 0.1, Vector4(0, 0, 1, 1));
        Gizmo::Ray(a + Vector3::Forward * ((_animModelNode != _originModelNode) ? 1 : 0),
            Vector3::TransformNormal(Vector3::Right, g), 0.1, Vector4(1, 0, 0, 1));
        Gizmo::Ray(a + Vector3::Forward * ((_animModelNode != _originModelNode) ? 1 : 0),
            Vector3::TransformNormal(Vector3::Up, g), 0.1, Vector4(0, 1, 0, 1));

        //Gizmo::WidthRollBack();

        //Gizmo::Ray(a + Vector3::Forward * ((_animModelNode != _originModelNode) ? 1 : 0),
        //    Vector3::Transform(Vector3::Forward, finalMatrix), 0.1);
        //return _originModelNode->GetLocalSRT();
        Quaternion corr;
        _originModelNode->GetLocalPreRotation().Inverse(corr);
        //interpolatedRotation = interpolatedRotation * _animModelNode->GetLocalPreRotation();

        auto d = LookToQuaternion(Vector3::Transform(Vector3::Forward, _originModelNode->GetLocalPreRotation()),
            Vector3::Transform(Vector3::Up, _originModelNode->GetLocalPreRotation())
            );
        d = _originModelNode->GetLocalPreRotation();
        Quaternion h;
        _animModelNode->GetLocalPreRotation().Inverse(h);
        auto worldInter = _animModelNode->_globalTPoseQuat * interpolatedRotation;
        auto localInter = _originModelNode->_globalInvertTPoseQuat * worldInter;

        //auto worldInter2 = _originModelNode->_globalTPoseQuat * interpolatedRotation;
        auto worldInter2 = interpolatedRotation;

        //interpolatedRotation = Quaternion::Slerp(d, worldInter2 * d, 0.2f);
        auto newQ = Quaternion::CreateFromYawPitchRoll(interpolatedRotation.ToEuler() + _originModelNode->GetLocalPreRotation().ToEuler());
        auto e = interpolatedRotation * _originModelNode->GetLocalPreRotation();
        if (_originModelNode->GetOriginalName() == "LeftLeg" && false)
        {
      /*      std::cout << to_string(_originModelNode->GetLocalPreRotation()) << "\n";
            std::cout << to_string(_animModelNode->GetLocalPreRotation()) << "\n";
            std::cout << to_string(interpolatedRotation * _originModelNode->GetLocalPreRotation()) << "\n";
            std::cout << to_string(interpolatedRotation * _animModelNode->GetLocalPreRotation()) << "\n\n";

            std::cout << to_string(interpolatedRotation.ToEuler() * R2D) << "\n";
            std::cout << to_string(_originModelNode->GetLocalPreRotation().ToEuler() * R2D) << "\n";
            std::cout << to_string(_animModelNode->GetLocalPreRotation().ToEuler() * R2D) << "\n";*/
        }

        a = Vector3::Transform(Vector3::Zero, Matrix::CreateFromQuaternion(worldInter2) * _originModelNode->_globalTPose);

        Gizmo::Width(0.001);
        Gizmo::Ray(a + Vector3::Forward * ((_animModelNode != _originModelNode) ? 1 : 0) + Vector3::Right * 3,
            Vector3::Transform(Vector3::Forward, worldInter2), 0.1, Vector4(0, 0, 1, 1));
        Gizmo::Ray(a + Vector3::Forward * ((_animModelNode != _originModelNode) ? 1 : 0) + Vector3::Right * 3,
            Vector3::Transform(Vector3::Right, worldInter2), 0.1, Vector4(1, 0, 0, 1));
        Gizmo::Ray(a + Vector3::Forward * ((_animModelNode != _originModelNode) ? 1 : 0) + Vector3::Right * 3,
            Vector3::Transform(Vector3::Up, worldInter2), 0.1, Vector4(0, 1, 0, 1));
        
        return Matrix::CreateScale(_originModelNode->GetLocalScale()) *
            Matrix::CreateFromQuaternion(interpolatedRotation * _originModelNode->GetLocalPreRotation()) * //
            Matrix::CreateTranslation(_originModelNode->GetLocalPosition());
        //interpolatedRotation
    }

    
    return Matrix::CreateScale(_originModelNode->GetLocalScale()) *
        Matrix::CreateFromQuaternion(Quaternion::CreateFromRotationMatrix(d)) *
        Matrix::CreateTranslation(_originModelNode->GetLocalPosition());


    //if (hasScale)
    //    matrix = Matrix::CreateScale(interpolatedScale) * matrix;
    //else
    //    matrix = Matrix::CreateScale(_originalNodeData->GetLocalScale()) * matrix;

    return matrix;
}

Vector3 AnimationNode::CalculateDeltaPosition(const double& prevTime, const double& time) const
{
    Vector3 interpolatedPosition = offsetPosition;
    Vector3 interpolatedPositionPrev = offsetPosition;

    const size_t keyCount = _keyFrame_positions.size();
    {
        if (keyCount != 0)
        {
            if (keyCount == 1)
                interpolatedPosition = _keyFrame_positions[0].position;
            int index = FindKeyFrameIndex(_keyFrame_positions, time);
            index = std::max(index, 0);
            int nextIndex = (index + 1 < keyCount) ? index + 1 : 0;
            const auto& key0 = _keyFrame_positions[index];
            const auto& key1 = _keyFrame_positions[nextIndex];
            const double dt = key1._time - key0._time;
            const double t = (dt != 0.f) ? (time - key0._time) / dt : 0.f;
            Vector3::Lerp(key0.position, key1.position, t, interpolatedPosition);
        }
    }

    {
        if (keyCount != 0)
        {
            if (keyCount == 1)
                interpolatedPositionPrev = _keyFrame_positions[0].position;
            int index = FindKeyFrameIndex(_keyFrame_positions, prevTime);
            index = std::max(index, 0);
            int nextIndex = (index + 1 < keyCount) ? index + 1 : 0;

            const auto& key0 = _keyFrame_positions[index];
            const auto& key1 = _keyFrame_positions[nextIndex];
            const double dt = key1._time - key0._time;
            const double t = (dt != 0.f) ? (prevTime - key0._time) / dt : 0.f;
            Vector3::Lerp(key0.position, key1.position, t, interpolatedPositionPrev);


            if ((time - prevTime) < 0) {
                interpolatedPositionPrev = _keyFrame_positions[0].position - (_keyFrame_positions[keyCount - 1].position - interpolatedPositionPrev);
            }
        }
    }

    return interpolatedPosition - interpolatedPositionPrev;
}

Quaternion AnimationNode::CalculateDeltaRotation(const double& prevTime, const double& time) const
{
    Quaternion interpolatedRotation = offsetRotation;
    Quaternion interpolatedPrevRotation = offsetRotation;

    const size_t keyCount = _keyFrame_rotations.size();
    {
        if (keyCount != 0)
        {
            if (keyCount == 1)
                interpolatedRotation = _keyFrame_rotations[0].rotation;
            int index = FindKeyFrameIndex(_keyFrame_rotations, time);
            index = std::max(index, 0);
            int nextIndex = (index + 1 < keyCount) ? index + 1 : 0;
            const auto& key0 = _keyFrame_rotations[index];
            const auto& key1 = _keyFrame_rotations[nextIndex];
            const double dt = key1._time - key0._time;
            const double t = (dt != 0.f) ? (time - key0._time) / dt : 0.f;
            Quaternion::Slerp(key0.rotation, key1.rotation, t, interpolatedRotation);
        }
    }

    {
        if (keyCount != 0)
        {
            if (keyCount == 1)
                interpolatedPrevRotation = _keyFrame_rotations[0].rotation;
            int index = FindKeyFrameIndex(_keyFrame_rotations, prevTime);
            index = std::max(index, 0);
            int nextIndex = (index + 1 < keyCount) ? index + 1 : 0;
            const auto& key0 = _keyFrame_rotations[index];
            const auto& key1 = _keyFrame_rotations[nextIndex];
            const double dt = key1._time - key0._time;
            const double t = (dt != 0.f) ? (prevTime - key0._time) / dt : 0.f;
            Quaternion::Slerp(key0.rotation, key1.rotation, t, interpolatedPrevRotation);

            if ((time - prevTime) < 0) {
                Quaternion tempR;
                _keyFrame_rotations[keyCount - 1].rotation.Inverse(tempR);
                Quaternion rotationDiff = tempR * interpolatedPrevRotation;
                interpolatedPrevRotation = _keyFrame_rotations[0].rotation * rotationDiff;
            }
        }
    }

    Quaternion tempR;
    interpolatedPrevRotation.Inverse(tempR);
    return tempR * interpolatedRotation;
}

Vector3 AnimationNode::CalculateDeltaScale(const double& prevTime, const double& time) const
{
    Vector3 interpolatedScale = offsetScale;
    Vector3 interpolatedPrevScale = offsetScale;

    const size_t keyCount = _keyFrame_scales.size();
    {
        if (keyCount != 0)
        {
            if (keyCount == 1)
                interpolatedScale = _keyFrame_scales[0].scale;
            int index = FindKeyFrameIndex(_keyFrame_scales, time);
            index = std::max(index, 0);
            int nextIndex = (index + 1 < keyCount) ? index + 1 : 0;
            const auto& key0 = _keyFrame_scales[index];
            const auto& key1 = _keyFrame_scales[nextIndex];
            const double dt = key1._time - key0._time;
            const double t = (dt != 0.f) ? (time - key0._time) / dt : 0.f;
            Vector3::Lerp(key0.scale, key1.scale, t, interpolatedScale);
        }
    }

    {
        if (keyCount != 0)
        {
            if (keyCount == 1)
                interpolatedPrevScale = _keyFrame_scales[0].scale;
            int index = FindKeyFrameIndex(_keyFrame_scales, prevTime);
            index = std::max(index, 0);
            int nextIndex = (index + 1 < keyCount) ? index + 1 : 0;
            const auto& key0 = _keyFrame_scales[index];
            const auto& key1 = _keyFrame_scales[nextIndex];
            const double dt = key1._time - key0._time;
            const double t = (dt != 0.f) ? (prevTime - key0._time) / dt : 0.f;
            Vector3::Lerp(key0.scale, key1.scale, t, interpolatedPrevScale);

            if ((time - prevTime) < 0) {
                interpolatedPrevScale = _keyFrame_scales[0].scale - (_keyFrame_scales[keyCount - 1].scale - interpolatedPrevScale);
            }
        }
    }

    return interpolatedScale - interpolatedPrevScale;
}


Vector3 AnimationNode::CalculatePosition(const double& time) const
{
    Vector3 interpolatedPosition = offsetPosition;

    {
        const size_t keyCount = _keyFrame_positions.size();
        if (keyCount != 0)
        {
            if (keyCount == 1)
                interpolatedPosition = _keyFrame_positions[0].position;
            int index = FindKeyFrameIndex(_keyFrame_positions, time);
            index = std::max(index, 0);
            int nextIndex = (index + 1 < keyCount) ? index + 1 : 0;
            const auto& key0 = _keyFrame_positions[index];
            const auto& key1 = _keyFrame_positions[nextIndex];
            const double dt = key1._time - key0._time;
            const double t = (dt != 0.f) ? (time - key0._time) / dt : 0.f;
            Vector3::Lerp(key0.position, key1.position, t, interpolatedPosition);
        }
    }
    return interpolatedPosition;
}

Quaternion AnimationNode::CalculateRotation(const double& time) const
{
    Quaternion interpolatedRotation = offsetRotation;
    {
        const size_t keyCount = _keyFrame_rotations.size();
        if (keyCount != 0)
        {
            if (keyCount == 1)
                interpolatedRotation = _keyFrame_rotations[0].rotation;
            int index = FindKeyFrameIndex(_keyFrame_rotations, time);
            index = std::max(index, 0);
            int nextIndex = (index + 1 < keyCount) ? index + 1 : 0;
            const auto& key0 = _keyFrame_rotations[index];
            const auto& key1 = _keyFrame_rotations[nextIndex];
            const double dt = key1._time - key0._time;
            const double t = (dt != 0.f) ? (time - key0._time) / dt : 0.f;
            Quaternion::Slerp(key0.rotation, key1.rotation, t, interpolatedRotation);
        }
    }

    //if (hasPreRotation)
    //    interpolatedRotation = interpolatedRotation * offsetPreRotation;
    //if (hasPostRotation)
    //    interpolatedRotation = offsetPostRotation * interpolatedRotation;
    return interpolatedRotation;
}

Vector3 AnimationNode::CalculateScale(const double& time) const
{
    Vector3 interpolatedScale = offsetScale;
    {
        const size_t keyCount = _keyFrame_scales.size();
        if (keyCount != 0)
        {
            if (keyCount == 1)
                interpolatedScale = _keyFrame_scales[0].scale;
            int index = FindKeyFrameIndex(_keyFrame_scales, time);
            index = std::max(index, 0);
            int nextIndex = (index + 1 < keyCount) ? index + 1 : 0;
            const auto& key0 = _keyFrame_scales[index];
            const auto& key1 = _keyFrame_scales[nextIndex];
            const double dt = key1._time - key0._time;
            const double t = (dt != 0.f) ? (time - key0._time) / dt : 0.f;
            Vector3::Lerp(key0.scale, key1.scale, t, interpolatedScale);
        }
    }

    return interpolatedScale;
}
