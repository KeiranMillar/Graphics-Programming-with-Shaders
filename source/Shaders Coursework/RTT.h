// Light shader.h
// Basic single light shader setup
#ifndef _RTTShader_H_
#define _RTTShader_H_

#include "../DXFramework/BaseShader.h"
#include "../DXFramework/Light.h"

using namespace std;
using namespace DirectX;


class RTT : public BaseShader
{
private:
	struct RTTType
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT3 direction;
		float padding;
	};

public:

	RTT(ID3D11Device* device, HWND hwnd);
	~RTT();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, Light* light);
	void render(ID3D11DeviceContext* deviceContext, int vertexCount);

private:
	void initShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* lightBuffer;
};

#endif