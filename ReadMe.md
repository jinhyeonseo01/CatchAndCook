# Catch & Cook
![Static Badge](https://img.shields.io/badge/C%2B%2B-%2300599C?style=for-the-badge&logo=cplusplus)
![Static Badge](https://img.shields.io/badge/Graphics%20API-DirectX12-76B900?style=for-the-badge&logo=nvidia&logoColor=%2376B900)
![License Badge](https://img.shields.io/github/license/jinhyeonseo01/CatchAndCook?style=for-the-badge)
![GitHub Repo stars](https://img.shields.io/github/stars/jinhyeonseo01/CatchAndCook?style=for-the-badge&logo=github)
![GitHub Release](https://img.shields.io/github/v/release/jinhyeonseo01/CatchAndCook?style=for-the-badge&logo=Github)  

C++ 20과 DirectX 12 기반의 프레임워크 및 렌더링 파이프라인을 설계·구축하여 개발된 3인칭 게임 프로젝트.  

<details>
<summary>English</summary>

The third-person game project developed by designing and building a C++20 and DirectX 12–based framework and rendering pipeline.
</details>  

<br>

![image](./docs/Main2.png)  

<details>
<summary>More</summary>

![image](./docs/Main.png)  
![image](./docs/Main3.png)  
![image](./docs/Main4.png)  

</details>  


### 개발 목적
 - C++20과 DirectX 12를 활용해 렌더링 파이프라인 프레임워크를 직접 설계·구축하고, 이를 기반으로 플레이 가능한 3인칭 게임을 개발하기 위함  
 - 이 과정을 통해 그래픽스 및 코어 엔진 엔지니어링에 대한 이해를 심화하기 위함  

<details>
<summary>English</summary>

 - To design and implement a rendering‑pipeline framework using C++20 and DirectX 12, and develop a playable third‑person game based on it  
 - To deepen my understanding of graphics and core‑engine engineering through this process  
</details>  

### 사용 기술 & 서드파티
 - C++ 20 (*MSVC*)  
 - DirectX12  
 - [ImGUI](https://github.com/ocornut/imgui)  
 - [Assimp](https://github.com/assimp/assimp)  
 - [FMOD](https://www.fmod.com)  
 - Unity ( *Level Design Tools* )  
   - [Asset Store](https://assetstore.unity.com/packages/tools/utilities/scene-object-json-exporter-322005)

### 개발 기간
 - **8개월 (2025.01.15 ~ 2025.07.28)**  

### 팀원 / 역할 분담
| 이름                        | 역할               |
|----------------------------|----------------------|
| **진현서 / Jin Hyeon Seo (Clrain)** | [클라이언트 – 육지 파트 ( *Client – Land Content* )](#jin) |
| **김상혁 / Kim Sang Hyuk**          | [클라이언트 – 해상 파트 ( *Client – Maritime Content* )](#kim) | 

**(DirectX12의 기반 렌더링 및 로직 파이프라인 전반의 대부분을 공동으로 상의하며 함께 설계함.)**


### 게임 개요  
   - 육지와 해상이 나뉘어진 공간에서 플레이
   - 낮에는 식당을 경영하기 위하여 직접 해상에서 재료들을 수급하고, 밤에는 손님들에게 음식을 제공하여 돈을 버는 방식
   - 성장형 경영 타이쿤 게임.  
<details>
<summary>English</summary>

 - Set in an environment split between land and sea  
 - During the day, you source ingredients directly from the sea to run your restaurant; at night, you serve food to customers to earn money  
 - A growth‑oriented management tycoon game  
</details>  


<details>
<summary>줄거리</summary>

   - 망망대해 위에 어느 한 섬. 희귀한 광물 **미스라이트**가 풍부한 것으로 알려진 이 섬에는 무역상인들을 중심으로 한 모험가와 귀족상단이 주둔하여 광물을 채집하고 무역을 하고 있었다. 사람은 모여있고, 돈은 많고, 먹을 것은 부족한 이 섬에서 주인공은 식장을 개업하기로 하는데...  
</details>  

<br><br>  

# Project Overview  
**Youtube - Devlog**  
 - [[Devlog] DirectX12 - Post Processing](https://youtu.be/-TeBrZH87XQ)  
 - [[Devlog] DirectX12 - Character NPR Toon Shader](https://youtu.be/TLnCw8tBEnE)  
 - [[Devlog] DirectX12 - Cascade Shadow Mapping](https://youtu.be/0_6SBgFWlvk)  
 - [[Devlog] DirectX12 - Stylized Water Shader](https://youtu.be/ExiXoHy73so)  
 - [[Devlog] DirectX12 - Collision & PathFinding](https://youtu.be/xzuFgoVAOT0)  
 - [[Devlog] DirectX12 - Skinned Mesh Animation](https://youtu.be/XBQ563GVM80)  

### 맵소개
#### 인트로 / Intro
![image](./docs/Animation77.gif)  

#### 식당 / Restaurant
![image](./docs/Animation111.gif)  

#### 성 마을 / Fortified Village
![image](./docs/Animation113.gif)  

#### 신비한 연못 / Mysterious Pond
![image](./docs/Animation115.gif)  

#### 중앙 마을 / Middle Village
![image](./docs/Animation117.gif)  

#### 부둣가 / Dockside
![image](./docs/Animation119.gif)  

#### 바닷속 / UnderWater
![image](./docs/Animation121.gif)  

* * *  
<br><br>  

# Technical Overview

## Pipeline Flow
![image](./docs/Flow%20Chart.png)  

### Render Pass Flow
![image](./docs/Flow%20Chart%20Pass.png)  

* * *  
<br><br>  

## Technologies Used

### Skinned Mesh Rendering
![image](./docs/Animation87.gif)  

### NavMesh & PathFinding  
[[ YouTube ] DirectX12 - Collision & PathFinding](https://youtu.be/xzuFgoVAOT0)  
![image](./docs/Animation79.gif)  
![image](./docs/Animation84.gif)  

### Collision
![image](./docs/Animation90.gif)  

### Frustum Culling
![image](./docs/Animation102.gif)  
![image](./docs/Animation103.gif)  

### Static & Dynamic GPU Instancing  
> Object Count
> **(5000 objects, 30000 trees, 100,000 grass)**  

![image](./docs/Animation86.gif)  

### Cascade Shadow Mapping (CSM)  
**4‑Cascade LOD**
[[ YouTube ] DirectX12 - Cascade Shadow Mapping](https://youtu.be/0_6SBgFWlvk)  
![image](./docs/Animation109.gif)  
![image](./docs/Animation107.gif)  

### Terrain Tessellation  
![image](./docs/Animation81.gif)  
![image](./docs/Animation99.gif)  

### Deferred Rendering (Lots of Additional Light)   
![image](./docs/Deferred.png)  

### Post Processing  
#### Final Visualization
[[ YouTube ] DirectX12 - Post Processing](https://youtu.be/-TeBrZH87XQ)  
![image](./docs/Animation93.gif)  
![image](./docs/Animation54.gif)  

#### Screen Space Ambient Occlusion (SSAO)  
![image](./docs/Animation82.gif)  

### Time of Day
![image](./docs/Animation58.gif)  

### Stylized Water Shader
[[ YouTube ] DirectX12 - Stylized Water Shader](https://youtu.be/ExiXoHy73so) 
![image](./docs/Animation88.gif)  

### Character Non-Photo Realistic (NPR) based Shading
[[ YouTube ] DirectX12 - Character NPR Toon Shader](https://youtu.be/TLnCw8tBEnE)  
#### SDF Face Shadow
![image](./docs/Animation46.gif)  

#### Subsurface scattering (SSS)
![image](./docs/Animation94.gif)  

#### Sobel Filter based Outline & Specular
![image](./docs/Animation97.gif)  

**(+Rim Light, Simple GI)**
### Seawater Visualization & Waves
![image](./docs/Animation105.gif)  

### Unity-based Level-Design(*Scene*) Loader

![image](./docs/Scene%20Loader.png)  

#### Scene&Object Json Exporter  
**The project’s functionality was modularized and released as an asset.**  
[Click to visit the Asset Store](https://assetstore.unity.com/packages/tools/utilities/scene-object-json-exporter-322005)  

![Asset Store](./docs/Asset%20Store.png)  

* * *  
<br><br>  

## Development Tasks
#### 공동 개발
<details>  
  <summary>상세 개발 내용</summary> 

- **Overall DirectX 12 setup and development**
- Render Pass Policy
- Vertex, Material, Shader, Pipeline Policy
- Mesh Rendering Flow
- Root Signature
- Light Logic
- Dynamic Instancing & Batching
- Frustum Culling
- Mesh‑based Pass System
- Scene System, Scene Manager

</details>  

#### 진현서 / Jin Hyeon Seo (Clrain)

<details>  
  <summary>상세 개발 내용</summary>  

<a id="jin"></a>

**Content**
 - 육지 컨텐츠 전담 개발 
 - 메인 메뉴, 팝업 메뉴, InGame GUI

**Core Systems**
 - GameObject, Component, GUID‑based Object‑Control System
 - Transform Hierarchy System
 - Collider Raycasting System
 - Unity based level Scene Loader
 - Model Resource Loader
 - NavMesh based ASter(+Funnel) PathFinding
 - UI - Sprite Rect Transform based GUI System
 - Debug - Gizmo
 - Debug - Performance Profiler 
 - Finite state machine(FSM) based NPC System
 - Shader Compiler, Profiler
 - Terrain Static Instancing (*tree, grass*)

**Rendering**  
 - Geometry‑based Gizmo Rendering
 - Forward (*+Transparent*) Rendering
 - Resource Dynamic Matching&Linking System
 - Pipeline State Object (PSO) Policy
 - Skinned Mesh, Animation System
 - Deferred Environment Visual Shading
 - Character NPR based Shading
   - SDF Face Shadow
   - Sobel Filter based Outline, Specular
   - Subsurface scattering (SSS)
   - Rim
   - Ramp Texture based shading
 - Cascade Shadow Mapping (CSM)
 - Stylized Water Shader
 - Post Processing (Compute Shader)
   - Depth of Field
   - Bloom, Blur (*Optimization*)
   - God Ray
   - Time of Day
   - Screen Space Ambient Occlusion (SSAO)
   - Fast approximate Anti-Aliasing (FXAA)
   - Color Grading
   - Vignette

</details>  

#### 김상혁 / Kim Sang Hyuk
<details>  
  <summary>상세 개발 내용</summary>  

<a id="kim"></a>

**Content**
 - 해상 컨텐츠 전담 개발
 - All Particle Effects Contant (*폭죽, 연기, 물안개, 불 등등*)
 - 육지 - Boat Controller
 - 육지 - Sea Water Visual, Movement

**Core Systems**
 - Call based Collision Division Space
 - Path Trail Mover System
 - UI - Text, Texture Rendering System
 - Buffer Pool, Manager
 - Resource Manager
 - Debug - IMGUI System Link
 - Sound System
 - Texture Loader System
 - Render Texture System
 - Boids Algorithm based Swarm Control

**Rendering**  
 - DirectX Default Initialization
 - Deferred Rendering
 - Terrain Tessellation
 - Compute Shader Pipeline
 - Nested Frame Rendering
 - Underwater Visual Shading & Effects
   - Phong‑based Rendering
   - Rim
 - 육지 - Sea Water Shader, Wave Movement
 - Geometry, Stream-Output based Particle System
 - Post Processing (Compute Shader)
   - Blur
   - Bloom
   - Fog Factor
   - Depth Capture
   - Under Water Post Effect
   - 	Dissolve (*Fade‑In/Out*)
   - Scattering

</details>  

* * *  

Middle Present Execute : https://drive.google.com/file/d/1GvVb6xl6p1NtAi3A1yd_0g4KXDfFq1Eu/view?usp=drive_link
Last Present Execute : https://drive.google.com/file/d/1mJVY8-xbjTHjggJgFJks5_SR9z3ktg3_/view?usp=sharing