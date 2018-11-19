//
// pch.h
// Header for standard system include files.
//

#pragma once
#include <WinSDKVer.h>
#define _WIN32_WINNT 0x0602
#include <SDKDDKVer.h>
//#include <wrl.h>
//#include <stdint.h>
// Use the C++ standard templated min/max
#define NOMINMAX

// DirectX apps don't need GDI
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

// Include <mcx.h> if you need this
#define NOMCX

// Include <winsvc.h> if you need this
#define NOSERVICE

// WinHelp is deprecated
#define NOHELP

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <wrl/client.h>

#include <d3d11_1.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <stdio.h>
#include <io.h>
#include <streambuf>
#include <algorithm>
#include <exception>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <climits>
#include <limits>
#include <stdexcept>
#include <dirent.h>
#include <stdio.h>
#include <mutex>
#include <shared_mutex>
#include <deque>
using std::string;
using std::vector;
using std::map;
using std::unordered_map;
using std::unordered_set;

//#include <experimental/filesystem>
#include <filesystem>

namespace Filesystem = std::filesystem;
namespace DX
{
    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            // Set a breakpoint on this line to catch DirectX API errors
            throw std::exception();
        }
    }
}
class Entity;
typedef std::shared_ptr<Entity> EntityPtr;
#include "Audio.h"
#include "CommonStates.h"
#include "DDSTextureLoader.h"
#include "DirectXHelpers.h"
#include "Effects.h"
#include "GamePad.h"
#include "GeometricPrimitive.h"
#include "GraphicsMemory.h"
#include "Keyboard.h"
#include <Model.h>
#include "Mouse.h"
#include "PrimitiveBatch.h"
#include "ScreenGrab.h"
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "VertexTypes.h"
#include "WICTextureLoader.h"
#include "ReadData.h"
#include "ConversionUtil.h"
#include "Utility.h"
#include "Geometry.h"
#include "ProUtil.h"
#include "MathConstants.h"
