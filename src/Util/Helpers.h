#pragma once

#include <codecvt>
#include <d3d11.h>
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <string>
#include <Windows.h>

#define SAFE_RELEASE(x, ...) { if(x) { x->Release(); x = nullptr; } }

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class Helpers
{
public:
    //--------------------------------------------------------------------------------------
    // Helper for compiling shaders with D3DCompile
    // With VS 11, we could load up prebuilt .cso files instead...
    //--------------------------------------------------------------------------------------
    static HRESULT CompileShaderFromFile(const WCHAR *szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob **ppBlobOut)
    {
        HRESULT hr;

        DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef _DEBUG
        // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
        // Setting this flag improves the shader debugging experience, but still allows
        // the shaders to be optimized and to run exactly the way they will run in
        // the release configuration of this program.
        dwShaderFlags |= D3DCOMPILE_DEBUG;

        // Disable optimizations to further improve shader debugging
        dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        ID3DBlob *pErrorBlob = nullptr;
        hr = D3DCompileFromFile(szFileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, szEntryPoint, szShaderModel,
                                dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
        if (FAILED(hr))
        {
            if (pErrorBlob)
            {
                OutputDebugStringA(reinterpret_cast<const char *>(pErrorBlob->GetBufferPointer()));
                pErrorBlob->Release();
            }
            return hr;
        }
        if (pErrorBlob)
            pErrorBlob->Release();

        return hr;
    };

    //--------------------------------------------------------------------------------------
    // Helper to convert a string to a wstring
    //--------------------------------------------------------------------------------------
    static std::wstring StrToWstr(const std::string text)
    {
    	// TODO: This converstion is messy but safe. Is there a nicer way of doing this?
        const size_t size = 100;
        size_t length = (text.length() + 1);
        size_t numConvertedChars = 0;
        wchar_t wText[size];
        mbstowcs_s(&numConvertedChars, wText, length, text.c_str(), _TRUNCATE);
        return wText;
    }

    //--------------------------------------------------------------------------------------
    // Called every time the application receives a message
    //--------------------------------------------------------------------------------------
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        PAINTSTRUCT ps;
        HDC hdc;

        if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
            return true;

        switch (message)
        {
        case WM_LBUTTONDOWN:
        {
            // TODO: Are these inherited variables needed?
            //int xPos = GET_X_LPARAM(lParam);
            //int yPos = GET_Y_LPARAM(lParam);
            break;
        }

        case WM_PAINT:
        {
            hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;
        }

        case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }

        return 0;
    };
};
