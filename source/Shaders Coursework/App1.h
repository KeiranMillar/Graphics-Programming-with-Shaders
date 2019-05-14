// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "../DXFramework/DXF.h"
#include <math.h>
#include "ColourShader.h"
#include "LightShader.h"
#include "TextureShader.h"
#include "DepthShader.h"
#include "ShadowShader.h"
#include "HorizontalBlurShader.h"
#include "VerticalBlurShader.h"
#include "TessellationMeshQuad.h"
#include "TessellationShader.h"
#include "VertexShader.h"

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in);
	void variableSampleSizes(float scale);
	bool frame();

	int screenW;
	int screenH;

protected:
	bool render();
	void gui();
	void DepthMapFunction();
	void RenderToTexture();
	void RenderScene();
	void downSampleFunction();
	void horizontalBlurFunction();
	void verticalBlurFunction();
	void upSampleFunction();
	void TesselationRender();

private:
	//Shaders
	TextureShader* textureShader;
	ColourShader* colourShader;
	LightShader* lightShader;
	DepthShader* depthShader;
	ShadowShader* shadowShader;
	HorizontalBlurShader* horizontalBlurShader;
	VerticalBlurShader* verticalBlurShader;
	TessellationShader* tessShader;
	VertexShader* vertShader;
	

	//Render Textures
	RenderTexture* renderTexture;
	RenderTexture* depthMap;
	RenderTexture* downSample;
	RenderTexture* horizontalBlur;
	RenderTexture* verticalBlur;
	RenderTexture* upSample;
	
	//Meshes
	PlaneMesh* plane;
	CubeMesh* cube;
	TriangleMesh* mesh;
	SphereMesh* sphere;
	OrthoMesh* ortho;
	OrthoMesh* orthoMesh;
	TessellationMeshQuad* tesMesh;

	//Lights
	Light* m_light;
	Light* m_light2;

	bool isWireframe;
	bool scene = false;
	float scale = 2;
	float angle = 0;
	float radAngle = 0;
	float blurIntensity = 1;

	const int SHADOWMAP_HEIGHT = 1024;
	const int SHADOWMAP_WIDTH = 1024;

};

#endif