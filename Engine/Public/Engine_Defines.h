#pragma once

#pragma warning (disable : 4251)

#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>

#define DIRECTINPUT_VERSION  0x0800
#include <dinput.h>

#include "DirectXTK/DDSTextureLoader.h"
#include "DirectXTK/WICTextureLoader.h"
#include "DirectXTK/VertexTypes.h"
#include "DirectXTK/SpriteBatch.h"
#include "DirectXTK/SpriteFont.h"
#include "DirectXTK/PrimitiveBatch.h"
#include "DirectXTK/Effects.h"


#include "Fx11/d3dx11effect.h"
#include <d3dcompiler.h>
using namespace DirectX;

#pragma region FMOD
//sound
#include <io.h>

#include "fmod.h"
#include "fmod.hpp"
//#pragma comment(lib, "fmodex_vc.lib")
// Debug 일 때
#pragma comment(lib, "fmodL_vc.lib")
// Release 일 때
#pragma comment(lib, "fmod_vc.lib")

using namespace FMOD;
#pragma endregion
#pragma region IMGUI
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "../../External Libraries/ImGuizmo-master/ImGuizmo.h" 
#pragma endregion

#include <string>
#include <map>
#include <list>
#include <vector>
#include <locale>
#include <fstream>  
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <functional>
#include <optional>
#include <algorithm>
using namespace std;


#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Enum.h"
#include "Engine_Function.h"
#include "Engine_Typedef.h"




namespace Engine
{
	const _wstring	g_strTransformTag = TEXT("Com_Transform");
	const _uint g_iMaxNumBones = 512;

	//const unsigned int g_iMaxWidth = 16384;
	//const unsigned int g_iMaxHeight = 9216;	

	const unsigned int g_iMaxWidth =  4096;
	const unsigned int g_iMaxHeight = 2304;
}

using namespace Engine;




#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif
#endif



