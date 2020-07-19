#include "Helper.fx"

//--------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------
Texture2D ColorTexture : register(t0);
Texture2D NormalTexture : register(t1);
SamplerState PointSampler : register(s0);

//--------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------
struct VS_INPUT
{
	float4 Position : POSITION;
	float2 Tex : TEXCOORD;
};

struct PS_INPUT
{
	float4 Position : SV_POSITION;
	float2 Tex : TEXCOORD;
};

//--------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;
	
	// Set a base/default 4th unit for the position for proper matrix calculations
    input.Position.w = 1.0f;

    // Calculate the position of the vertex against the world, view & projection matrices
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    
    // Pass texture co-ords straight over
    output.Tex = input.Tex;
    
    return output;
}

//--------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_TARGET
{
    float4 textureColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    // Sample the gbuffer render targets
    float4 color = ColorTexture.Sample(PointSampler, input.Tex);
    float4 normals = NormalTexture.Sample(PointSampler, input.Tex);
    
    // Calculate the lighting
    LightingResult light = ComputeLighting(input.Position, normalize(normals.rgb));

    float4 emissive = Material.Emissive;
    float4 ambient = Material.Ambient * GlobalAmbient;
    float4 diffuse = Material.Diffuse * light.Diffuse;
    float4 specular = Material.Specular * light.Specular;

    // Determine the final amount of diffuse color based on the color of the pixel combined with the light intensity
    return saturate(color * (emissive + ambient + diffuse + specular));
}
