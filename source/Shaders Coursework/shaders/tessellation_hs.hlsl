// Tessellation Hull Shader
// Prepares control points for tessellation

cbuffer TessBuffer : register (cb0)
{
	float3 cameraPosition;
	float padding;
};

struct InputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 colour : COLOR;
};

struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct OutputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 colour : COLOR;
};

float CalculateDistance(float3 pos1, float3 pos2)
{
	//Create a vector from the two points
	float dx = (pos1.x - pos2.x);
	float dy = (pos1.y - pos2.y);
	float dz = (pos1.z - pos2.z);

	//Find the magnitude of said vector
	float distance = sqrt((dx * dx) + (dy * dy) + (dz * dz));

	//Tweak  the factor so that it tesselates at relevant distances
	float factor = 25.0f - (distance * 2);

	//Clamp the tesselation factor
	if (factor < 2)
	{
		factor = 2;
	}

	if (factor > 64)
	{
		factor = 64;
	}

	return factor;

}

ConstantOutputType PatchConstantFunction(InputPatch<InputType, 4> inputPatch, uint patchId : SV_PrimitiveID)
{    
    ConstantOutputType output;
	float factor;
	float mid;

	//Find mid point 
	mid = inputPatch[0].position + inputPatch[1].position + inputPatch[2].position + inputPatch[3].position;
	mid /= 4;

	//Get tesselation factor required for the distance viewed from
	factor = CalculateDistance(cameraPosition, mid);

    // Set the tessellation factors for the three edges of the triangle.
	output.edges[0] = factor;
	output.edges[1] = factor;
	output.edges[2] = factor;
	output.edges[3] = factor;

    // Set the tessellation factor for tessallating inside the triangle.
	output.inside[0] = factor;
	output.inside[1] = factor;

    return output;
}


[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchConstantFunction")]
OutputType main(InputPatch<InputType, 4> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    OutputType output;

    // Set the position for this control point as the output position.
    output.position = patch[pointId].position;

    // Set the input color as the output color.
    output.colour = patch[pointId].colour;

    return output;
}