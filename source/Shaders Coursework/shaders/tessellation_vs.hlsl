// Tessellation vertex shader.
// Doesn't do much, could manipulate the control points
// Pass forward data, strip out some values not required for example.

cbuffer MatrixBuffer : register(cb0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct InputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
    float4 colour : COLOR;
};

OutputType main(InputType input)
{
    OutputType output;

	// Pass the vertex position into the hull shader.
    output.position = input.position;
	output.tex = input.tex;
	output.normal = input.normal;
    
    return output;
}
