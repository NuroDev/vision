#include "Application.h"

#define _XM_NO_INTRINSICS_
#define MS_PER_UPDATE 16.6666666667

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Create a new application instance
	std::unique_ptr<Application> pApplication(new Application("Assets/Config.toml"));
    if (FAILED(pApplication->Init(hInstance, nCmdShow)))
        return -1;

    // Main message loop
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        const float frameStartTime = static_cast<float>(GetTickCount());

        pApplication->Update(MS_PER_UPDATE / 1000);
        pApplication->Draw();

        const float currentFrameTime = static_cast<float>(GetTickCount64());
        const float frameProcessingTime = currentFrameTime - frameStartTime;

        if (frameProcessingTime < 16.67f)
            Sleep(static_cast<DWORD>(16.67f - frameProcessingTime));
    }

    return static_cast<int>(msg.wParam);
}
