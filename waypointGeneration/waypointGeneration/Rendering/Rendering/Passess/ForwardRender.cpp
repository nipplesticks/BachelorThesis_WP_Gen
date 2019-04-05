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
}

void ForwardRender::Update()
{

}

void ForwardRender::Draw()
{
	Renderer * r = Renderer::GetInstance();
	ID3D11DeviceContext * dc = r->GetDeviceContext();
	dc->RSSetViewports(1, &m_viewport);
	dc->OMSetDepthStencilState(m_depthStencilState, NULL);
	dc->PSSetSamplers(1, 1, &m_samplerState);
	m_forwardShaders.SetShaders(dc);
	dc->OMSetRenderTargets(1, &m_backBufferRTV, m_depthStencilView);
	
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

	
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	size_t drawQueueSize = p_drawQueue.size();
	for (size_t i = 0; i < drawQueueSize; i++)
	{
		m_objectValues.color = p_drawQueue[i]->GetColor();
		m_objectValues.worldMatrix = p_drawQueue[i]->GetWorldMatrix();

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

void ForwardRender::Release()
{
	if (m_cameraBuffer)
		m_cameraBuffer->Release();
	m_cameraBuffer = nullptr;

	if (m_objectBuffer)
		m_objectBuffer->Release();
	m_objectBuffer = nullptr;

	m_forwardShaders.Release();

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
}
