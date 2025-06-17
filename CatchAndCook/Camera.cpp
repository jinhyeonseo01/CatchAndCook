#include "pch.h"
#include "Camera.h"
#include "Collider.h"
#include "simple_mesh_ext.h"
#include "Gizmo.h"
Camera::Camera(CameraType type)
{
	_type = type;
}

void Camera::Update()
{
    Calculate();
}

Vector3 Camera::GetScreenToWorldPosition(Vector2 mousePosition)
{
    auto cameraParam = _params;
    auto screenToWorld = CreateViewportMatrix(Viewport(
        cameraParam.cameraScreenData.z,cameraParam.cameraScreenData.w,
        cameraParam.cameraScreenData.x,cameraParam.cameraScreenData.y
    )).Invert() * cameraParam.InvertVPMatrix;
    return Vector3::Transform(Vector3(mousePosition.x,mousePosition.y,0),screenToWorld);
}

Vector2 Camera::GetWorldToScreenPosition(Vector3 worldPosition)
{
    auto cameraParam = _params;
    auto worldToScreen = cameraParam.VPMatrix * CreateViewportMatrix(Viewport(
        cameraParam.cameraScreenData.z,cameraParam.cameraScreenData.w,
        cameraParam.cameraScreenData.x,cameraParam.cameraScreenData.y
    ));
    return Vector2(Vector3::Transform(worldPosition, worldToScreen));
}


void Camera::SetCameraRotation(const Quaternion& quat)
{
    vec3 orginLook = vec3(0, 0, 1.0f);
    vec3 orginUp = vec3(0, 1.0f, 0);
    vec3 orginRight = vec3(1.0f, 0, 0);

    Vector3 eu = quat.ToEuler();

    _yaw += eu.y;
    _pitch += eu.x;
    _roll += eu.z;

    _cameraLook = vec3::Transform(orginLook, quat);
    _cameraRight = vec3::Transform(orginRight, quat);
    _cameraUp = vec3::Transform(orginUp, quat);

    _cameraLook.Normalize();
    _cameraRight.Normalize();
    _cameraUp.Normalize();
}

void Camera::SetCameraRotation(float yaw, float pitch, float roll)
{
    vec3 orginLook = vec3(0, 0, 1.0f);
    vec3 orginUp = vec3(0, 1.0f, 0);
    vec3 orginRight = vec3(1.0f, 0, 0);

    float radianYaw = XMConvertToRadians(yaw);
    float radianPitch = XMConvertToRadians(pitch);
    float radianRoll = XMConvertToRadians(roll);

    _yaw = radianYaw;
    _pitch = radianPitch;
    _roll= radianRoll;

    _cameraLook = vec3::TransformNormal(orginLook, Matrix::CreateFromYawPitchRoll(_yaw, _pitch, _roll));
    _cameraRight = vec3::TransformNormal(orginRight, Matrix::CreateFromYawPitchRoll(_yaw, _pitch, _roll));
    _cameraUp = vec3::TransformNormal(orginUp, Matrix::CreateFromYawPitchRoll(_yaw, _pitch, _roll));

    _cameraLook.Normalize();
    _cameraRight.Normalize();
    _cameraUp.Normalize();
}

void Camera::Calculate()
{
    _params.cameraScreenData = vec4(
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0, 0);

     _params.cameraPos = vec4(_cameraPos.x, _cameraPos.y, _cameraPos.z, 1);
     _params.cameraLook = vec4(_cameraLook.x, _cameraLook.y, _cameraLook.z, 0);
     _params.cameraUp = vec4(_cameraUp.x, _cameraUp.y, _cameraUp.z, 0);

    if (_projmode == CameraProjectionMode::Perspective)
    {
        _params.cameraFrustumData = Vector4(_fov , _params.cameraScreenData.x / _params.cameraScreenData.y, _near, _far);
        _params.ProjectionMatrix = Matrix::CreatePerspectiveFieldOfView(
            _params.cameraFrustumData.x,
            _params.cameraFrustumData.y,
            _params.cameraFrustumData.z,
            _params.cameraFrustumData.w);
    }

    if (_projmode == CameraProjectionMode::Orthographic)
    {
        _params.cameraFrustumData = vec4(_orthoSize.x, _orthoSize.y, _near, _far);
        _params.ProjectionMatrix = Matrix::CreateOrthographic(
            _params.cameraFrustumData.x,
            _params.cameraFrustumData.y,
            _params.cameraFrustumData.z,
            _params.cameraFrustumData.w);
    }

    _params.ViewMatrix = XMMatrixLookToLH(
        _params.cameraPos,
        _params.cameraLook,
        _params.cameraUp);

    _params.VPMatrix = _params.ViewMatrix * _params.ProjectionMatrix;
    _params.InvertViewMatrix = _params.ViewMatrix.Invert();
    _params.InvertProjectionMatrix = _params.ProjectionMatrix.Invert();
    _params.InvertVPMatrix = _params.VPMatrix.Invert();

    _boundingFrsutum.CreateFromMatrix(_boundingFrsutum,_params.ProjectionMatrix);
    _boundingFrsutum.Transform(_boundingFrsutum,_params.InvertViewMatrix);
}


void Camera::PushData()
{
	_cbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::CameraParam)->Alloc(1);
	memcpy(_cbufferContainer->ptr, (void*)&_params, sizeof(CameraParams));
}

void Camera::SetData()
{
	auto& cmdList =Core::main->GetCmdList();
	cmdList->SetGraphicsRootConstantBufferView(2, _cbufferContainer->GPUAdress);
    cmdList->SetComputeRootConstantBufferView(2, _cbufferContainer->GPUAdress);
}

void Camera::SetCameraLook(const vec3& look)
{
    _cameraLook = look;
    _cameraLook.Normalize();

    _cameraRight = _cameraUp.Cross(_cameraLook);
    _cameraRight.Normalize();

    _cameraUp = _cameraLook.Cross(_cameraRight);
    _cameraUp.Normalize();
}

void Camera::SetCameraUp(const vec3& up)
{
    _cameraUp = up;
    _cameraUp.Normalize();

    _cameraRight = _cameraUp.Cross(_cameraLook);
    _cameraRight.Normalize();

    _cameraLook = _cameraRight.Cross(_cameraUp);
    _cameraLook.Normalize();
}

void Camera::SetCameraRight(const vec3& right)
{
    _cameraRight = right;
    _cameraRight.Normalize();

    _cameraLook = _cameraRight.Cross(_cameraUp);
    _cameraLook.Normalize();

    _cameraUp = _cameraLook.Cross(_cameraRight);
    _cameraUp.Normalize();
}

bool Camera::IsInFrustum(BoundingBox& box)
{
    return _boundingFrsutum.Intersects(box);
}

/*************************
*                        *
*     DebugCamera        *
*                        *
**************************/

DebugCamera::DebugCamera()
{
    _type = CameraType::DebugCamera;

    _cameraPos = vec3(245.946 ,79.8085 ,225.333);
 /*   SetCameraLook()*/
}

DebugCamera::~DebugCamera()
{
}

void DebugCamera::Update()
{
	Camera::Update();
}

/*************************
*                        *
*     ThirdPersonCamera  *
*                        *
**************************/

SeaCamera::SeaCamera() : Camera(CameraType::SeaCamera)
{
    _type = CameraType::SeaCamera;
}

SeaCamera::~SeaCamera()
{
}

void SeaCamera::Update()
{
	Camera::Update();
}
/*************************
*                        *
*     UiCamera           *
*                        *
**************************/
UiCamera::UiCamera()
{
	_type = CameraType::UiCamera;
	_projmode = CameraProjectionMode::Orthographic;
    _orthoSize = vec2(WINDOW_WIDTH, WINDOW_HEIGHT);
	_cameraPos = vec3(0, 0, -1.0f);
}

UiCamera::~UiCamera()
{

}

void UiCamera::Update()
{
    Camera::Update();
}

BoatCamera::BoatCamera()
{
    _type = CameraType::BoatCamera;
}

BoatCamera::~BoatCamera()
{
}

void BoatCamera::Update()
{
    Camera::Update();
}
