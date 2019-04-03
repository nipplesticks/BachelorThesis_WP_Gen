#pragma once

class Renderer
{
private:
	Renderer();
	~Renderer();

public:
	static Renderer * GetInstance();

	void Init();

	void Clear();
	void Flush();
	void Present();

	void Release();

public:
	ID3D11Device * GetDevice();
	ID3D11DeviceContext * GetDeviceContext();


private:
	ID3D11Device*			m_device = nullptr;
	ID3D11DeviceContext *	m_deviceContext = nullptr;

	IDXGISwapChain*				m_swapChain;
	ID3D11RenderTargetView*		m_backBufferRTV;
	ID3D11DepthStencilView*		m_depthStencilView;
	ID3D11Texture2D*			m_depthBufferTex;
	ID3D11SamplerState*			m_samplerState;
	ID3D11DepthStencilState*	m_depthStencilState;
	D3D11_VIEWPORT				m_viewport;

	UINT m_sampleCount = 1;

private:
	void _createDepthStencil(UINT width = 0, UINT hight = 0);
	void _initViewPort(UINT width = 0, UINT hight = 0);

};