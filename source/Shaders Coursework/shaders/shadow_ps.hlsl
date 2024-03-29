
Texture2D shaderTexture : register(t0);
Texture2D depthMapTexture : register(t1);

SamplerState SampleTypeWrap  : register(s0);
SamplerState SampleTypeClamp : register(s1);

cbuffer LightBuffer : register(cb0)
{
	float4 ambientColor;
	float4 diffuseColor;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
    float4 lightViewPosition : TEXCOORD1;
	float3 lightPos : TEXCOORD2;
};


float4 main(InputType input) : SV_TARGET
{
	float bias;
    float4 color;
	float4 projectTexCoord;
	float depthValue;
	float lightDepthValue;
    float lightIntensity;
	float4 textureColor;
	
	// Set the bias value for fixing the floating point precision issues.
	bias = 0.0001f;

	// Set the default output color to the ambient light value for all pixels.
    color = ambientColor;

	// Calculate projected coordinates, then into UV range
	projectTexCoord.xyz = input.lightViewPosition.xyz / input.lightViewPosition.z;

	// Calculate the projected texture coordinates.
	projectTexCoord.x =  (projectTexCoord.x / 2.0f) + 0.5f;
	projectTexCoord.y =  (-projectTexCoord.y / 2.0f) + 0.5f;


	// Determine if the projected coordinates are in the 0 to 1 range.  If so then this pixel is in the view of the light.
	if((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		// Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
		depthValue = depthMapTexture.Sample(SampleTypeClamp, projectTexCoord).r;

		// Calculate the depth of the light.
		lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;

		// Subtract the bias from the lightDepthValue.
		lightDepthValue = lightDepthValue - bias;

		// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
		// If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it.
		if(lightDepthValue < depthValue)
		{
		    // Calculate the amount of light on this pixel.
			lightIntensity = saturate(dot(input.normal, input.lightPos));

		    if(lightIntensity > 0.0f)
			{
				// Determine the final diffuse color based on the diffuse color and the amount of light intensity.
				color += (diffuseColor * lightIntensity);

				// Saturate the final light color.
				color = saturate(color);
			}
		}
	}

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample(SampleTypeWrap, input.tex);

	// Combine the light and texture color.
	color = color * textureColor;

    return color;
}