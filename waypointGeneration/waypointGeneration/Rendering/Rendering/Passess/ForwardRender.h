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
	
	bool GetMousePicking(DirectX::XMFLOAT3 & worldPos);

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

	ID3D11Buffer * m_mouseBuffer = nullptr;
	MouseBuffer m_mouseValues;

	Shader m_forwardShaders;
	Shader m_depthPrePassShaders;
	Shader m_pickingPassShaders;

	/* PICKING VARIABLES */

	struct Texture_Render_Target
	{
		ID3D11Texture2D* Texture2D = nullptr;
		ID3D11RenderTargetView* RTV = nullptr;
		ID3D11ShaderResourceView* SRV = nullptr;

		void Release()
		{
			if (Texture2D)
				Texture2D->Release();
			Texture2D = nullptr;
			if (RTV)
				RTV->Release();
			RTV = nullptr;
			if (SRV)
				SRV->Release();
			SRV = nullptr;
		}

	}m_worldTexture;

	struct UAV
	{
		ID3D11UnorderedAccessView*	PickingUAV = nullptr;
		ID3D11Texture2D*			PickingTexBuffer = nullptr;
		ID3D11Texture2D*			PickingTexBufferCPU = nullptr;

		void Release()
		{
			if (PickingUAV)
				PickingUAV->Release();
			PickingUAV = nullptr;
			if (PickingTexBuffer)
				PickingTexBuffer->Release();
			PickingTexBuffer = nullptr;
			if (PickingTexBufferCPU)
				PickingTexBufferCPU->Release();
			PickingTexBufferCPU = nullptr;
		}

	} m_UAV;
	DirectX::XMFLOAT3 m_mouseWorldPos = DirectX::XMFLOAT3(0,0,0);
	bool m_isValidWorldPos = false;

private:
	void _createConstantBuffers();
	void _createShaders();
	void _createBlendState();
	void _createUAV();
	void _CreateTextureRenderTarget();

	void _prePass();
	void _forwardPass();
	void _pickingPass();
	void _noDepth();

};