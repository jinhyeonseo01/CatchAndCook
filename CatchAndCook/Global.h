#pragma once
#include "Vertex.h"

extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;

extern bool Initalize;

extern bool BakingLoadMode;

//
extern DXGI_FORMAT SWAP_CHAIN_FORMAT;
extern DXGI_FORMAT DEPTH_STENCIL_FORMAT;

extern size_t CBV_ROOT_INDEX_COUNT;
extern size_t CBV_TABLE_INDEX;
extern size_t SRV_TABLE_INDEX;
extern size_t SRV_LONG_TABLE_INDEX;
extern size_t SRV_STRUCTURED_TABLE_INDEX;

extern size_t UAV_TABLE_INDEX;
extern size_t SPRITE_TABLE_INDEX;

const size_t CBV_TABLE_COUNT = 32;

const size_t SRV_TABLE_REGISTER_COUNT = 16;
const size_t SRV_LONG_TABLE_REGISTER_COUNT = 60;
const size_t SRV_LONG_TABLE_REGISTER_OFFSET = 40;
const size_t SRV_STRUCTURED_TABLE_REGISTER_COUNT = 10;
const size_t SRV_STRUCTURED_TABLE_REGISTER_OFFSET = 30;
extern size_t UAV_TABLE_REGISTER_COUNT;

extern std::vector<VertexProp> SkinProp;
extern std::vector<VertexProp> StaticProp;
extern std::vector<VertexProp> GeoMetryProp;
extern std::vector<VertexProp> ColorProp;
extern std::vector<VertexProp> GizmoInstanceProp;

extern std::vector<VertexProp> TransformInstanceProp;


const uint32 SWAP_CHAIN_FRAME_COUNT = 2; 
const uint32 MAX_FRAME_COUNT = SWAP_CHAIN_FRAME_COUNT - 1;
extern uint32 CURRENT_CONTEXT_INDEX;

extern size_t GLOBAL_SRV_SHADOW_INDEX;
extern size_t GLOBAL_SRV_POSITION_INDEX;
extern size_t GLOBAL_SRV_NORAML_INDEX;
extern size_t GLOBAL_SRV_COLOR_INDEX;
extern size_t GLOBAL_SRV_MAO_INDEX;
extern size_t GLOBAL_SRV_DEPTH_INDEX;
extern size_t SRV_LIGHTPARM_TABLE_INDEX;
extern size_t GLOBAL_SRV_INDEX;


extern size_t g_debug_forward_count;
extern size_t g_debug_forward_culling_count;
extern size_t g_debug_deferred_count;
extern size_t g_debug_deferred_culling_count;
extern size_t g_debug_draw_call;
extern size_t g_debug_shadow_draw_call;

//extern size_t GLOBAL_SRV_DEPTH_INDEX;

extern size_t debug_test;