
#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <DirectXMath.h>
using namespace DirectX;

using vec2 = DirectX::SimpleMath::Vector2;
using vec3 = DirectX::SimpleMath::Vector3;
using vec4 = DirectX::SimpleMath::Vector4;
using Matrix = DirectX::SimpleMath::Matrix;
using Ray = DirectX::SimpleMath::Ray;
using Quaternion = DirectX::SimpleMath::Quaternion;


void Print(const Matrix& mat)
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			cout << mat.m[i][j] << " ";
		}
		cout << endl;
	}

	cout << endl;
}

int main()
{
	vec4 worldPos = vec4(-5.0f, 0, 30.0f, 1); 

    // 1) 카메라 설정
    Vector3 cameraPos(0, 0, 0);
    Vector3 cameraLook(0, 0, -1);
    Vector3 cameraUp(0, 1, 0);

    Matrix viewMat = Matrix::CreateLookAt(
        cameraPos,
        cameraLook,
        cameraUp
    );

    Matrix projMat = Matrix::CreatePerspectiveFieldOfView(
        XM_PI / 4.0f,      // FOV 45°
        1920.0f / 1080.0f, // Aspect
        0.1f,              // Near
        3000.0f            // Far
    );


	vec4 viewPos = vec4::Transform(worldPos, viewMat);
    vec4 clipPos = vec4::Transform(viewPos, projMat);

	cout << "World Space= (" << worldPos.x << " " << worldPos.y << " " << worldPos.z <<")" << endl;
	cout << "View Space = (" << viewPos.x << ", " << viewPos.y << ", " << viewPos.z << ")" << endl;
	cout << "Clip Space = (" << clipPos.x << ", " << clipPos.y << ", " << clipPos.z << ", " << clipPos.w << ")" << endl;


	vec3 Originndc;
    Originndc.x = clipPos.x / clipPos.w;
    Originndc.y = clipPos.y / clipPos.w;
    Originndc.z = clipPos.z / clipPos.w;
	cout << "NDC = (" << Originndc.x << ", " << Originndc.y << ", " << Originndc.z << ")" << endl;

    cout << "===========================================" << endl;
    //////////////////////////////////////////////////////////////////////////////////////////

    // 2) 화면상의 픽셀 좌표 + 깊이값 (예시)

    Vector2 screenSize(1920.0f, 1080.0f);
    float pixelX = (Originndc.x + 1.0f) * 0.5f * screenSize.x;
    float pixelY = (1.0f - Originndc.y) * 0.5f * screenSize.y;

	cout << "Pixel = (" << pixelX << ", " << pixelY << ")" << endl;
  
    Vector3 ndc;
    //ndc.x = (pixelX / screenSize.x) * 2.0f - 1.0f;
    //ndc.y = 1.0f - (pixelY / screenSize.y) * 2.0f;
    ndc.x = Originndc.x;
    ndc.y = Originndc.y;
    ndc.z = Originndc.z;

    // 4) NDC → Clip Space
    Vector4 clipPos2(ndc.x, ndc.y, ndc.z, 1.0f);

    // 5) Clip → View Space (Inverse Projection + Perspective Divide)
    Matrix invProj = projMat.Invert();
    Vector4 viewH = Vector4::Transform(clipPos2, invProj);
    viewH = viewH / viewH.w;   // perspective divide

	cout << "Unprojected View Space = (" << viewH.x << ", " << viewH.y << ", " << viewH.z << ")" << endl;

    // 6) View → World Space (Inverse View + Divide)
    Matrix invView = viewMat.Invert();
    Vector4 worldH = Vector4::Transform(viewH, invView);
    cout << worldH.w << endl;

 /*   worldH = worldH / worldH.w;*/

    // 7) 결과 출력
    std::cout
        << "Unprojected World Pos = ("
        << worldH.x << ", "
        << worldH.y << ", "
        << worldH.z << ")\n";

    return 0;
}