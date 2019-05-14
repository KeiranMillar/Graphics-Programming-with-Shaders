// Light shader.h
// Basic single light shader setup
#ifndef _VERTEXSHADER_H_
#define _VERTEXSHADER_H_

#include "../DXFramework/BaseShader.h"
#include "../DXFramework/Light.h"

using namespace std;
using namespace DirectX;

class VertexShader :
	public BaseShader
{
private:
	struct LightBufferType
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT3 direction;
		float padding;
	};
public:

	VertexShader(ID3D11Device* device, HWND hwnd);
	~VertexShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, Light* light, float time);
	void render(ID3D11DeviceContext* deviceContext, int vertexCount);

private:
	void initShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* timeBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* lightBuffer;
};
#endif
