#include "Application.hpp"

HRESULT Application::Init(HINSTANCE hInstance, const int nCmdShow)
{
    HRESULT hr = S_OK;

    // ================================================================
	// Initialize config
	// ================================================================
    if (m_pConfig == nullptr)
        return E_FAIL;
    hr = m_pConfig->Init();
    if (FAILED(hr))
        return hr;
	
    // ================================================================
    // Initialize the application window
    // ================================================================
    Window& window = Window::Get();
    window.SetParams(m_pConfig->GetAppParams());
    hr = window.Init(hInstance, nCmdShow);
    if (FAILED(hr))
        return hr;

    // ================================================================
    // Initialize core graphics
    // ================================================================
    Graphics& graphics = Graphics::Get();
    graphics.SetParams(m_pConfig->GetGraphicsParams());
    hr = graphics.Init();
    if (FAILED(hr))
        return hr;

    // ================================================================
	// Load config objects
	// ================================================================
    hr = m_pConfig->LoadScene();
    if (FAILED(hr))
        return hr;
    for (WorldObject* pObject : m_pConfig->GetWorldObjects())
        m_pWorldObjects.push_back(pObject);
    for (WorldLight* pLight : m_pConfig->GetLights())
        m_pLights.push_back(pLight);

	// ================================================================
	// Initialize a new terrain object
	// ================================================================
    Terrain* pTerrain = new Terrain("Terrain", rand() % 1000, TerrainType::Procedural);
	pTerrain->SetTexture({
        "Assets/Textures/terrain_blend.dds",
    	"Assets/Textures/terrain_darkdirt.dds",
    	"Assets/Textures/terrain_grass.dds",
    	"Assets/Textures/terrain_lightdirt.dds",
    	"Assets/Textures/terrain_snow.dds",
    	"Assets/Textures/terrain_stone.dds"
    });
    hr = pTerrain->LoadShader(L"Assets/Shaders/Terrain.fx");
    if (FAILED(hr))
        return hr;
	hr = pTerrain->Init();
	if (FAILED(hr))
	   return hr;
    m_pWorldObjects.push_back(pTerrain);

    // ================================================================
    // Initialize post-processing renderer
    // ================================================================
    m_pPostProcessing = std::make_unique<PostProcessing>(m_pConfig->GetAppParams()->width, m_pConfig->GetAppParams()->height);
    hr = m_pPostProcessing->LoadShader(L"Assets/Shaders/PostProcessing.fx");
    if (FAILED(hr))
        return hr;
    hr = m_pPostProcessing->Init();
    if (FAILED(hr))
        return hr;

    // ================================================================
    // Initialize deferred renderer
    // ================================================================
    m_pDeferredRenderer = std::make_unique<DeferredRenderer>(DeferredRenderer(m_pConfig->GetAppParams()->width, m_pConfig->GetAppParams()->height));
    hr = m_pDeferredRenderer->Init();
    if (FAILED(hr))
        return hr;
	
    // ================================================================
    // Initialize ImGui
    // ================================================================
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(window.GetWindow());
    ImGui_ImplDX11_Init(graphics.GetDevice(), graphics.GetContext());
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = "Assets/ui.ini";

    return hr;
}

void Application::Draw()
{
    // Get the graphics instance
    Graphics& graphics = Graphics::Get();

    // Get the constant buffer
    ConstantBuffer& constantBuffer = ConstantBuffer::Get();
	
    // Update constant buffer view & projection matrix
    constantBuffer.SetView(XMMatrixTranspose(m_pConfig->GetCamera(0)->GetView()));
    constantBuffer.SetProjection(XMMatrixTranspose(m_pConfig->GetCamera(0)->GetProjection()));
	
	// Create new ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Draw core graphics
    graphics.Draw();

    // Draw objects
    const std::function<void()> render = [&] {    	
        for (WorldLight* pLight : m_pLights)
            pLight->Draw();
        for (WorldObject* pObject : m_pWorldObjects)
            pObject->Draw();	
    };

    if (graphics.GetParams()->doDeferred)
        m_pDeferredRenderer->Draw(render);

    // Draw content/objects
    graphics.GetParams()->doPostProcess ? m_pPostProcessing->Draw(render) : render();

    // Draw ImGui components
    Ui();
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Present our back buffer to our front buffer
    graphics.GetSwapChain()->Present(graphics.GetParams()->doVsync ? 1 : 0, 0);
}

void Application::Update(float t)
{
    // Get the constant buffer
    ConstantBuffer& constantBuffer = ConstantBuffer::Get();

    // Get the camera parameters
    const CameraParameters* pCameraParameters = m_pConfig->GetCamera(0)->GetParams();

    // TODO: Add support for multiple cameras
    // Update the constant buffer
    constantBuffer.SetWorld(XMMatrixTranslation(t * pCameraParameters->position.x, t * pCameraParameters->position.y, t * pCameraParameters->position.z));
    constantBuffer.SetView(m_pConfig->GetCamera(0)->GetView());
    constantBuffer.SetProjection(m_pConfig->GetCamera(0)->GetProjection());
	
    // Update camera(s)
	for (FirstPersonCamera* pCamera : m_pConfig->GetCameras())
		pCamera->Update(t);

    // Update light(s)
    for (WorldLight* pLight : m_pLights)
        pLight->Update(t);
	
    // Update world object(s)
    for (WorldObject *pObject : m_pWorldObjects)
        pObject->Update(t);

// Exit the application
#if DEBUG
    if (GetAsyncKeyState(VK_ESCAPE))
        exit(0);
#endif
}

void Application::Ui()
{
    ApplicationParameters* pParams = m_pConfig->GetAppParams();
	
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Show Inspector", nullptr, &pParams->showInspector))
                m_pConfig->SetAppParams(pParams);

            if (ImGui::MenuItem("Show Metrics", nullptr, &pParams->showMetrics))
                m_pConfig->SetAppParams(pParams);
        	
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::MenuItem("Exit"))
                exit(0);

            ImGui::EndMenu();
        }

        if (pParams->showMetrics)
        {
            const float fps = ImGui::GetIO().Framerate;
            ImGui::Text("\t%.2f fps", fps);

            const float updateTick = 1000.0f / fps;
            ImGui::Text("\t%.2f ms", updateTick);
        }

        ImGui::EndMainMenuBar();
    }

    if (pParams->showInspector)
    {
        ImGui::Begin("Inspector:", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

        // Get the graphics instance
        Graphics& graphics = Graphics::Get();
    	
		// Draw graphics Ui
        graphics.Ui();

    	// Draw deferred rendering Ui
        if (graphics.GetParams()->doDeferred)
            m_pDeferredRenderer->Ui();

        // Draw post-processing Ui
        if (graphics.GetParams()->doPostProcess)
            m_pPostProcessing->Ui();
    	
        // Draw camera(s) Ui
        for (FirstPersonCamera* pCamera : m_pConfig->GetCameras())
            pCamera->Ui();

        // Draw light(s) Ui
        for (WorldLight *pLight : m_pLights)
            pLight->Ui();

        // Draw world object(s) Ui
        for (WorldObject *pObject : m_pWorldObjects)
            pObject->Ui();

        ImGui::End();
    }
}

void Application::Release()
{
    Graphics::Get().Release();

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    for (WorldLight* pLight : m_pLights)
        SAFE_RELEASE(pLight);
    for (WorldObject* pObject : m_pWorldObjects)
        SAFE_RELEASE(pObject);
}
