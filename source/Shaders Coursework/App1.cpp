// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1(){
	//BaseApplication::BaseApplication();
	mesh = nullptr;
	cube = nullptr;
	plane = nullptr;
	ortho = nullptr;
	orthoMesh = nullptr;
	tesMesh = nullptr;

	colourShader = nullptr;
	textureShader = nullptr;
	depthShader = nullptr;
	shadowShader = nullptr;
	lightShader = nullptr;
	horizontalBlurShader = nullptr;
	verticalBlurShader = nullptr;
	tessShader = nullptr;
	vertShader = nullptr;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in);
	
	//Textures
	textureMgr->loadTexture("supra", L"../res/Supra.png");
	textureMgr->loadTexture("brick1", L"../res/brick1.dds");
	textureMgr->loadTexture("height", L"../res/height.png");

	//Mesh's
	cube = new CubeMesh(renderer->getDevice(), renderer->getDeviceContext());
	plane = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	ortho = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), 200, 150, -400, 150);
	orthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight, 0, 0);
	tesMesh = new TessellationMeshQuad(renderer->getDevice(), renderer->getDeviceContext());

	//Shaders
	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	colourShader = new ColourShader(renderer->getDevice(), hwnd);
	lightShader = new LightShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	shadowShader = new ShadowShader(renderer->getDevice(), hwnd);
	horizontalBlurShader = new HorizontalBlurShader(renderer->getDevice(), hwnd);
	verticalBlurShader = new VerticalBlurShader(renderer->getDevice(), hwnd);
	tessShader = new TessellationShader(renderer->getDevice(), hwnd);
	vertShader = new VertexShader(renderer->getDevice(), hwnd);

	//Render Texture's
	renderTexture = new RenderTexture(renderer->getDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_NEAR, SCREEN_DEPTH);
	depthMap = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	upSample = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	//Lights
	m_light = new Light;
	m_light->setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);
	m_light->setDiffuseColour(0.7f, 0.0f, 0.0f, 1.0f);
	m_light->setPosition(-5.0f, 10.0f, -10.0f);
	m_light->setDirection(0.0, 0.0, 1.0f);
	m_light->setLookAt(0.0f, 0.0f, 0.0f);
	m_light->generateProjectionMatrix(1.0f, 100.0f);

	m_light2 = new Light;
	m_light2->setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);
	m_light2->setDiffuseColour(0.0f, 0.7f, 0.0f, 1.0f);
	m_light2->setPosition(5.0f, 10.0f, -10.0f);
	m_light2->setDirection(0.0, 0.0, 1.0f);
	m_light2->setLookAt(0.0f, 0.0f, 0.0f);
	m_light2->generateProjectionMatrix(1.0f, 100.0f);

	//Variables
	isWireframe = false;
	scene = false;
	screenH = screenHeight;
	screenW = screenWidth;
	
}

App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	if (cube)
	{
		delete cube;
		cube = 0;
	}

	if (plane)
	{
		delete plane;
		plane = 0;
	}

	if (mesh)
	{
		delete mesh;
		mesh = 0;
	}

	if (ortho)
	{
		delete ortho;
		ortho = 0;
	}

	if (orthoMesh)
	{
		delete orthoMesh;
		orthoMesh = 0;
	}

	if (tesMesh)
	{
		delete tesMesh;
		tesMesh = 0;
	}

	if (colourShader)
	{
		delete colourShader;
		colourShader = 0;
	}

	if (lightShader)
	{
		delete lightShader;
		lightShader = 0;
	}

	if (depthShader)
	{
		delete depthShader;
		depthShader = 0;
	}

	if (shadowShader)
	{
		delete shadowShader;
		shadowShader = 0;
	}

	if (textureShader)
	{
		delete textureShader;
		textureShader = 0;
	}

	if (horizontalBlurShader)
	{
		delete horizontalBlurShader;
		horizontalBlurShader = 0;
	}

	if (verticalBlurShader)
	{
		delete verticalBlurShader;
		verticalBlurShader = 0;
	}

	if (tessShader)
	{
		delete tessShader;
		tessShader = 0;
	}

	if (vertShader)
	{
		delete vertShader;
		vertShader = 0;
	}
}

bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}

	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

void App1::variableSampleSizes(float scale)
{
	//This resets the textures
	delete downSample;
	downSample = NULL;
	delete horizontalBlur;
	horizontalBlur = NULL;
	delete verticalBlur;
	verticalBlur = NULL;
	
	//This creates a new set of textures at the desired size
	downSample = new RenderTexture(renderer->getDevice(), screenW / scale, screenH / scale, SCREEN_NEAR, SCREEN_DEPTH);
	horizontalBlur = new RenderTexture(renderer->getDevice(), screenW / scale, screenH / scale, SCREEN_NEAR, SCREEN_DEPTH);
	verticalBlur = new RenderTexture(renderer->getDevice(), screenW / scale, screenH / scale, SCREEN_NEAR, SCREEN_DEPTH);
}

void App1::DepthMapFunction()
{
	XMMATRIX worldMatrix, lightViewMatrix, lightProjectionMatrix;

	//Get required matrices
	m_light->generateViewMatrix();

	lightViewMatrix = m_light->getViewMatrix();
	lightProjectionMatrix = m_light->getProjectionMatrix();
	worldMatrix = renderer->getWorldMatrix();

	//Set up the render texture
	depthMap->setRenderTarget(renderer->getDeviceContext());

	depthMap->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	//// Generate the view matrix based on the camera's position.
	camera->update();

	//Move the plane under the cube
	worldMatrix = XMMatrixTranslation(-30.0f, -1.0f, -20.0f);
	// Send geometry data (from mesh)
	plane->sendData(renderer->getDeviceContext());
	// Set shader parameters (matrices and texture)
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	// Render object (combination of mesh geometry and shader process
	depthShader->render(renderer->getDeviceContext(), plane->getIndexCount());

	//Reset the transform so the cube appears at the origin
	worldMatrix = renderer->getWorldMatrix();

	// Send geometry data (from mesh)
	cube->sendData(renderer->getDeviceContext());
	// Set shader parameters (matrices and texture)
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	// Render object (combination of mesh geometry and shader process
	depthShader->render(renderer->getDeviceContext(), cube->getIndexCount());

	//// Present the rendered scene to the screen.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::RenderToTexture()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX viewMatrix2;
	// Set the render target to be the render to texture.
	renderTexture->setRenderTarget(renderer->getDeviceContext());

	// Clear the render to texture.
	renderTexture->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	worldMatrix = XMMatrixTranslation(-30.0f, -1.0f, -20.0f);
	// Send geometry data (from mesh)
	plane->sendData(renderer->getDeviceContext());

	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("brick1"), depthMap->getShaderResourceView(), m_light);
	// Render object (combination of mesh geometry and shader process
	shadowShader->render(renderer->getDeviceContext(), plane->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();

	// Send geometry data (from mesh)
	cube->sendData(renderer->getDeviceContext());
	// Set shader parameters (matrices and texture)
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("supra"), depthMap->getShaderResourceView(), m_light);
	// Render object (combination of mesh geometry and shader process
	shadowShader->render(renderer->getDeviceContext(), cube->getIndexCount());

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

void App1::downSampleFunction()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoViewMatrix, orthoMatrix;

	downSample->setRenderTarget(renderer->getDeviceContext());
	downSample->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	//This section sets the render texture to display the scene, ready to be processed by the blur
	renderer->setZBuffer(false);

	orthoMatrix = renderer->getOrthoMatrix();

	orthoViewMatrix = camera->getOrthoViewMatrix();

	orthoMesh->sendData(renderer->getDeviceContext());

	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, renderTexture->getShaderResourceView());

	textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

	renderer->setZBuffer(true);

	renderer->setBackBufferRenderTarget();
}

void App1::horizontalBlurFunction()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoViewMatrix, orthoMatrix;
	float width = downSample->getTextureWidth();
	radAngle = (angle * XM_PI) / 180;

	horizontalBlur->setRenderTarget(renderer->getDeviceContext());
	horizontalBlur->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	//cube->sendData(renderer->getDeviceContext());

	renderer->setZBuffer(false);

	orthoMatrix = renderer->getOrthoMatrix();

	orthoViewMatrix = camera->getOrthoViewMatrix();

	orthoMesh->sendData(renderer->getDeviceContext());

	horizontalBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, downSample->getShaderResourceView(), width, radAngle, blurIntensity);

	horizontalBlurShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

	renderer->setZBuffer(true);

	renderer->setBackBufferRenderTarget();
}

void App1::verticalBlurFunction()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoViewMatrix, orthoMatrix;
	float width = downSample->getTextureWidth();
	radAngle = (angle * XM_PI) / 180;

	verticalBlur->setRenderTarget(renderer->getDeviceContext());
	verticalBlur->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	//cube->sendData(renderer->getDeviceContext());

	renderer->setZBuffer(false);

	orthoMatrix = renderer->getOrthoMatrix();

	orthoViewMatrix = camera->getOrthoViewMatrix();

	orthoMesh->sendData(renderer->getDeviceContext());

	verticalBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, horizontalBlur->getShaderResourceView(), width, radAngle, blurIntensity);

	verticalBlurShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

	renderer->setZBuffer(true);

	renderer->setBackBufferRenderTarget();
}

void App1::upSampleFunction()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoViewMatrix, orthoMatrix;

	upSample->setRenderTarget(renderer->getDeviceContext());
	upSample->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	renderer->setZBuffer(false);

	orthoMatrix = renderer->getOrthoMatrix();

	orthoViewMatrix = camera->getOrthoViewMatrix();

	orthoMesh->sendData(renderer->getDeviceContext());

	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, verticalBlur->getShaderResourceView());

	textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

	renderer->setZBuffer(true);

	renderer->setBackBufferRenderTarget();
}

void App1::RenderScene()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix, orthoViewMatrix;

	//// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();
	
	renderer->setZBuffer(false);

	orthoMatrix = renderer->getOrthoMatrix();
	orthoViewMatrix = camera->getOrthoViewMatrix();

	orthoMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, upSample->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

	renderer->setZBuffer(true);


}

void App1::TesselationRender()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	//// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	//// Generate the view matrix based on the camera's position.
	camera->update();

	//// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	renderer->setWireframeMode(isWireframe);

	worldMatrix = XMMatrixScaling(2.0f, 1.0f, 2.0f);

	tesMesh->sendData(renderer->getDeviceContext());
	// Set shader parameters (matrices and texture)
	tessShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("height"), camera->getPosition());
	// Render object (combination of mesh geometry and shader process
	tessShader->render(renderer->getDeviceContext(), tesMesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(5.0f, 0.0f, 0.0f);

	cube->sendData(renderer->getDeviceContext());
	vertShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("height"), m_light, 2.0f);
	vertShader->render(renderer->getDeviceContext(), cube->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
}

bool App1::render()
{
	if (scene == false)
	{
		//Generate textures
		variableSampleSizes(scale);

		//Obtain depth map
		DepthMapFunction();

		//Render the scene to a texture
		RenderToTexture();

		//Down sample said texture
		downSampleFunction();

		//Blur
		horizontalBlurFunction();

		//Blur
		verticalBlurFunction();

		//Up sample the texture
		upSampleFunction();

		//Put finished texture on the scene
		RenderScene();
	}

	if (scene == true)
	{
		//Render the tesselation scene
		TesselationRender();
	}

	gui();

	//// Present the rendered scene to the screen.
	renderer->endScene();

	return true;
}

void App1::gui()
{
	// Force turn off on Geometry shader
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	//Toggle between scenes
	ImGui::Checkbox("Scenes", &scene);
	if (scene == false)
	{
		isWireframe = false;
		//This number is what the screen size is divided by to scale the texure
		ImGui::SliderFloat("Render Scale", &scale, 0.25, 20);
		//Blur variables required by the shader
		ImGui::SliderFloat("Blur Angle", &angle, 0.0, 360);
		ImGui::SliderFloat("Blur Intensity", &blurIntensity, 0.0, 10.0);
	}
	if (scene == true)
	{
		//Toggles wireframe mode
		ImGui::Checkbox("Wireframe", &isWireframe);
	}

	// Render UI
	ImGui::Render();
}

