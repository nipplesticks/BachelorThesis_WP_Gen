#pragma once
#include "IRender.h"
#include "../ShaderWrapper/Shader.h"


class ForwardRender : public IRender
{
public:
	ForwardRender();
	~ForwardRender();

	void Init() override;
	void Update() override;
	void Draw() override;
	void Release() override;
	
private:
	IDXGISwapChain*				m_swapChain = nullptr;
	ID3D11RenderTargetView*		m_backBufferRTV = nullptr;
	ID3D11DepthStencilView*		m_depthStencilView = nullptr;
	ID3D11Texture2D*			m_depthBufferTex = nullptr;
	ID3D11SamplerState*			m_samplerState = nullptr;
	ID3D11DepthStencilState*	m_depthStencilState = nullptr;
	D3D11_VIEWPORT				m_viewport;

	ID3D11BlendState *			m_blendState = nullptr;

	ID3D11Buffer * m_cameraBuffer = nullptr;
	CameraBuffer m_cameraValues;

	ID3D11Buffer * m_objectBuffer = nullptr;
	ObjectBuffer m_objectValues;

	Shader m_forwardShaders;

private:
	void _createConstantBuffers();
	void _createShaders();
	void _createBlendState();

};