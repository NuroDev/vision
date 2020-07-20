#include "Graphics.hpp"

std::unique_ptr<Graphics> Graphics::m_pInstance = std::unique_ptr<Graphics>();

Graphics& Graphics::Get()
{
    if (m_pInstance == nullptr)
        m_pInstance = std::unique_ptr<Graphics>(new Graphics());

    return *m_pInstance;
}

HRESULT Graphics::Init()
{
    HRESULT hr = S_OK;

	// Initialize the parameters
	if (m_pParams == nullptr)
		m_pParams = new GraphicsParameters();

    // Initialize devices
    hr = InitDevice(Window::Get().GetWindow());
    if (FAILED(hr))
        return E_FAIL;

    // Create the constant buffer
    D3D11_BUFFER_DESC bd = {};
    ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBufferData);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = m_pDevice->CreateBuffer(&bd, nullptr, &m_pConstantBuffer);
    if (FAILED(hr))
        return hr;

    // Set up wireframe rendering
    D3D11_RASTERIZER_DESC wfdesc = {};
    ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
    wfdesc.FillMode = D3D11_FILL_WIREFRAME;
    wfdesc.CullMode = D3D11_CULL_NONE;
    hr = m_pDevice->CreateRasterizerState(&wfdesc, &m_pWireFrame);
    if (FAILED(hr))
        return hr;

    // Set up solid frame rendering
    ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
    wfdesc.FillMode = D3D11_FILL_SOLID;
    wfdesc.CullMode = D3D11_CULL_BACK;
    hr = m_pDevice->CreateRasterizerState(&wfdesc, &m_pSolidObject);
    if (FAILED(hr))
        return hr;

    // Create the material constant buffer
    ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(MaterialBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = m_pDevice->CreateBuffer(&bd, nullptr, &m_pMaterialConstantBuffer);
    if (FAILED(hr))
        return hr;

    // Set up custom render target blend state
    D3D11_RENDER_TARGET_BLEND_DESC rtbd;
    ZeroMemory(&rtbd, sizeof(rtbd));
    rtbd.BlendEnable = true;
    rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR;
    rtbd.DestBlend = D3D11_BLEND_BLEND_FACTOR;
    rtbd.BlendOp = D3D11_BLEND_OP_ADD;
    rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
    rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
    rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

    D3D11_BLEND_DESC blendDesc;
    ZeroMemory(&blendDesc, sizeof(blendDesc));
    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.RenderTarget[0] = rtbd;
    hr = m_pDevice->CreateBlendState(&blendDesc, &m_pTransparency);
    if (FAILED(hr))
        return hr;

    return hr;
}

void Graphics::Draw()
{
    // Set the render target back to the back buffer
    m_pContext->ClearRenderTargetView(m_pRenderTargetView, m_backgroundColor);
    m_pContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
    m_pContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    // Constant buffer
    ConstantBuffer& constantBuffer = ConstantBuffer::Get();
    constantBuffer.SetColor(XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));
    const ConstantBufferData bufferData = constantBuffer.GetData();
    m_pContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &bufferData, 0, 0);

    // Set constant buffers
    m_pContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
    m_pContext->PSSetConstantBuffers(1, 1, &m_pMaterialConstantBuffer);

    // Set pixel shader sampler
    m_pContext->PSSetSamplers(0, 1, &m_pSamplerLinear);

    // Set default rasterizer state & blend state
    m_pContext->RSSetState(m_pSolidObject);
    m_pContext->OMSetBlendState(0, 0, 0xffffffff);
	
    // Toggle transparency
	if (m_pParams->doTransparency)
	{
        const float blendFactor[] = { m_pParams->doTransparencyFactor, m_pParams->doTransparencyFactor, m_pParams->doTransparencyFactor, 1.0f };
        m_pContext->OMSetBlendState(m_pTransparency, blendFactor, 0xffffffff);
	}

    // Toggle wireframe rendering
    if(m_pParams->doWireframe)
        m_pContext->RSSetState(m_pWireFrame);
}

void Graphics::Ui()
{
    if (ImGui::CollapsingHeader("Graphics"))
    {
        if (ImGui::TreeNode("General"))
        {
            ImGui::Checkbox("V-Sync", &m_pParams->doVsync);

            ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&m_backgroundColor));
        	
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Rendering"))
        {
            ImGui::Checkbox("Deferred", &m_pParams->doDeferred);
            ImGui::Checkbox("Post Processing", &m_pParams->doPostProcess);

            ImGui::Checkbox("Transparency", &m_pParams->doTransparency);
            if (m_pParams->doTransparency)
                ImGui::SliderFloat("", &m_pParams->doTransparencyFactor, 0.0f, 1.0f);

            ImGui::Checkbox("Wireframe", &m_pParams->doWireframe);

            ImGui::TreePop();
        }
    }
}

void Graphics::Release()
{
    SAFE_RELEASE(m_pConstantBuffer);
    SAFE_RELEASE(m_pDepthStencilBuffer);
    SAFE_RELEASE(m_pMaterialConstantBuffer);

    if (m_pContext)
    {
        m_pContext->ClearState();
        m_pContext->Release();
        m_pContext = nullptr;
    }
    if (m_pContextAlt)
    {
        m_pContextAlt->ClearState();
        m_pContextAlt->Release();
        m_pContextAlt = nullptr;
    }
    SAFE_RELEASE(m_pDevice);
    SAFE_RELEASE(m_pDeviceAlt);

    SAFE_RELEASE(m_pWireFrame);
    SAFE_RELEASE(m_pSolidObject);
    SAFE_RELEASE(m_pTransparency);

    SAFE_RELEASE(m_pSamplerLinear);

    SAFE_RELEASE(m_pSwapChain);
    SAFE_RELEASE(m_pSwapChainAlt);

    SAFE_RELEASE(m_pDepthStencilView);
    SAFE_RELEASE(m_pRenderTargetView);
}

HRESULT Graphics::InitDevice(HWND hWnd)
{
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect(hWnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;

#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] = {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        m_driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDevice(
            nullptr,
            m_driverType,
            nullptr,
            createDeviceFlags,
            featureLevels,
            numFeatureLevels,
            D3D11_SDK_VERSION,
            &m_pDevice,
            &m_featureLevel,
            &m_pContext);

        // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
        if (hr == E_INVALIDARG)
            hr = D3D11CreateDevice(
                nullptr,
                m_driverType,
                nullptr,
                createDeviceFlags,
                &featureLevels[1],
                numFeatureLevels - 1,
                D3D11_SDK_VERSION,
                &m_pDevice,
                &m_featureLevel,
                &m_pContext);

        if (SUCCEEDED(hr))
            break;
    }
    if (FAILED(hr))
        return hr;

    // Obtain DXGI factory from device (since we used nullptr for pAdapter above)
    IDXGIFactory1 *dxgiFactory = nullptr;
    {
        IDXGIDevice *dxgiDevice = nullptr;
        hr = m_pDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void **>(&dxgiDevice));
        if (SUCCEEDED(hr))
        {
            IDXGIAdapter *adapter = nullptr;
            hr = dxgiDevice->GetAdapter(&adapter);
            if (SUCCEEDED(hr))
            {
                hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void **>(&dxgiFactory));
                adapter->Release();
            }
            dxgiDevice->Release();
        }
    }
    if (FAILED(hr))
        return hr;

    // Create swap chain
    IDXGIFactory2 *dxgiFactory2 = nullptr;
    hr = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void **>(&dxgiFactory2));
    if (dxgiFactory2)
    {
        // DirectX 11.1 or later
        hr = m_pDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void **>(&m_pDeviceAlt));
        if (SUCCEEDED(hr))
            static_cast<void>(m_pContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void **>(&m_pContextAlt)));

        DXGI_SWAP_CHAIN_DESC1 sd = {};
        ZeroMemory(&sd, sizeof(DXGI_SWAP_CHAIN_DESC1));
        sd.Width = width;
        sd.Height = height;
        sd.Format = DXGI_FORMAT_R16G16B16A16_FLOAT; //  DXGI_FORMAT_R16G16B16A16_FLOAT; ////DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 1;

        hr = dxgiFactory2->CreateSwapChainForHwnd(m_pDevice, hWnd, &sd, nullptr, nullptr, &m_pSwapChainAlt);
        if (SUCCEEDED(hr))
            hr = m_pSwapChainAlt->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void **>(&m_pSwapChain));

        dxgiFactory2->Release();
    }
    else
    {
        // DirectX 11.0 systems
        DXGI_SWAP_CHAIN_DESC sd = {};
        ZeroMemory(&sd, sizeof(DXGI_SWAP_CHAIN_DESC));
        sd.BufferCount = 1;
        sd.BufferDesc.Width = width;
        sd.BufferDesc.Height = height;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hWnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;

        hr = dxgiFactory->CreateSwapChain(m_pDevice, &sd, &m_pSwapChain);
    }

    // Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
    dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
    dxgiFactory->Release();
    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D *pBackBuffer = nullptr;
    hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&pBackBuffer));
    if (FAILED(hr))
        return hr;

    hr = m_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pRenderTargetView);
    pBackBuffer->Release();
    if (FAILED(hr))
        return hr;

    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC descDepth = {};
    ZeroMemory(&descDepth, sizeof(D3D11_TEXTURE2D_DESC));
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 2;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = m_pDevice->CreateTexture2D(&descDepth, nullptr, &m_pDepthStencilBuffer);
    if (FAILED(hr))
        return hr;

    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    ZeroMemory(&descDSV, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &descDSV, &m_pDepthStencilView);
    if (FAILED(hr))
        return hr;

    m_pContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

    // Setup the viewport
    m_viewport.Width = static_cast<FLOAT>(width);
    m_viewport.Height = static_cast<FLOAT>(height);
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;
    m_viewport.TopLeftX = 0;
    m_viewport.TopLeftY = 0;
    m_pContext->RSSetViewports(1, &m_viewport);

    // Create sampler description
    D3D11_SAMPLER_DESC sampDesc = {};
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = m_pDevice->CreateSamplerState(&sampDesc, &m_pSamplerLinear);
    if (FAILED(hr))
        return hr;

    return hr;
}
