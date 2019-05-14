// Light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer MatrixBuffer : register(cb0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer TimeBuffer : register(cb1)
{
	float height;
	float freq;
	float time;
	float padding;
}

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

OutputType main(InputType input)
{
    OutputType output;
	float textureHeight;

    input.position.w = 1.0f;

	//offset position based on sine wave
	//input.position.y += height *sin(input.position.x + time);
	
	//Find the height off of the height texture
	textureHeight = shaderTexture.SampleLevel(SampleType, input.tex, 0);

	//Find out what face of the cube is being manipulated then move the vertex in the same direction as the normal
	if (input.normal.x == 1 || input.normal.x == -1)
	{
		input.position.x += (input.normal.x * textureHeight);
	}

	if (input.normal.y == 1 || input.normal.y == -1)
	{
		input.position.y += (input.normal.y * textureHeight);
	}

	if (input.normal.z == 1 || input.normal.z == -1)
	{
		input.position.z += (input.normal.z * textureHeight);
	}

	//modify normals
	input.normal.x = 1 - cos(input.position.x + time);
	input.normal.y = abs(cos(input.position.x + time));

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

	 // Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
	
    // Normalize the normal vector.
    output.normal = normalize(output.normal);

    return output;
}