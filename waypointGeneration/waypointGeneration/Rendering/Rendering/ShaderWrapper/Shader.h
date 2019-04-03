#pragma once

class Shader
{
public:
	Shader();
	~Shader();

	ID3D11VertexShader * GetVertexShader();
	ID3D11DomainShader * GetDomainShader();
	ID3D11HullShader * GetHullShader();
	ID3D11GeometryShader * GetGeomteryShader();
	ID3D11PixelShader * GetPixelShader();
	ID3D11ComputeShader	* GetComputeShader();
	ID3D11InputLayout * GetInputLayout();

	void LoadVertexShader(const std::wstring & path, D3D11_INPUT_ELEMENT_DESC desc[], UINT elements);
	void LoadPixelShader(const std::wstring & path);

	void SetShaders(ID3D11DeviceContext * deviceContext);

	void Release();

private:
	ID3D11VertexShader		* m_vertexShader = nullptr;
	ID3D11DomainShader		* m_domainShader = nullptr;
	ID3D11HullShader		* m_hullShader = nullptr;
	ID3D11GeometryShader	* m_geometryShader = nullptr;
	ID3D11PixelShader		* m_pixelShader = nullptr;
	ID3D11ComputeShader		* m_computeShader = nullptr;
	ID3D11InputLayout		* m_inputLayout = nullptr;

};