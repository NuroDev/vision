#pragma once

#include <windows.h>

#include "../Config/Structrures.h"
#include "../Util/Resource.h"

class Window
{
public:
	/// [WINDOW]
    HRESULT Init(HINSTANCE hInstance, const int nCmdShow);

	/// [WINDOW] Cleanup and release
    void Release();

    /// [CMD SHOW] Get the cmd show
    int GetCmdShow() const { return m_cmdShow; };

    /// [CMD SHOW] Set the cmd show
    void SetCmdShow(int cmdShow) { m_cmdShow = cmdShow; };

	/// [WINDOW] Get the window singleton instance
    static Window& Get();

	/// [INSTANCE] Get the HINSTANCE
    HINSTANCE GetHInstance() const { return m_hInstance; };

    /// [INSTANCE] Set the HINSTANCE
    void SetHInstance(HINSTANCE hInstance) { m_hInstance = hInstance; };

    /// [INSTANCE] Get the application parameters object
    ApplicationParameters* GetParams() const { return m_pParameters; };

    /// [INSTANCE] Set the application params object
    void SetParams(ApplicationParameters* pParams) { m_pParameters = pParams; };

    /// [WINDOW] Get the window
    HWND GetWindow() const { return m_hWnd; };

    /// [WINDOW] Get the window's resolution height
    unsigned int GetHeight() const { return m_pParameters->height; };
	
	/// [WINDOW] Get the window's resolution width
    unsigned int GetWidth() const { return m_pParameters->width; };
	
    /// [WINDOW] Set the window
    void SetWindow(HWND hWnd) { m_hWnd = hWnd; };

private:
	Window() = default;
    Window(const Window&);
    Window& operator= (const Window&);

    /// [CMD SHOW]
    int m_cmdShow;
	
	/// [INSTANCE]
	static std::unique_ptr<Window> m_pInstance;

    /// [PARAMETERS]
    ApplicationParameters* m_pParameters = nullptr;
	
	/// [WINDOW]
	HINSTANCE m_hInstance;
	HWND m_hWnd;
};
