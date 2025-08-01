#pragma once

#include <assimp/color4.h>
#include <assimp/matrix3x3.h>
#include <assimp/matrix4x4.h>
#include <assimp/quaternion.h>
#include <assimp/types.h>
#include <assimp/vector2.h>
#include <assimp/color4.h>
#include <assimp/types.h>

#include <assimp/scene.h>

#include "simple_mesh_LH.h"

class convert_assimp
{
public:
    static DirectX::SimpleMath::Matrix Format(const aiMatrix4x4& from);
    static DirectX::SimpleMath::Matrix Format(const aiMatrix3x3& from);
    static DirectX::SimpleMath::Vector3 Format(const aiVector3D& from);
    static DirectX::SimpleMath::Vector2 Format(const aiVector2D& from);
    static DirectX::SimpleMath::Quaternion Format(const aiQuaternion& pOrientation);
    static DirectX::SimpleMath::Color Format(const aiColor4D& color);
    static DirectX::SimpleMath::Ray Format(const aiRay& ray);
    static DirectX::SimpleMath::Plane Format(const aiPlane& plane);
    static std::string Format(const aiString& string);

};



class MakeLeftHandedProcess{
public:
    MakeLeftHandedProcess() = default;
    ~MakeLeftHandedProcess() = default;

    // -------------------------------------------------------------------

    bool IsActive(unsigned int pFlags) const;
    // -------------------------------------------------------------------
    void Execute(aiScene* pScene);

protected:
    // -------------------------------------------------------------------
    /** Recursively converts a node and all of its children
     */
    void ProcessNode(aiNode* pNode, const aiMatrix4x4& pParentGlobalRotation);

    // -------------------------------------------------------------------
    /** Converts a single mesh to left handed coordinates.
     * This means that positions, normals and tangents are mirrored at
     * the local Z axis and the order of all faces are inverted.
     * @param pMesh The mesh to convert.
     */
    void ProcessMesh(aiMesh* pMesh);

    // -------------------------------------------------------------------
    /** Converts a single material to left-handed coordinates
     * @param pMat Material to convert
     */
    void ProcessMaterial(aiMaterial* pMat);

    // -------------------------------------------------------------------
    /** Converts the given _animation to LH coordinates.
     * The rotation and translation keys are transformed, the scale keys
     * work in local space and can therefore be left untouched.
     * @param pAnim The bone animation to transform
     */
    void ProcessAnimation(aiNodeAnim* pAnim);

    // -------------------------------------------------------------------
    /** Converts a single camera to left handed coordinates.
     * The camera viewing direction is inverted by reflecting mLookAt
     * across mPosition.
     * @param pCam The camera to convert
     */
    void ProcessCamera(aiCamera* pCam);
};
