#include "waypointGenerationPCH.h"
#include "Shader.h"
#include "../Renderer.h"
#include <comdef.h>

Shader::Shader()
{
}

Shader::~Shader()
{
}

ID3D11VertexShader * Shader::GetVertexShader()
{
	return m_vertexShader;
}

ID3D11DomainShader * Shader::GetDomainShader()
{
	return m_domainShader;
}

ID3D11HullShader * Shader::GetHullShader()
{
	return m_hullShader;
}

ID3D11GeometryShader * Shader::GetGeomteryShader()
{
	return m_geometryShader;
}

ID3D11PixelShader * Shader::GetPixelShader()
{
	return m_pixelShader;
}

ID3D11ComputeShader * Shader::GetComputeShader()
{
	return m_computeShader;
}

ID3D11InputLayout * Shader::GetInputLayout()
{
	return m_inputLayout;
}

void Shader::LoadVertexShader(const std::wstring & path, D3D11_INPUT_ELEMENT_DESC desc[], UINT elements)
{
	Renderer * r = Renderer::GetInstance();

	HRESULT shaderError;
	ID3DBlob* pVS = nullptr;
	ID3DBlob * errorBlob = nullptr;
	shaderError = D3DCompileFromFile(
		path.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"vs_5_0",
		0,
		0,
		&pVS,
		&errorBlob
	);

	if (FAILED(shaderError))
	{
		_com_error err(shaderError);
		OutputDebugStringA((char*)errorBlob->GetBufferPointer());

		std::cout << ((char*)errorBlob->GetBufferPointer());
		errorBlob->Release();

		if (pVS)
		{
			pVS->Release();
		}

		return;
	}

	r->GetDevice()->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &m_vertexShader);
	r->GetDevice()->CreateInputLayout(desc, elements, pVS->GetBufferPointer(), pVS->GetBufferSize(), &m_inputLayout);
}

void Shader::LoadPixelShader(const std::wstring & path)
{
	Renderer * r = Renderer::GetInstance();

	
	HRESULT shaderError;
	ID3DBlob* pVS = nullptr;
	ID3DBlob * errorBlob = nullptr;
	shaderError = D3DCompileFromFile(
		path.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		0,
		0,
		&pVS,
		&errorBlob
	);

	if (FAILED(shaderError))
	{
		_com_error err(shaderError);
		OutputDebugStringA((char*)errorBlob->GetBufferPointer());

		std::cout << ((char*)errorBlob->GetBufferPointer());
		errorBlob->Release();

		if (pVS)
		{
			pVS->Release();
		}

		return;
	}

	r->GetDevice()->CreatePixelShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &m_pixelShader);
}

void Shader::SetShaders(ID3D11DeviceContext * deviceContext)
{
	deviceContext->IASetInputLayout(m_inputLayout);
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	/*
		TODO :: The Rest
	*/
	deviceContext->PSSetShader(m_pixelShader, nullptr, 0);
}

void Shader::Release()
{
	if (m_vertexShader)
		m_vertexShader->Release();
	m_vertexShader = nullptr;

	if (m_domainShader)
		m_domainShader->Release();
	m_domainShader = nullptr;

	if (m_hullShader)
		m_hullShader->Release();
	m_hullShader = nullptr;

	if (m_geometryShader)
		m_geometryShader->Release();
	m_geometryShader = nullptr;

	if (m_pixelShader)
		m_pixelShader->Release();
	m_pixelShader = nullptr;

	if (m_computeShader)
		m_computeShader->Release();
	m_computeShader = nullptr;

	if (m_inputLayout)
		m_inputLayout->Release();
	m_inputLayout = nullptr;

}
