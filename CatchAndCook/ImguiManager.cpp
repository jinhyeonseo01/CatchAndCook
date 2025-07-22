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
#include "RectTransform.h"
#include "Volumetric.h"
#include "BoatController.h"
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
    if (Input::main->GetKeyDown(KeyCode::F5))
    {
        _IMGUION = !_IMGUION;
    }

    if (!_IMGUION)
        return;



    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();


    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_FirstUseEver);

    ImGui::Begin("Main GUI", nullptr, ImGuiWindowFlags_NoMove );

    if (ImGui::BeginTabBar("MainTabs"))
    {
        if (ImGui::BeginTabItem("Debug"))
        {
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
            ImGui::Text("ForwardCount: %d", g_debug_forward_count);
            ImGui::Text("DeferredCount: %d", g_debug_deferred_count);
            ImGui::Text("ForwardCullingCount: %d", g_debug_forward_culling_count);
            ImGui::Text("DeferredCullingCount: %d", g_debug_deferred_culling_count);
            ImGui::Text("shadowDrawCall: %d", g_debug_shadow_draw_call);
            ImGui::Text("DrawCall: %d", g_debug_draw_call);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Gizmo"))
        {
            GizmoController();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Objects"))
        {
            SeaController();
            LightController();
            BoidMove();
            Sky();
            LimLightControl();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Compute"))
        {
            ComputeController();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Player Camera"))
        {
            if (playerHeightOffset)
            {
                ImGui::SliderFloat("playerHeightOffset", playerHeightOffset, 0.0f, 6.0f);
                ImGui::SliderFloat("playerForwardOffset", playerForwardOffset, -6.0f, 6.0f);
                ImGui::SliderFloat("cameraPitchOffset", cameraPitchOffset, -90.0f, 90.0f);
                ImGui::SliderFloat("cameraYawOffset", cameraYawOffset, -10.0f, 10.0f);
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Boat (Scene2 Only)"))
        {
            if (SceneManager::main->GetCurrentScene()->GetSceneType() == SceneType::TestScene2)
            {
                ImGui::SliderFloat("heightOffset", &BoatController::heightOffset, 0.0f, 50.0f);
                ImGui::SliderFloat("SpringArmLength", &BoatController::SpringArmLength, 0.0f, 50.0f);
                ImGui::SliderFloat3("boatPitch", &BoatController::_pitch, 0.0f, 360.0f);
            }
            else
            {
                ImGui::Text("Not in TestScene2");
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Camera Select"))
        {
            static CameraType selectedCameraType = CameraType::DebugCamera;

            auto& cameras = CameraManager::main->GetCameraAlls();

            static std::vector<CameraType> cameraKeys;
            static std::vector<const char*> cameraNames;

            if (cameraKeys.size() != cameras.size())
            {
                cameraKeys.clear();
                cameraNames.clear();

                for (auto& [key, cam] : cameras)
                {
                    cameraKeys.push_back(key);
                    cameraNames.push_back(CameraManager::main->CameraTypeToString(key));
                }
            }

            int currentIndex = 0;
            for (int i = 0; i < (int)cameraKeys.size(); ++i)
            {
                if (cameraKeys[i] == selectedCameraType)
                {
                    currentIndex = i;
                    break;
                }
            }

            if (ImGui::Combo("Select Camera", &currentIndex, cameraNames.data(), (int)cameraNames.size()))
            {
                selectedCameraType = cameraKeys[currentIndex];
                CameraManager::main->SetActiveCamera(selectedCameraType);
            }

            ImGui::EndTabItem();
        }

    /*    if (ImGui::BeginTabItem("Jin"))
        {
            Test();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Inspector"))
        {
            Test2();
            ImGui::EndTabItem();
        }*/

        ImGui::EndTabBar();
    }

    ImGui::End();
    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), Core::main->GetCmdList().Get());
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
    if (ImGui::CollapsingHeader("Post Effects"))
    {
        if (ImGui::Checkbox("Blur", _blurPtr)) {}
        if (ImGui::Checkbox("Bloom", _bloomPtr)) {}
        if (ImGui::Checkbox("SSAO", _ssaoOnOff)) {}
        if (ImGui::Checkbox("Color Grading", _colorGradingOnOff)) {}
        if (ImGui::Checkbox("FXAA", _fxaa)) {}
        if (ImGui::Checkbox("DOF", _dofPtr)) {}
        if (ImGui::Checkbox("Vignette", mainField_vignette)) {}
    }

    if (ImGui::CollapsingHeader("Lighting & Shadow"))
    {
        if (ImGui::Checkbox("Shadow", _shadowOnOff)) {}
        if (ImGui::Checkbox("Baked GI", _bakedGIOnOff)) {}
        if (ImGui::Checkbox("God Ray", _godRayPtr)) {}
        if (ImGui::Checkbox("Fog", mainField_fog)) {}
        if (ImGui::Checkbox("Total Toggle", mainField_total))
        {
            *_shadowOnOff = *mainField_total;
            *_bakedGIOnOff = *mainField_total;
        }
    }

    if (ImGui::CollapsingHeader("Depth Fog Debug"))
    {
        static bool showDepthRender = false;
        ImGui::Checkbox("Show Depth Fog UI", &showDepthRender);

        if (showDepthRender)
        {
            ImGui::SliderInt("Render Type", &_fogParam->depthRendering, 0, 2);
            ImGui::SliderFloat("Fog Max", &_fogParam->g_fogMax, 0.0f, 5000.0f);
            ImGui::SliderFloat("Fog Min", &_fogParam->g_fogMin, 0.0f, 5000.0f);
            ImGui::SliderFloat3("Fog Color", &_fogParam->g_fogColor.x, 0.0f, 1.0f);
            ImGui::SliderFloat("Fog Power", &_fogParam->power, 0.0f, 30.0f);
        }
    }

    if (ImGui::CollapsingHeader("Underwater Effect"))
    {
        if (ImGui::Checkbox("Enable Underwater", (bool*)&_underWaterParam->g_on))
        {
            _underWaterParam->g_on = _underWaterParam->g_on ? 1 : -1;
        }

        if (_underWaterParam->g_on == 1)
        {
            ImGui::SliderFloat3("Fog Color", &_underWaterParam->g_fogColor.x, 0.0f, 1.0f);
            ImGui::SliderFloat("Fog Power", &_underWaterParam->g_fog_power, 0.0f, 30.0f);
            ImGui::SliderFloat("Fog Max", &_underWaterParam->g_fogMax, 0.0f, 5000.0f);
            ImGui::SliderFloat("Fog Min", &_underWaterParam->g_fogMin, 0.0f, 5000.0f);
            ImGui::SliderFloat3("Underwater Color", &_underWaterParam->g_underWaterColor.x, 0.0f, 1.0f);
        }
    }

    if (ImGui::CollapsingHeader("Scattering"))
    {
        if (ImGui::Checkbox("Enable Scattering", _scattering)) {}

        if (*_scattering)
        {
            ImGui::SliderFloat("Phase G", &_scatteringData->phaseG, 0.0f, 1.0f);
            ImGui::SliderFloat("Absorption", &_scatteringData->absorption, 0.0f, 1000.0f);
            ImGui::SliderFloat("Density", &_scatteringData->density, 0.0f, 15.0f);
            ImGui::SliderFloat3("Scatter Color", &_scatteringData->scatterColor.x, 0.0f, 1.0f);
            ImGui::SliderFloat3("direction ", &_scatteringData->direction.x, -1.0f, 1.0f);
        }
    }
}


void ImguiManager::LightController()
{
    if (ImGui::TreeNode("Lighting"))
    {
        auto& lights = LightManager::main->GetLights();

        size_t size = lights.size();

        for (size_t i = 0; i < size; i++)
        {
            auto light = lights[i];

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
                        if (name == "RectTransform")
                        {
                            auto transform = std::static_pointer_cast<RectTransform>(com);
                            ImGui::SliderFloat3("Position", &transform->_localPosition.x, -1000.0f, 1000.0f);
                            vec3 eu = transform->GetLocalEuler() * R2D;
                            if (ImGui::SliderFloat3("LocalEuler", &eu.x, -180.0f, 180.0f))
                                transform->SetLocalRotation(eu * D2R);
                            ImGui::SliderFloat3("Scale", &transform->_localScale.x, 0.1f, 100.0f);

                            auto& d = transform->_rectTransformData;
                            ImGui::Separator();
                            ImGui::Text("RectTransform");

                            ImGui::SliderFloat2("Pivot", &d.pivot.x, 0.0f, 1.0f);
                            ImGui::SliderFloat2("Anchor Min", &d.anchorMin.x, 0.0f, 1.0f);
                            ImGui::SliderFloat2("Anchor Max", &d.anchorMax.x, 0.0f, 1.0f);

                            // rectSize 와 rectSize 는 음수/임의 값이 필요할 수 있으니 InputFloat2 로
                            ImGui::InputFloat2("Size Delta", &d.rectSize.x);
                            ImGui::InputFloat2("Rect Size", &d.rectSize.x);

                            auto& c = transform->_computedRect;
                            ImGui::Separator();
                            ImGui::Text("ComputedRect");
                            ImGui::InputFloat3("LocalPos3D", &c.localPosition.x);

                            ImGui::Text("LocalRect2D Min/Max");
                            ImGui::InputFloat2("  Min", &c.absoluteRect.min.x);
                            ImGui::InputFloat2("  Max", &c.absoluteRect.max.x);

                            ImGui::Text("AdjRect2D Min/Max");
                            ImGui::InputFloat2("  Min", &c.relativeRect.min.x);
                            ImGui::InputFloat2("  Max", &c.relativeRect.max.x);

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

void ImguiManager::LimLightControl()
{
        if (ImGui::TreeNode("RimLightController"))
        {
            ImGui::SliderFloat("rimPower", &LightManager::main->_lightParmas.rimPower, 0.0f, 50.0f);
            ImGui::SliderFloat("rimStrength", &LightManager::main->_lightParmas.rimStrength, 0.0f, 50.0f);
            ImGui::TreePop();
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

    }
    if (ImGui::CollapsingHeader("Jin"))
    {
	    if (ImGui::TreeNode("Jin Test"))
	    {
	        ImGui::SliderFloat("testValue", &testValue, 0, 4);
	
	        ImGui::TreePop();
	    }
    }

}
