#include "waypointGenerationPCH.h"
#include "ForwardRender.h"
#include "../Renderer.h"
#include "../../Camera/Camera.h"


ForwardRender::ForwardRender() : IRender()
{
}

ForwardRender::~ForwardRender()
{
}

void ForwardRender::Init()
{
	Renderer * r = Renderer::GetInstance();
	ID3D11DeviceContext * dc = r->GetDeviceContext();

	m_swapChain = r->GetSwapChain();
	m_backBufferRTV = r->GetRTV();
	m_depthStencilView = r->GetDSV();
	m_depthBufferTex = r->GetDepthBufferTex();
	m_samplerState = r->GetSamplerState();
	m_depthStencilState = r->GetDepthStencilState();
	m_viewport = r->GetViewport();

	_createConstantBuffers();
	_createShaders();
	_createBlendState();
	_createUAV();
	_CreateTextureRenderTarget();
}

void ForwardRender::Update()
{
	Renderer * r = Renderer::GetInstance();
	ID3D11DeviceContext * dc = r->GetDeviceContext();

	DirectX::XMFLOAT4 camPos = Camera::GetActiveCamera()->GetPosition();
	m_cameraValues.cameraPosition.x = camPos.x;
	m_cameraValues.cameraPosition.y = camPos.y;
	m_cameraValues.cameraPosition.z = camPos.z;
	m_cameraValues.cameraPosition.w = 1.0f;
	m_cameraValues.viewProjection = Camera::GetActiveCamera()->GetViewProjectionMatrix();

	D3D11_MAPPED_SUBRESOURCE dataPtr;
	dc->Map(m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
	memcpy(dataPtr.pData, &m_cameraValues, sizeof(m_cameraValues));
	dc->Unmap(m_cameraBuffer, 0);
	dc->VSSetConstantBuffers(1, 1, &m_cameraBuffer);
	dc->PSSetConstantBuffers(1, 1, &m_cameraBuffer);
}

void ForwardRender::Draw()
{
	Renderer * r = Renderer::GetInstance();
	ID3D11DeviceContext * dc = r->GetDeviceContext();
	dc->OMSetBlendState(nullptr, 0, 0xffffffff);
	dc->RSSetViewports(1, &m_viewport);
	dc->OMSetDepthStencilState(m_depthStencilState, NULL);
	dc->PSSetSamplers(1, 1, &m_samplerState);

	_prePass();
	_forwardPass();
	_pickingPass();
	_noDepth();
}

void ForwardRender::Release()
{
	if (m_cameraBuffer)
		m_cameraBuffer->Release();
	m_cameraBuffer = nullptr;

	if (m_objectBuffer)
		m_objectBuffer->Release();
	m_objectBuffer = nullptr;

	if (m_blendState)
		m_blendState->Release();
	m_blendState = nullptr;

	m_UAV.Release();
	m_worldTexture.Release();

	m_forwardShaders.Release();
}

bool ForwardRender::GetMousePicking(DirectX::XMFLOAT3 & worldPos)
{
	worldPos = m_mouseWorldPos;
	return m_isValidWorldPos;
}

void ForwardRender::_createConstantBuffers()
{
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(CameraBuffer);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	Renderer * r = Renderer::GetInstance();
	HRESULT hr = r->GetDevice()->CreateBuffer(&bufferDesc, nullptr, &m_cameraBuffer);

	bufferDesc.ByteWidth = sizeof(ObjectBuffer);

	hr = r->GetDevice()->CreateBuffer(&bufferDesc, nullptr, &m_objectBuffer);

	bufferDesc.ByteWidth = sizeof(MouseBuffer);

	hr = r->GetDevice()->CreateBuffer(&bufferDesc, nullptr, &m_mouseBuffer);
}

void ForwardRender::_createShaders()
{
	ID3D11DeviceContext * dc = Renderer::GetInstance()->GetDeviceContext();
	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD" , 0, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_forwardShaders.LoadVertexShader(L"Rendering\\Rendering\\Shaders\\ForwardVertex.hlsl", inputDesc, _countof(inputDesc));
	m_forwardShaders.LoadPixelShader(L"Rendering\\Rendering\\Shaders\\ForwardPixel.hlsl");

	m_depthPrePassShaders.LoadVertexShader(L"Rendering\\Rendering\\Shaders\\ForwardVertex.hlsl", inputDesc, _countof(inputDesc));
	m_depthPrePassShaders.LoadPixelShader(L"Rendering\\Rendering\\Shaders\\DepthPrePassPixel.hlsl");

	m_pickingPassShaders.LoadVertexShader(L"Rendering\\Rendering\\Shaders\\PickingPassVertex.hlsl", nullptr, 0);
	m_pickingPassShaders.LoadPixelShader(L"Rendering\\Rendering\\Shaders\\PickingPassPixel.hlsl");

}

void ForwardRender::_createBlendState()
{
	D3D11_BLEND_DESC omDesc = {};
	omDesc.RenderTarget[0].BlendEnable = true;
	omDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	omDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	omDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	omDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	omDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	omDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	omDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	Renderer::GetInstance()->GetDevice()->CreateBlendState(&omDesc, &m_blendState);
}

void ForwardRender::_createUAV()
{
	ID3D11Device * device = Renderer::GetInstance()->GetDevice();
	D3D11_TEXTURE2D_DESC TextureData = {};
	TextureData.ArraySize = 1;
	TextureData.Height = 1;
	TextureData.Width = 1;
	TextureData.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	TextureData.CPUAccessFlags = 0;
	TextureData.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	TextureData.MipLevels = 1;
	TextureData.MiscFlags = 0;
	TextureData.SampleDesc.Count = 1;
	TextureData.SampleDesc.Quality = 0;
	TextureData.Usage = D3D11_USAGE_DEFAULT;

	device->CreateTexture2D(&TextureData, nullptr, &m_UAV.PickingTexBuffer);

	TextureData.Usage = D3D11_USAGE_STAGING;
	TextureData.BindFlags = 0;
	TextureData.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

	device->CreateTexture2D(&TextureData, nullptr, &m_UAV.PickingTexBufferCPU);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = TextureData.Format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	device->CreateUnorderedAccessView(m_UAV.PickingTexBuffer, &uavDesc, &m_UAV.PickingUAV);
}

void ForwardRender::_CreateTextureRenderTarget()
{
	POINT wndSize = Window::GetInstance()->GetResolutionSize();

	ID3D11Device* device = Renderer::GetInstance()->GetDevice();

	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = (UINT)wndSize.x;
	texDesc.Height = (UINT)wndSize.y;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	device->CreateTexture2D(&texDesc, NULL, &m_worldTexture.Texture2D);

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	device->CreateRenderTargetView(m_worldTexture.Texture2D, &rtvDesc, &m_worldTexture.RTV);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = rtvDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(m_worldTexture.Texture2D, &srvDesc, &m_worldTexture.SRV);


}

void ForwardRender::_prePass()
{
	Renderer * r = Renderer::GetInstance();
	ID3D11DeviceContext * dc = r->GetDeviceContext();
	
	m_depthPrePassShaders.SetShaders(dc);
	dc->OMSetRenderTargets(1, &m_worldTexture.RTV, m_depthStencilView);

	float c1[4]{ -1.0f, -1.0f, -1.0f, 1.0f };

	dc->ClearRenderTargetView(m_worldTexture.RTV, c1);

	D3D11_MAPPED_SUBRESOURCE dataPtr;

	size_t drawQueueSize = p_drawQueue.size();
	for (size_t i = 0; i < drawQueueSize; i++)
	{
		if (p_drawQueue[i]->IsPickable())
		{
			dc->IASetPrimitiveTopology(p_drawQueue[i]->GetTopology());
			m_objectValues.color = p_drawQueue[i]->GetColor();
			m_objectValues.worldMatrix = p_drawQueue[i]->GetWorldMatrix();
			m_objectValues.uvOffset = p_drawQueue[i]->GetUVOffset();

			dc->Map(m_objectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
			memcpy(dataPtr.pData, &m_objectValues, sizeof(m_objectValues));
			dc->Unmap(m_objectBuffer, 0);

			dc->VSSetConstantBuffers(0, 1, &m_objectBuffer);

			ID3D11ShaderResourceView * tex = p_drawQueue[i]->GetTexture();

			dc->PSSetShaderResources(0, 1, &tex);

			ID3D11Buffer * vertexBuffer = p_drawQueue[i]->GetBuffer();
			UINT stride = sizeof(Vertex);
			UINT offset = 0;
			dc->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
			dc->Draw((UINT)p_drawQueue[i]->GetVertices()->size(), 0);
		}
	}

	/*dc->OMSetBlendState(m_blendState, 0, 0xffffffff);
	drawQueueSize = p_drawQueueTransparent.size();
	for (size_t i = 0; i < drawQueueSize; i++)
	{
		if (p_drawQueueTransparent[i]->IsPickable())
		{
			dc->IASetPrimitiveTopology(p_drawQueueTransparent[i]->GetTopology());
			m_objectValues.color = p_drawQueueTransparent[i]->GetColor();
			m_objectValues.worldMatrix = p_drawQueueTransparent[i]->GetWorldMatrix();

			dc->Map(m_objectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
			memcpy(dataPtr.pData, &m_objectValues, sizeof(m_objectValues));
			dc->Unmap(m_objectBuffer, 0);

			dc->VSSetConstantBuffers(0, 1, &m_objectBuffer);

			ID3D11ShaderResourceView * tex = p_drawQueueTransparent[i]->GetTexture();

			dc->PSSetShaderResources(0, 1, &tex);

			ID3D11Buffer * vertexBuffer = p_drawQueueTransparent[i]->GetBuffer();
			UINT stride = sizeof(Vertex);
			UINT offset = 0;
			dc->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
			dc->Draw((UINT)p_drawQueueTransparent[i]->GetVertices()->size(), 0);
		}
	}*/

}

void ForwardRender::_forwardPass()
{
	Renderer * r = Renderer::GetInstance();
	ID3D11DeviceContext * dc = r->GetDeviceContext();
	m_forwardShaders.SetShaders(dc);

	dc->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 0, 0);
	dc->OMSetRenderTargets(1, &m_backBufferRTV, m_depthStencilView);
	dc->OMSetBlendState(nullptr, 0, 0xffffffff);
	D3D11_MAPPED_SUBRESOURCE dataPtr;
	size_t drawQueueSize = p_drawQueue.size();
	for (size_t i = 0; i < drawQueueSize; i++)
	{
		dc->IASetPrimitiveTopology(p_drawQueue[i]->GetTopology());
		m_objectValues.color = p_drawQueue[i]->GetColor();
		m_objectValues.worldMatrix = p_drawQueue[i]->GetWorldMatrix();
		m_objectValues.uvOffset = p_drawQueue[i]->GetUVOffset();

		dc->Map(m_objectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
		memcpy(dataPtr.pData, &m_objectValues, sizeof(m_objectValues));
		dc->Unmap(m_objectBuffer, 0);

		dc->VSSetConstantBuffers(0, 1, &m_objectBuffer);

		ID3D11ShaderResourceView * tex = p_drawQueue[i]->GetTexture();

		dc->PSSetShaderResources(0, 1, &tex);

		ID3D11Buffer * vertexBuffer = p_drawQueue[i]->GetBuffer();
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		dc->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		dc->Draw((UINT)p_drawQueue[i]->GetVertices()->size(), 0);
	}

	dc->OMSetBlendState(m_blendState, 0, 0xffffffff);
	drawQueueSize = p_drawQueueTransparent.size();
	for (size_t i = 0; i < drawQueueSize; i++)
	{
		dc->IASetPrimitiveTopology(p_drawQueueTransparent[i]->GetTopology());
		m_objectValues.color = p_drawQueueTransparent[i]->GetColor();
		m_objectValues.worldMatrix = p_drawQueueTransparent[i]->GetWorldMatrix();
		m_objectValues.uvOffset = p_drawQueueTransparent[i]->GetUVOffset();

		dc->Map(m_objectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
		memcpy(dataPtr.pData, &m_objectValues, sizeof(m_objectValues));
		dc->Unmap(m_objectBuffer, 0);

		dc->VSSetConstantBuffers(0, 1, &m_objectBuffer);

		ID3D11ShaderResourceView * tex = p_drawQueueTransparent[i]->GetTexture();

		dc->PSSetShaderResources(0, 1, &tex);

		ID3D11Buffer * vertexBuffer = p_drawQueueTransparent[i]->GetBuffer();
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		dc->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		dc->Draw((UINT)p_drawQueueTransparent[i]->GetVertices()->size(), 0);
	}
}

void ForwardRender::_pickingPass()
{
	Renderer * r = Renderer::GetInstance();
	ID3D11DeviceContext * dc = r->GetDeviceContext();

	m_pickingPassShaders.SetShaders(dc);

	dc->PSSetShaderResources(0, 1, &m_worldTexture.SRV);

	dc->OMSetRenderTargetsAndUnorderedAccessViews(
		0,
		nullptr,
		nullptr,
		1, 1, &m_UAV.PickingUAV, 0
	);
	
	dc->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);

	D3D11_MAPPED_SUBRESOURCE dataPtr;
	POINT mp = Window::GetInstance()->GetMousePosition();
	m_mouseValues.MousePos.x = mp.x;
	m_mouseValues.MousePos.y = mp.y;

	dc->Map(m_mouseBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
	memcpy(dataPtr.pData, &m_mouseValues, sizeof(m_mouseValues));
	dc->Unmap(m_mouseBuffer, 0);

	dc->PSSetConstantBuffers(0, 1, &m_mouseBuffer);

	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	dc->Draw(4, 0);

	dc->CopyResource(m_UAV.PickingTexBufferCPU, m_UAV.PickingTexBuffer);
	D3D11_MAPPED_SUBRESOURCE mr;

	dc->Map(m_UAV.PickingTexBufferCPU, 0, D3D11_MAP_READ, 0, &mr);
	
	DirectX::XMFLOAT3 * data = (DirectX::XMFLOAT3*)mr.pData;
	m_mouseWorldPos = *data;
	m_isValidWorldPos = true;
	if (m_mouseWorldPos.x < 0.0)
		m_isValidWorldPos = false;

	dc->Unmap(m_UAV.PickingTexBufferCPU, 0);

	if (SUCCEEDED(dc->Map(m_UAV.PickingTexBufferCPU, 0, D3D11_MAP_WRITE, 0, &dataPtr)))
	{
		DirectX::XMFLOAT3 killer = { -1, -1, -1 };
		memcpy(dataPtr.pData, &killer, sizeof(DirectX::XMFLOAT3));
		dc->CopyResource(m_UAV.PickingTexBuffer, m_UAV.PickingTexBufferCPU);
		dc->Unmap(m_UAV.PickingTexBufferCPU, 0);
	}

}

void ForwardRender::_noDepth()
{
	Renderer * r = Renderer::GetInstance();
	ID3D11DeviceContext * dc = r->GetDeviceContext();
	m_forwardShaders.SetShaders(dc);

	//dc->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 0, 0);
	dc->OMSetRenderTargets(1, &m_backBufferRTV, nullptr);
	dc->OMSetBlendState(nullptr, 0, 0xffffffff);
	D3D11_MAPPED_SUBRESOURCE dataPtr;
	size_t drawQueueSize = p_drawQueueNoDepthBuffer.size();
	for (size_t i = 0; i < drawQueueSize; i++)
	{
		dc->IASetPrimitiveTopology(p_drawQueueNoDepthBuffer[i]->GetTopology());
		m_objectValues.color = p_drawQueueNoDepthBuffer[i]->GetColor();
		m_objectValues.worldMatrix = p_drawQueueNoDepthBuffer[i]->GetWorldMatrix();
		m_objectValues.uvOffset = p_drawQueueNoDepthBuffer[i]->GetUVOffset();

		dc->Map(m_objectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
		memcpy(dataPtr.pData, &m_objectValues, sizeof(m_objectValues));
		dc->Unmap(m_objectBuffer, 0);

		dc->VSSetConstantBuffers(0, 1, &m_objectBuffer);

		ID3D11ShaderResourceView * tex = p_drawQueueNoDepthBuffer[i]->GetTexture();

		dc->PSSetShaderResources(0, 1, &tex);

		ID3D11Buffer * vertexBuffer = p_drawQueueNoDepthBuffer[i]->GetBuffer();
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		dc->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		dc->Draw((UINT)p_drawQueueNoDepthBuffer[i]->GetVertices()->size(), 0);
	}
}
