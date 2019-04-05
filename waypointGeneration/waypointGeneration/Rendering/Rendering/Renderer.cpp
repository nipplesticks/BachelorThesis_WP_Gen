#include "waypointGenerationPCH.h"
#include "Renderer.h"

Renderer::Renderer()
{

}

Renderer::~Renderer()
{
}

Renderer * Renderer::GetInstance()
{
	static Renderer r;
	return &r;
}

void Renderer::Init()
{
	Window * wnd = Window::GetInstance();

	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif //DEBUG

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	D3D_FEATURE_LEVEL fl_in[] = { D3D_FEATURE_LEVEL_11_1 };
	D3D_FEATURE_LEVEL fl_out = D3D_FEATURE_LEVEL_11_0;

	// fill the swap chain description struct
	scd.BufferCount = 4;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = wnd->GetHwnd();						// the window to be used
	scd.SampleDesc.Count = m_sampleCount;                   // how many multisamples
	scd.Windowed = !wnd->IsFullscreen();					// windowed/full-screen mode

	ID3D11DeviceContext* pDevResult = nullptr;// create a device, device context and swap chain using the information in the scd struct
	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		createDeviceFlags,//D3D11_CREATE_DEVICE_DEBUG,
		fl_in,
		ARRAYSIZE(fl_in),
		D3D11_SDK_VERSION,
		&scd,
		&m_swapChain,
		&m_device,
		&fl_out,
		&pDevResult);

	if (SUCCEEDED(hr))
	{
		if (SUCCEEDED(pDevResult->QueryInterface(__uuidof(ID3D11DeviceContext1), (void**)&m_deviceContext)))
		{
			m_deviceContext->Release();
		}

		// get the address of the back buffer
		ID3D11Texture2D* pBackBuffer = nullptr;
		//m_swapChain->ResizeBuffers(0, width, hight, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_DISPLAY_ONLY);

		// use the back buffer address to create the render target
		m_deviceContext->OMSetRenderTargets(NULL, NULL, NULL);
		m_deviceContext->Flush();

		m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		pBackBuffer->Release();

		POINT wndRes = wnd->GetResolutionSize();

		m_swapChain->ResizeBuffers(0, wndRes.x, wndRes.y, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
		m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		if (SUCCEEDED(m_device->CreateRenderTargetView(pBackBuffer, NULL, &m_backBufferRTV)))
		{
			//we are creating the standard depth buffer here.
			_createDepthStencil(wndRes.x, wndRes.y);
			_initViewPort(wndRes.x, wndRes.y);
			pBackBuffer->Release();
		}

		D3D11_SAMPLER_DESC ssDesc = {};
		ssDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		ssDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		ssDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		ssDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		ssDesc.MaxAnisotropy = 0;
		ssDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		ssDesc.MaxLOD = D3D11_FLOAT32_MAX;
		ssDesc.MinLOD = -D3D11_FLOAT32_MAX;
		
		for (unsigned int i = 0; i < 4; i++)
			ssDesc.BorderColor[i] = 1.0f;

		m_device->CreateSamplerState(&ssDesc, &m_samplerState);

		_createDefaultTexture();

		m_forwardRenderer.Init();

	}
}

void Renderer::Clear()
{
	IRender::Clear();
	const float c[4] = { .15f,.15f,.15f,1.0f };

	m_deviceContext->ClearRenderTargetView(m_backBufferRTV, c);
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Renderer::Flush()
{
	m_forwardRenderer.Draw();
}

void Renderer::Present()
{
	m_swapChain->Present(0, 0);
}

void Renderer::Release()
{
	m_swapChain->SetFullscreenState(false, NULL);

	if (m_swapChain)
		m_swapChain->Release();
	m_swapChain = nullptr;
	if (m_backBufferRTV)
		m_backBufferRTV->Release();
	m_backBufferRTV = nullptr;
	if (m_depthStencilView)
		m_depthStencilView->Release();
	m_depthStencilView = nullptr;
	if (m_depthBufferTex)
		m_depthBufferTex->Release();
	m_depthBufferTex = nullptr;
	if (m_samplerState)
		m_samplerState->Release();
	m_samplerState = nullptr;
	if (m_depthStencilState)
		m_depthStencilState->Release();
	m_depthStencilState = nullptr;

	if (m_depthStencilState)
		m_depthStencilState->Release();
	m_depthStencilState = nullptr;

	if (m_defaultTex2D)
		m_defaultTex2D->Release();
	m_defaultTex2D = nullptr;

	if (m_defaultTexture)
		m_defaultTexture->Release();
	m_defaultTexture = nullptr;

	m_forwardRenderer.Release();

	m_deviceContext->Release();

#ifdef _DEBUG
	if (m_device->Release() > 0)
	{
		ID3D11Debug* dbg_device = nullptr;
		if (SUCCEEDED(m_device->QueryInterface(__uuidof(ID3D11Debug), (void**)&dbg_device)))
		{
			if (SUCCEEDED(dbg_device->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL)))
			{

			}
			dbg_device->Release();
		}
	}
#endif
}

ID3D11Device * Renderer::GetDevice()
{
	return m_device;
}

ID3D11DeviceContext * Renderer::GetDeviceContext()
{
	return m_deviceContext;
}

IDXGISwapChain * Renderer::GetSwapChain()
{
	return m_swapChain;
}

ID3D11RenderTargetView * Renderer::GetRTV()
{
	return m_backBufferRTV;
}

ID3D11DepthStencilView * Renderer::GetDSV()
{
	return m_depthStencilView;
}

ID3D11Texture2D * Renderer::GetDepthBufferTex()
{
	return m_depthBufferTex;
}

ID3D11SamplerState * Renderer::GetSamplerState()
{
	return m_samplerState;
}

ID3D11DepthStencilState * Renderer::GetDepthStencilState()
{
	return m_depthStencilState;
}

const D3D11_VIEWPORT & Renderer::GetViewport() const
{
	return m_viewport;
}

ID3D11Texture2D * Renderer::GetDefaultTex2D() const
{
	return m_defaultTex2D;
}

ID3D11ShaderResourceView * Renderer::GetDefaultTexture() const
{
	return m_defaultTexture;
}

void Renderer::_createDepthStencil(UINT width, UINT hight)
{
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = width;
	depthStencilDesc.Height = hight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = m_sampleCount;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	D3D11_DEPTH_STENCIL_DESC dpd{};
	dpd.DepthEnable = TRUE;
	dpd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dpd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dpd.StencilEnable = TRUE;

	dpd.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dpd.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	dpd.FrontFace.StencilFunc = D3D11_COMPARISON_LESS_EQUAL;
	dpd.BackFace.StencilFunc = D3D11_COMPARISON_LESS_EQUAL;

	dpd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dpd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	dpd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dpd.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	dpd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dpd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	//Create the Depth/Stencil View

	HRESULT hr;

	if (SUCCEEDED(hr = m_device->CreateDepthStencilState(&dpd, &m_depthStencilState)))
	{
		if (SUCCEEDED(hr = m_device->CreateTexture2D(&depthStencilDesc, NULL, &m_depthBufferTex)))
		{
			if (SUCCEEDED(hr = m_device->CreateDepthStencilView(m_depthBufferTex, NULL, &m_depthStencilView)))
			{
				
			}
		}
	}
}

void Renderer::_initViewPort(UINT width, UINT hight)
{
	m_viewport.Width = static_cast<float>(width);
	m_viewport.Height = static_cast<float>(hight);
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
}

void Renderer::_createDefaultTexture()
{
	D3D11_TEXTURE2D_DESC desc = {};
	desc.ArraySize = 1;
	desc.Height = 1;
	desc.Width = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags = 0;

	DirectX::XMFLOAT4 c(1.0f, 1.0f, 1.0f, 1.0f);
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &c;
	data.SysMemPitch = sizeof(DirectX::XMFLOAT4);

	if (SUCCEEDED(m_device->CreateTexture2D(&desc, &data, &m_defaultTex2D)))
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = desc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		HRESULT hr = m_device->CreateShaderResourceView(m_defaultTex2D, &srvDesc, &m_defaultTexture);
	}

}
