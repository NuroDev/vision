#include "Window.h"

std::unique_ptr<Window> Window::m_pInstance = std::unique_ptr<Window>();

HRESULT Window::Init(HINSTANCE hInstance, const int nCmdShow)
{
    HRESULT hr = S_OK;

    // Store the initialization data
    m_hInstance = hInstance;
    m_cmdShow = nCmdShow;

    // Create an initial application config
    if (m_pParameters == nullptr)
        m_pParameters = new ApplicationParameters();

    // Convert std string to LPCWSTR
    std::wstring titleW(m_pParameters->title.length(), L' ');
    std::copy(m_pParameters->title.begin(), m_pParameters->title.end(), titleW.begin());
    const LPCWSTR title = static_cast<LPCWSTR>(titleW.c_str());

    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = Helpers::WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = m_hInstance;
    wcex.hIcon = LoadIcon(m_hInstance, reinterpret_cast<LPCTSTR>(IDI_TUTORIAL1));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = reinterpret_cast<HBRUSH>((COLOR_WINDOW + 1));
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = title;
    wcex.hIconSm = LoadIcon(wcex.hInstance, reinterpret_cast<LPCTSTR>(IDI_TUTORIAL1));
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    RECT rc = { 0, 0, static_cast<LONG>(m_pParameters->width), static_cast<LONG>(m_pParameters->height) };

    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    m_hWnd = CreateWindow(
        title,
        title,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rc.right - rc.left,
        rc.bottom - rc.top,
        nullptr,
        nullptr,
        m_hInstance,
        nullptr
    );

    if (!m_hWnd)
        return E_FAIL;

    ShowWindow(m_hWnd, m_cmdShow);
    UpdateWindow(m_hWnd);

    return hr;
}

Window& Window::Get()
{
    if (m_pInstance == nullptr)
        m_pInstance = std::unique_ptr<Window>(new Window());

    return *m_pInstance;
}
