#pragma once
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
#define _HAS_STD_BYTE 0
#define NOMINMAX

#include <windows.h>
#include <tchar.h>
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <queue>
#include <iostream>
#include <fstream>
#include <typeinfo>
#include <filesystem>
#include <any>
#include <random>
#include <rpc.h>

#pragma comment(lib, "rpcrt4.lib")


#include "d3dx12.h"
#include <d3d12.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <dxgi1_4.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <d3d11on12.h>
#include <d2d1_3.h>
#include <dwrite_3.h>
#include <d2d1_1.h> 
#include <d2d1_1helper.h>

using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;

#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/DirectXTex.inl>

// 각종 lib
#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "d3d11.lib" )
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "Fmod/fmod_vc.lib")

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex\\DirectXTex_Debug.lib")
#else
#pragma comment(lib, "DirectXTex\\DirectXTex.lib")
#endif


#include <Assimp/Importer.hpp>
#include <Assimp/scene.h>
#include <Assimp/postprocess.h>

#ifdef _DEBUG
#pragma comment(lib, "Assimp/assimp-vc143-mtd.lib")
#pragma comment(lib, "Zlib/zlibstaticd.lib")
#else
#pragma comment(lib, "Assimp/assimp-vc143-mt.lib")
#pragma comment(lib, "Zlib/zlibstatic.lib")
#endif

#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <Assimp/postprocess.h>
#include <assimp/cimport.h>

#include <assimp/ai_assert.h>
#include <assimp/aabb.h>
#include <assimp/color4.h>
#include <assimp/quaternion.h>
#include <assimp/vector3.h>
#include <assimp/vector2.h>
#include <assimp/matrix4x4.h>
#include <assimp/matrix3x3.h>

#include "simple_mesh_LH.h"
#include "SimpleMath_LH.inl"

#include "nlohmann/json.hpp"

#include "NVApi/nvapi.h"
#pragma comment(lib, "NVApi/nvapi64.lib")



using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;
using vec2 = DirectX::SimpleMath::Vector2;
using vec3 = DirectX::SimpleMath::Vector3;
using vec4 = DirectX::SimpleMath::Vector4;
using Matrix = DirectX::SimpleMath::Matrix;
using Ray = DirectX::SimpleMath::Ray;
using Quaternion = DirectX::SimpleMath::Quaternion;

using namespace SimpleMath;
using namespace std;
namespace fs = std::filesystem;

#include "Global.h"

#include "strutil.h"
#include "wstrutil.h"
#include "guid_utility.h"
#include "convert_math.h"
#include "convert_string.h"
#include "convert_assimp.h"
#include "convert_directx.h"
#include "Input.h"
#include "Time.h"
#include "CbufferParam.h"
#include "ResourceManager.h"
#include "GeoMetryHelper.h"
#include "SceneManager.h"
#include "SpriteAction.h"
#include "Vertex.h"

#include "InGameGlobal.h"


#define IMGUI_DEFINE_MATH_OPERATORS
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_dx12.h"
#include "Imgui/imgui_impl_win32.h"

#include "ImguiManager.h"
#include "IComponentFactory.h"
#define IMGUI_ON

inline std::random_device _random;
inline std::uniform_int_distribution _random_dist(0, std::numeric_limits<int>::max());
inline std::uniform_real_distribution<float> _random_dist01(0, 1.0f);

inline void ThrowIfFailed(HRESULT hr) {
    if (FAILED(hr)) {
         throw std::exception();
    }
}
