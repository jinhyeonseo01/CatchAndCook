#include "pch.h"

DirectX::SimpleMath::Matrix convert_assimp::Format(const aiMatrix4x4& mat)
{
    DirectX::SimpleMath::Matrix result(mat[0]);
    result._11 = mat.a1;
    result._21 = mat.a2;
    result._31 = mat.a3;
    result._41 = mat.a4;
    result._12 = mat.b1;
    result._22 = mat.b2;
    result._32 = mat.b3;
    result._42 = mat.b4;
    result._13 = mat.c1;
    result._23 = mat.c2;
    result._33 = mat.c3;
    result._43 = mat.c4;
    result._14 = mat.d1;
    result._24 = mat.d2;
    result._34 = mat.d3;
    result._44 = mat.d4;
    //Matrix result2(mat[0]);
    //result(mat[0]);
    //result = result.Transpose();
    return result;
}

DirectX::SimpleMath::Matrix convert_assimp::Format(const aiMatrix3x3& mat)
{
    DirectX::SimpleMath::Matrix result = DirectX::SimpleMath::Matrix::Identity;
    result._11 = mat.a1;
    result._21 = mat.a2;
    result._31 = mat.a3;
    result._12 = mat.b1;
    result._22 = mat.b2;
    result._32 = mat.b3;
    result._13 = mat.c1;
    result._23 = mat.c2;
    result._33 = mat.c3;
    return result;
}

DirectX::SimpleMath::Vector3 convert_assimp::Format(const aiVector3D& from)
{
    return DirectX::SimpleMath::Vector3(from.x, from.y, from.z);
}

DirectX::SimpleMath::Vector2 convert_assimp::Format(const aiVector2D& from)
{
    return DirectX::SimpleMath::Vector2(from.x, from.y);
}

DirectX::SimpleMath::Quaternion convert_assimp::Format(const aiQuaternion& pOrientation)
{
    Quaternion q = DirectX::SimpleMath::Quaternion(pOrientation.x, pOrientation.y, pOrientation.z, pOrientation.w);
    return q;
}

DirectX::SimpleMath::Color convert_assimp::Format(const aiColor4D& color)
{
    return DirectX::SimpleMath::Color(color.r, color.g, color.b, color.a);
}

DirectX::SimpleMath::Ray convert_assimp::Format(const aiRay& ray)
{
    return DirectX::SimpleMath::Ray(Format(ray.pos), Format(ray.dir));
}

DirectX::SimpleMath::Plane convert_assimp::Format(const aiPlane& plane)
{
    return DirectX::SimpleMath::Plane(plane.a, plane.b, plane.c, plane.d);
}

std::string convert_assimp::Format(const aiString& str1)
{
    auto str2 = std::string(str1.C_Str(), str1.length);
    str::trim(str2);
    return str2;
}


using namespace Assimp;

namespace {

    template <typename aiMeshType>
    void flipUVs(aiMeshType* pMesh) {
        if (pMesh == nullptr) {
            return;
        }
        // mirror texture y coordinate
        for (unsigned int tcIdx = 0; tcIdx < AI_MAX_NUMBER_OF_TEXTURECOORDS; tcIdx++) {
            if (!pMesh->HasTextureCoords(tcIdx)) {
                break;
            }

            for (unsigned int vIdx = 0; vIdx < pMesh->mNumVertices; vIdx++) {
                pMesh->mTextureCoords[tcIdx][vIdx].y = 1.0f - pMesh->mTextureCoords[tcIdx][vIdx].y;
            }
        }
    }

} // namespace

// ------------------------------------------------------------------------------------------------
// Returns whether the processing step is present in the given flag field.
bool MakeLeftHandedProcess::IsActive(unsigned int pFlags) const {
    return 0 != (pFlags & aiProcess_MakeLeftHanded);
}

// ------------------------------------------------------------------------------------------------
// Executes the post processing step on the given imported data.
void MakeLeftHandedProcess::Execute(aiScene* pScene) {
    // Check for an existent root node to proceed
    ai_assert(pScene->mRootNode != nullptr);

    // recursively convert all the nodes
    ProcessNode(pScene->mRootNode, aiMatrix4x4());

    // process the meshes accordingly
    for (unsigned int a = 0; a < pScene->mNumMeshes; ++a) {
        ProcessMesh(pScene->mMeshes[a]);
    }

    // process the materials accordingly
    for (unsigned int a = 0; a < pScene->mNumMaterials; ++a) {
        ProcessMaterial(pScene->mMaterials[a]);
    }

    // transform all animation channels as well
    for (unsigned int a = 0; a < pScene->mNumAnimations; a++) {
        aiAnimation* anim = pScene->mAnimations[a];
        for (unsigned int b = 0; b < anim->mNumChannels; b++) {
            aiNodeAnim* nodeAnim = anim->mChannels[b];
            ProcessAnimation(nodeAnim);
        }
    }

    // process the cameras accordingly
    for (unsigned int a = 0; a < pScene->mNumCameras; ++a)
    {
        ProcessCamera(pScene->mCameras[a]);
    }
}

// ------------------------------------------------------------------------------------------------
// Recursively converts a node, all of its children and all of its meshes
void MakeLeftHandedProcess::ProcessNode(aiNode* pNode, const aiMatrix4x4& pParentGlobalRotation) {
    // mirror all base vectors at the local Z axis
    pNode->mTransformation.a1 = -pNode->mTransformation.a1;
    pNode->mTransformation.a2 = -pNode->mTransformation.a2;
    pNode->mTransformation.a3 = -pNode->mTransformation.a3;
    pNode->mTransformation.a4 = -pNode->mTransformation.a4;

    // now invert the Z axis again to keep the matrix determinant positive.
    // The local meshes will be inverted accordingly so that the result should look just fine again.
    pNode->mTransformation.a1 = -pNode->mTransformation.a1;
    pNode->mTransformation.b1 = -pNode->mTransformation.b1;
    pNode->mTransformation.c1 = -pNode->mTransformation.c1;
    pNode->mTransformation.d1 = -pNode->mTransformation.d1; // useless, but anyways...

    // continue for all children
    for (size_t a = 0; a < pNode->mNumChildren; ++a) {
        ProcessNode(pNode->mChildren[a], pParentGlobalRotation * pNode->mTransformation);
    }
}

// ------------------------------------------------------------------------------------------------
// Converts a single mesh to left handed coordinates.
void MakeLeftHandedProcess::ProcessMesh(aiMesh* pMesh) {
    if (nullptr == pMesh) {
        return;
    }
    // mirror positions, normals and stuff along the Z axis
    for (size_t a = 0; a < pMesh->mNumVertices; ++a) {
        pMesh->mVertices[a].x *= -1.0f;
        if (pMesh->HasNormals()) {
            pMesh->mNormals[a].x *= -1.0f;
        }
        if (pMesh->HasTangentsAndBitangents()) {
            pMesh->mTangents[a].x *= -1.0f;
            pMesh->mBitangents[a].x *= -1.0f;
        }
    }

    // mirror anim meshes positions, normals and stuff along the Z axis
    for (size_t m = 0; m < pMesh->mNumAnimMeshes; ++m) {
        for (size_t a = 0; a < pMesh->mAnimMeshes[m]->mNumVertices; ++a) {
            pMesh->mAnimMeshes[m]->mVertices[a].x *= -1.0f;
            if (pMesh->mAnimMeshes[m]->HasNormals()) {
                pMesh->mAnimMeshes[m]->mNormals[a].x *= -1.0f;
            }
            if (pMesh->mAnimMeshes[m]->HasTangentsAndBitangents()) {
                pMesh->mAnimMeshes[m]->mTangents[a].x *= -1.0f;
                pMesh->mAnimMeshes[m]->mBitangents[a].x *= -1.0f;
            }
        }
    }

    // mirror offset matrices of all bones
    for (size_t a = 0; a < pMesh->mNumBones; ++a) {
        aiBone* bone = pMesh->mBones[a];
        bone->mOffsetMatrix.b1 = -bone->mOffsetMatrix.b1;
        bone->mOffsetMatrix.c1 = -bone->mOffsetMatrix.c1;
        bone->mOffsetMatrix.d1 = -bone->mOffsetMatrix.d1;
        bone->mOffsetMatrix.a2 = -bone->mOffsetMatrix.a2;
        bone->mOffsetMatrix.a3 = -bone->mOffsetMatrix.a3;
        bone->mOffsetMatrix.a4 = -bone->mOffsetMatrix.a4;
    }

    // mirror bitangents as well as they're derived from the texture coords
    if (pMesh->HasTangentsAndBitangents()) {
        for (unsigned int a = 0; a < pMesh->mNumVertices; a++)
            pMesh->mBitangents[a] *= -1.0f;
    }
}

// ------------------------------------------------------------------------------------------------
// Converts a single material to left handed coordinates.
void MakeLeftHandedProcess::ProcessMaterial(aiMaterial* _mat) {
    if (nullptr == _mat) {
        return;
    }

    aiMaterial* mat = (aiMaterial*)_mat;
    for (unsigned int a = 0; a < mat->mNumProperties; ++a) {
        aiMaterialProperty* prop = mat->mProperties[a];

        // Mapping axis for UV mappings?
        if (!::strcmp(prop->mKey.data, "$tex.mapaxis")) {
            ai_assert(prop->mDataLength >= sizeof(aiVector3D)); // something is wrong with the validation if we end up here
            aiVector3D* pff = (aiVector3D*)prop->mData;
            pff->x *= -1.f;
        }
    }
}

// ------------------------------------------------------------------------------------------------
// Converts the given animation to LH coordinates.
void MakeLeftHandedProcess::ProcessAnimation(aiNodeAnim* pAnim) {
    // position keys
    for (unsigned int a = 0; a < pAnim->mNumPositionKeys; a++)
        pAnim->mPositionKeys[a].mValue.x *= -1.0f;

    // rotation keys
    for (unsigned int a = 0; a < pAnim->mNumRotationKeys; a++) {
        pAnim->mRotationKeys[a].mValue.y *= -1.0f;
        pAnim->mRotationKeys[a].mValue.z *= -1.0f;
    }
}

// ------------------------------------------------------------------------------------------------
// Converts a single camera to left handed coordinates.
void MakeLeftHandedProcess::ProcessCamera(aiCamera* pCam)
{
    pCam->mLookAt = ai_real(2.0f) * pCam->mPosition - pCam->mLookAt;
}
