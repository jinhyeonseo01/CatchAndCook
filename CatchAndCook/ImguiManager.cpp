#include "pch.h"
#include "ImguiManager.h"
#include "GameObject.h"
#include "Transform.h"
#include "LightManager.h"
#include "WaterController.h"
#include <commdlg.h>
#include "ComputeManager.h"
#include "Gizmo.h"
#include "CameraManager.h"
#include "Camera.h"
#include "InGameGlobal.h"
#include "LightComponent.h"
#include "PathFinder.h"
#include "Volumetric.h"
unique_ptr<ImguiManager> ImguiManager::main;

ImguiManager::~ImguiManager()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ImguiManager::Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;    
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;     

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(Core::main->GetHandle());
	ImGui_ImplDX12_Init(Core::main->GetDevice().Get(), 2,
		DXGI_FORMAT_R8G8B8A8_UNORM, Core::main->GetImguiHeap().Get(),
		Core::main->GetImguiHeap()->GetCPUDescriptorHandleForHeapStart(),
		Core::main->GetImguiHeap()->GetGPUDescriptorHandleForHeapStart());

    ImFont* font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\malgun.ttf", 16.0f, nullptr, io.Fonts->GetGlyphRangesKorean());
    if (!font) {
        std::cout << "Imgui Font Load 실패\n";
    }
}

void ImguiManager::Render()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	Debug();
	DebugJin();

    if (playerHeightOffset)
    {
        ImGui::SliderFloat("playerHeightOffset", playerHeightOffset, 0.0f, 6.0f);
        ImGui::SliderFloat("playerForwardOffset", playerForwardOffset,-6.0f, 6.0f);
		ImGui::SliderFloat("cameraPitchOffset", cameraPitchOffset, -90.0f, 90.0f);
        ImGui::SliderFloat("cameraYawOffset", cameraYawOffset, -10.0f, 10.0f);
    }

	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), Core::main->GetCmdList().Get());
}

void ImguiManager::Debug()
{
	ImGui::Begin("GUI");
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);

    if (ImGui::CollapsingHeader("Debug"))
    {
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
		ImGui::Text("ForwardCount: %d", g_debug_forward_count);
        ImGui::Text("DeferredCount: %d", g_debug_deferred_count);
		ImGui::Text("ForwardCullingCount: %d", g_debug_forward_culling_count);
		ImGui::Text("DeferredCullingCount: %d", g_debug_deferred_culling_count);
        ImGui::Text("shadowDrawCall: %d", g_debug_shadow_draw_call);
        ImGui::Text("DrawCall: %d", g_debug_draw_call);
    }

	if (ImGui::CollapsingHeader("Gizmo Controller"))
	{
        GizmoController();
	}

    if (ImGui::CollapsingHeader("Object Controller"))
    {
        SeaController();
        LightController();
        BoidMove();
        Sky();
        VolumetricTest();
      
    };

	if (ImGui::CollapsingHeader("Compute Controller"))
	{
		ComputeController();
	}


	ImGui::End();

}


void ImguiManager::BoidMove()
{

    if (separationWeight)
    {
        if(ImGui::TreeNode("BoidMove"))
        {
            ImGui::SliderFloat("separationWeight", separationWeight, 0, 100.0f);
            ImGui::SliderFloat("alignmentWeight", alignmentWeight, 0, 100.0f);
            ImGui::SliderFloat("cohesionWeight", cohesionWeight, 0, 100.0f);
            ImGui::TreePop();
        }
    }

}

void ImguiManager::Sky()
{
    if (ImGui::TreeNode("Sky"))
    {
        float b = InGameGlobal::main->skyTime;
        if (ImGui::SliderFloat("time", &b, 0.0f, 8.0f))
        {
            InGameGlobal::main->skyTime = b;
        }
        int a = (int)InGameGlobal::main->skyTime;
        if (ImGui::SliderInt("Render Type", &a, 0, 3))
        {
            if (auto light = LightComponent::GetMainLight())
                InGameGlobal::main->skyTime = a;
        }
        ImGui::TreePop();
    }
}

void ImguiManager::GizmoController()
{
    if (ImGui::Button("None"))
    {
        Gizmo::main->_flags = GizmoFlags::None;
    }

    if (ImGui::Button("Collision"))
    {
        Gizmo::main->_flags = Gizmo::main->_flags | GizmoFlags::Collision;
    }

    if (ImGui::Button("DivideSpace"))
    {
        Gizmo::main->_flags = Gizmo::main->_flags | GizmoFlags::DivideSpace;
    }


    if (ImGui::Button("Culling"))
    {
        Gizmo::main->_flags = Gizmo::main->_flags | GizmoFlags::Culling;
    }

	if (ImGui::Button("WorldPivot"))
	{
		Gizmo::main->_flags = Gizmo::main->_flags | GizmoFlags::WorldPivot;
	}
    if (ImGui::Button("Custom"))
    {
        Gizmo::main->_flags = Gizmo::main->_flags | GizmoFlags::Custom;
    }
    if (ImGui::Button("RenderPreview"))
    {
        Gizmo::main->_flags = Gizmo::main->_flags | GizmoFlags::RenderPreview;
    }
    if (ImGui::Button("NavMesh"))
    {
        Gizmo::main->_flags = Gizmo::main->_flags | GizmoFlags::NavMesh;
    }

    if (ImGui::Button("DrawPath"))
    {
        Gizmo::main->_flags = Gizmo::main->_flags | GizmoFlags::DrawPath;
    }
}

void ImguiManager::ComputeController()
{

    if (ImGui::Button("Blur ON/OFF"))
    {
        *_blurPtr = !(*_blurPtr);
    }

    if (ImGui::Button("Bloom ON/OFF"))
    {
        *_bloomPtr = !(*_bloomPtr);
    }

    if (ImGui::Button("Shadow ON/OFF"))
    {
        *_shadowOnOff = !(*_shadowOnOff);
    }

    if (ImGui::Button("SSAO ON/OFF"))
    {
        *_ssaoOnOff = !(*_ssaoOnOff);
    }
    if (ImGui::Button("ColorGrading ON/OFF"))
    {
        *_colorGradingOnOff = !(*_colorGradingOnOff);
    }

    if (ImGui::Button("MainField Fog ON/OFF"))
    {
        *mainField_fog = !(*mainField_fog);
    }
    if (ImGui::Button("MainField GodRay ON/OFF"))
    {
        *_godRayPtr = !(*_godRayPtr);
    }
    if (ImGui::Button("Vignette ON/OFF"))
    {
        *mainField_vignette = !(*mainField_vignette);
    }
    if (ImGui::Button("MainField BakedGI ON/OFF"))
    {
        *_bakedGIOnOff = !(*_bakedGIOnOff);
    }
    if (ImGui::Button("MainField FXAA ON/OFF"))
    {
    	*_fxaa = !(*_fxaa);
    }
    if (ImGui::Button("MainField DOF ON/OFF"))
    {
        *_dofPtr = !(*_dofPtr);
    }

 
    if (ImGui::Button("MainField Total ON/OFF"))
    {
        *mainField_total = !(*mainField_total);
        *_shadowOnOff = *mainField_total;
        *_bakedGIOnOff = *mainField_total;
    }


    static bool showDepthRender = false;
    if (ImGui::Button("Depth Render"))
    {
        showDepthRender = !showDepthRender;
    }
    if (showDepthRender)
    {
        ImGui::SliderInt("Render Type", &_fogParam->depthRendering, 0, 2);
        ImGui::SliderFloat("Fog Max", &_fogParam->g_fogMax, 0.0f, 5000.0f);
        ImGui::SliderFloat("Fog Min", &_fogParam->g_fogMin, 0.0f, 5000.0f);
        ImGui::SliderFloat3("Fog Color", &_fogParam->g_fogColor.x, 0.0f, 1.0f);
        ImGui::SliderFloat("Fog Power", &_fogParam->power, 0.0f, 30.0f);
    }

    if (ImGui::Button("Underwater Effect"))
    {
        _underWaterParam->g_on *= -1;
    }
    if (_underWaterParam->g_on == 1)
    {
        ImGui::SliderFloat3("Underwater Fog Color", &_underWaterParam->g_fogColor.x, 0.0f, 1.0f);
        ImGui::SliderFloat("Underwater Fog Power", &_underWaterParam->g_fog_power, 0.0f, 30.0f);
        ImGui::SliderFloat("Underwater Fog Max", &_underWaterParam->g_fogMax, 0.0f, 5000.0f);
        ImGui::SliderFloat("Underwater Fog Min", &_underWaterParam->g_fogMin, 0.0f, 5000.0f);
        ImGui::SliderFloat3("Underwater Color", &_underWaterParam->g_underWaterColor.x, 0.0f, 1.0f);
    }

	if (ImGui::Button("Scattering ON/OFF"))
	{
		*_scattering = !(*_scattering);
	}

	if (*_scattering)
	{
  
		ImGui::SliderFloat("phaseG", &_scatteringData->phaseG, 0.0f, 1.0f);
		ImGui::SliderFloat("absorption", &_scatteringData->absorption, 0.0f, 1000.0f);
		ImGui::SliderFloat("DENSITY", &_scatteringData->density, 0.0f, 15.0f);
		ImGui::SliderFloat3("scatterColor", &_scatteringData->scatterColor.x, 0.0f, 1.0f);
	}


}

void ImguiManager::LightController()
{
    if (ImGui::TreeNode("Lighting"))
    {
        size_t size = LightManager::main->_lights.size();
        for (size_t i = 0; i < size; i++)
        {
            auto light = LightManager::main->_lights[i];

            if (ImGui::TreeNode((std::string("Light ") + std::to_string(i)).c_str()))
            {
                std::string prefix = "Light " + std::to_string(i) + " ";
				ImGui::Text("Light Type : %s", light->material.lightType == 0 ? "Directional" : light->material.lightType == 1 ? "Point" : "Spot");
                ImGui::SliderFloat3((prefix + "strength").c_str(), &light->strength.x, 0, 1.0f);
                ImGui::SliderFloat3((prefix + "Diffuse").c_str(), &light->material.diffuse.x, 0, 1.0f);
                ImGui::SliderFloat3((prefix + "ambient").c_str(), &light->material.ambient.x, 0, 1.0f);
                ImGui::SliderFloat3((prefix + "Direction").c_str(), &light->direction.x, -1.0f, 1.0f);
                ImGui::SliderFloat3((prefix + "Position").c_str(), &light->position.x, -10000.0f, 10000.0f);
                ImGui::SliderFloat((prefix + "FallOff Start").c_str(), &light->fallOffStart, 0.0f, 100000.0f);
                ImGui::SliderFloat((prefix + "FallOff End").c_str(), &light->fallOffEnd, 0.0f, 100000.0f);
                ImGui::SliderFloat((prefix + "Inner Angle").c_str(), &light->innerSpotAngle, 0.0f, 360.0f * D2R);
                ImGui::SliderFloat((prefix + "Outer Angle").c_str(), &light->spotAngle, 0.0f, 360.0f * D2R);
                ImGui::SliderFloat((prefix + "Intensity").c_str(), &light->intensity, 0.0f, 100000.0f);
                ImGui::TreePop(); 
            }
        }
        ImGui::TreePop();
    }
}

void ImguiManager::SeaController()
{
    if (ImGui::TreeNode("Sea Controller"))
    {
        if (_seaParam)
        {
       
            if (ImGui::TreeNode("SeaMove"))
            {
             
                ImGui::SliderInt("Wave Count", &_seaParam->wave_count, 0, 10);

             
                for (int i = 0; i < _seaParam->wave_count; i++)
                {
                    char label[64];

                    sprintf_s(label, "Wave %d Amplitude", i);
                    ImGui::SliderFloat(label, &_seaParam->waves[i].amplitude, 0.0f, 10.0f);

                    sprintf_s(label, "Wave %d Wavelength", i);
                    ImGui::SliderFloat(label, &_seaParam->waves[i].wavelength, 0.0f, 2000.0f);

                    sprintf_s(label, "Wave %d Speed", i);
                    ImGui::SliderFloat(label, &_seaParam->waves[i].speed, 0.0f, 10.0f);

                    sprintf_s(label, "Wave %d Steepness", i);
                    ImGui::SliderFloat(label, &_seaParam->waves[i].steepness, 0.0f, 300.0f);

                    sprintf_s(label, "Wave %d Direction", i);
                    ImGui::SliderFloat2(label, &_seaParam->waves[i].direction.x, -1.0f, 1.0f);

                    ImGui::Text("\n");
                }

     
                if (ImGui::Button("Save SeaMove to Binary File"))
                {
                    char szFileName[MAX_PATH] = "";
                    OPENFILENAMEA ofn;
                    ZeroMemory(&ofn, sizeof(ofn));
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = nullptr;
                    ofn.lpstrFilter = "Binary Files (*.bin)\0*.bin\0All Files (*.*)\0*.*\0";
                    ofn.lpstrFile = szFileName;
                    ofn.nMaxFile = MAX_PATH;
                    ofn.lpstrTitle = "Save SeaMove Data";
                    ofn.Flags = OFN_OVERWRITEPROMPT;

                    if (GetSaveFileNameA(&ofn))
                    {
                        std::ofstream file(szFileName, std::ios::binary);
                        if (file.is_open())
                        {
                            file.write(reinterpret_cast<const char*>(&_seaParam->wave_count), sizeof(_seaParam->wave_count));
                            for (int i = 0; i < _seaParam->wave_count; i++)
                            {
                                file.write(reinterpret_cast<const char*>(&_seaParam->waves[i]), sizeof(Wave));
                            }
                            file.close();
                        }
                        else
                        {
                            MessageBoxA(nullptr, "파일 열기에 실패했습니다.", "Error", MB_OK | MB_ICONERROR);
                        }
                    }
                }
                ImGui::TreePop(); 
            }

            if (ImGui::TreeNode("SeaColor"))
            {
                ImGui::SliderFloat3("SeaBaseColor", &_seaParam->seaBaseColor.x, 0.0f, 1.0f);
                ImGui::SliderFloat3("SeaShallowColor", &_seaParam->seaShallowColor.x, 0.0f, 1.0f);
                ImGui::SliderFloat("blendingFact", &_seaParam->blendingFact, 0.0f, 100.0f);
                ImGui::SliderFloat3("diffuseColor", &_seaParam->diffuseColor.x, 0.0f, 1.0f);
                ImGui::SliderFloat("specularPower", &_seaParam->specularPower, 0.0f, 512.0f);
                ImGui::SliderFloat3("sun_dir", &_seaParam->sun_dir.x, -1.0f, 1.0f);
                ImGui::SliderFloat("env_power", &_seaParam->env_power, 0.0f, 1.0f);

 
                if (ImGui::Button("Save SeaColor to Binary File"))
                {
                    char szFileName[MAX_PATH] = "";
                    OPENFILENAMEA ofn;
                    ZeroMemory(&ofn, sizeof(ofn));
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = nullptr;
                    ofn.lpstrFilter = "Binary Files (*.bin)\0*.bin\0All Files (*.*)\0*.*\0";
                    ofn.lpstrFile = szFileName;
                    ofn.nMaxFile = MAX_PATH;
                    ofn.lpstrTitle = "Save SeaColor Data";
                    ofn.Flags = OFN_OVERWRITEPROMPT;

                    if (GetSaveFileNameA(&ofn))
                    {
                        std::ofstream file(szFileName, std::ios::binary);
                        if (file)
                        {
                            file.write(reinterpret_cast<const char*>(&_seaParam->seaBaseColor), sizeof(_seaParam->seaBaseColor));
                            file.write(reinterpret_cast<const char*>(&_seaParam->seaShallowColor), sizeof(_seaParam->seaShallowColor));
                            file.write(reinterpret_cast<const char*>(&_seaParam->blendingFact), sizeof(_seaParam->blendingFact));
                            file.write(reinterpret_cast<const char*>(&_seaParam->diffuseColor), sizeof(_seaParam->diffuseColor));
                            file.write(reinterpret_cast<const char*>(&_seaParam->specularPower), sizeof(_seaParam->specularPower));
                            file.write(reinterpret_cast<const char*>(&_seaParam->sun_dir), sizeof(_seaParam->sun_dir));
                            file.write(reinterpret_cast<const char*>(&_seaParam->env_power), sizeof(_seaParam->env_power));
                            file.close();
                        }
                        else
                        {
                            MessageBoxA(nullptr, "파일 열기에 실패했습니다.", "Error", MB_OK | MB_ICONERROR);
                        }
                    }
                }
                ImGui::TreePop(); 
            }
        }
        ImGui::TreePop(); 
    }
}




void ImguiManager::DebugJin()
{
    ImGui::Begin("Jin");
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    Test();
    ImGui::End();

    ImGui::Begin("Inspector");
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    Test2();
    ImGui::End();
}


float a = 0;
std::weak_ptr<GameObject> selectGameObject;

void ImguiManager::Test2()
{
    if (ImGui::CollapsingHeader("Inspector"))
    {
        auto select = selectGameObject.lock();
        if (select != nullptr)
        {
            Gizmo::Width(0.02f);
            auto o = select->_transform->GetWorldPosition();
            auto f = select->_transform->GetForward();
            auto u = select->_transform->GetUp();
            auto r = select->_transform->GetRight();
            Gizmo::Width(0.1f);
            Gizmo::Line(o, o + f, Vector4(0, 0, 1, 1));
            Gizmo::Line(o, o + u, Vector4(0, 1, 0, 1));
            Gizmo::Line(o, o + r, Vector4(1, 0, 0, 1));
            Gizmo::WidthRollBack();


            ImGui::Text(std::format("Name : {0}", to_string(select->GetName())).c_str());
            ImGui::Text("Active : %s", select->GetActive() ? "true" : "false");
            ImGui::Text("ActiveSelf : %s", select->GetActiveSelf() ? "true" : "false");
            if (select->GetType() == GameObjectType::Dynamic) {
                ImGui::Text("Type : Dynamic");
            }
            else if (select->GetType() == GameObjectType::Deactivate) {
                ImGui::Text("Type : Deactivate");
            }
            else {
                ImGui::Text("Type : Static");
            }
            if (ImGui::TreeNode("Components"))
            {
                for (auto& com : select->GetComponentAll())
                {
                    auto& name = com->GetTypeName();
                    ImGui::PushID(com->GetInstanceID());
                    if (ImGui::TreeNode(name.c_str()))
                    {
                        if (name == "Transform")
                        {
							auto transform = std::static_pointer_cast<Transform>(com);
							ImGui::SliderFloat3("Position", &transform->_localPosition.x, -1000.0f, 1000.0f);
                            vec3 eu = transform->GetLocalEuler() * R2D;
                            if (ImGui::SliderFloat3("LocalEuler", &eu.x, -180.0f, 180.0f))
                                transform->SetLocalRotation(eu * D2R);
							ImGui::SliderFloat3("Scale", &transform->_localScale.x, 0.1f, 100.0f);
                        }
                        if (name == "LightComponent")
                        {
                            auto lightComponent = std::static_pointer_cast<LightComponent>(com);
                            ImGui::SliderFloat("Intensity", &lightComponent->light->intensity, 0.0f, 40.0f);
                            ImGui::SliderFloat4("Color", &lightComponent->light->strength.x, 0.0f, 1.0f);
                            ImGui::SliderFloat3("Direction", &lightComponent->light->direction.x, -1.0f, 1.0f);
                        }
                        if (name == "Collider")
                        {
							
                        }

                        ImGui::TreePop();
                    }
                    ImGui::PopID();
                }
                ImGui::TreePop();
            }
        }
    }
}

void ImguiManager::VolumetricTest()
{
    if (_volumetricData != nullptr)
    {
        if (ImGui::TreeNode("_volumetricData"))
        {
            ImGui::SliderFloat("Absorption", &_volumetricData->absorption, 0.0f, 10.0f);
            ImGui::SliderFloat3("color", &_volumetricData->fogColor.x, 0, 1.0f);
            ImGui::SliderInt("numSlice", &_volumetricData->numSlices, 0.0f, 50);
            ImGui::SliderFloat("phase", &_volumetricData->phaseG, 0.0f, 10.0f);
            ImGui::SliderFloat("waterHeight", &_volumetricData->waterHeight, 0.0f, 3000.0f);
            ImGui::SliderInt("numstep", &_volumetricData->numSteps, 0.0f, 300);
            ImGui::SliderFloat("stepSize", &_volumetricData->stepSize, 0.0f, 3.0f);
            ImGui::TreePop();
        }
    }
}

void ImguiManager::Test()
{
    if (ImGui::CollapsingHeader("Scene Hierarchy"))
    {
		auto currentScene = SceneManager::main->GetCurrentScene();

        if (ImGui::TreeNode("GameObjects"))
        {
            auto objectList = currentScene->_gameObjects;
            objectList.insert(objectList.end(), currentScene->_gameObjects_deactivate.begin(), currentScene->_gameObjects_deactivate.end());
            std::ranges::sort(objectList, [&](const auto& a, const auto& b) {
                    return a->GetName() < b->GetName();
                });
            for (auto& obj : objectList)
            {
                if (obj->GetRoot() == obj)
                {
	                struct ObjectBlock
	                {
						bool isOpen = false;
                        operator bool() const { return isOpen; }
	                };
                    obj->ForHierarchyBeginEndAll([&](const std::shared_ptr<GameObject>& obj) {
						ObjectBlock block;
                        auto str = to_string(obj->GetName(), CP_UTF8);
                        ImGui::PushID(obj->GetInstanceID());
                        block.isOpen = ImGui::TreeNode(str.c_str());

                        if (ImGui::IsItemClicked() || ImGui::IsItemClicked(ImGuiMouseButton_Right))
                            selectGameObject = obj;
                        

                        if (block)
                        {

                        }
                        return block;
                    }, [&](const ObjectBlock& block, const std::shared_ptr<GameObject>& obj) {
						if (block)
						{
							ImGui::TreePop();
						}
                        ImGui::PopID();
                    });
                }
            }
            ImGui::TreePop();
        }
        /*
        if (_seaParam)
        {

            if (ImGui::TreeNode("Anim"))
            {

                ImGui::SliderFloat("a", &a, 0, 1);
            }
        }
        */
    }
    if (ImGui::CollapsingHeader("Jin"))
    {
	    if (ImGui::TreeNode("Jin Test"))
	    {
	        ImGui::SliderFloat("testValue", &testValue, 0, 4);
	        /*
	        if (_seaParam)
	        {

	            if (ImGui::TreeNode("Anim"))
	            {

	                ImGui::SliderFloat("a", &a, 0, 1);
	            }
	        }
			*/
	        ImGui::TreePop();
	    }
    }

}
