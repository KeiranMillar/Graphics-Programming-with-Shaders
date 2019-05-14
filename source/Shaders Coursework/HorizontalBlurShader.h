// Horizontal blur shader handler
// Loads horizontal blur shaders (vs and ps)
// Passes screen width to shaders, for sample coordinate calculation

#ifndef _HORIZONTALBLURSHADER_H_
#define _HORIZONTALBLURSHADER_H_

#include "../DXFramework/BaseShader.h"


using namespace std;
using namespace DirectX;


class HorizontalBlurShader : public BaseShader
{
private:
	struct ScreenSizeBufferType
	{
		float screenWidth;
		XMFLOAT3 padding;
	};

	struct BlurVariableBufferType
	{
		float angle;
		float intensity;
		XMFLOAT2 padding;
	};

public:

	HorizontalBlurShader(ID3D11Device* device, HWND hwnd);
	~HorizontalBlurShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, float width, float angle, float intensity);
	void render(ID3D11DeviceContext* deviceContext, int vertexCount);

private:
	void initShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* screenSizeBuffer;
	ID3D11Buffer* blurVariableBuffer;
};

#endif