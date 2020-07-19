#include "Helper.fx"

//--------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------
Texture2D Textures[2] : register(t0);
SamplerState PointSampler : register(s0);

//--------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------
struct VS_INPUT
{
	float4 Position : POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
    float3 Tangent : TANGENT;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
	float4 WorldPosition : POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
};

//--------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;
	
	// Convert the position from linear space to world space
	output.Position = mul(input.Position, World);
	output.WorldPosition = output.Position;
	output.Position = mul(output.Position, View);
	output.Position = mul(output.Position, Projection);
	
	// Multiply the normal by the world transform (to go from model space to world space)
    output.Normal = mul(float4(input.Normal, 1), World).xyz;
	
	// Pass texture straight over
	output.Tex = input.Tex;
	
	// Calculate TBN Matrix
    float3 N = output.Normal;
    float3 T = normalize(input.Tangent - N * dot(input.Tangent, N));
    float3 B = cross(T, N);
    float3x3 TBN = transpose(float3x3(normalize(T), normalize(B), normalize(N)));
	
	// Convert the input normals from model space to tangent space
    output.Normal = mul(output.Normal, TBN);
	
	return output;
}

//--------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_TARGET
{
    float4 textureColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	
	if (Material.UseTexture)
		textureColor = Textures[0].Sample(PointSampler, input.Tex);

	if (Material.UseNormalMap)
	{
		// Sample the normal map texture
		float4 normalMap = Textures[1].Sample(PointSampler, input.Tex);

		// Transform normal vector to range [-1, 1]
		normalMap = (normalMap * 2.0f) - 1.0f;
		normalMap = float4(normalize(normalMap.xyz), 1);

		// Fix: Inverse normal map due to incorrect texture UVs
		normalMap.y *= -1;

		// Apply newly calculated normals to input normals
		// so they can be used to calculate the lighting
		input.Normal = float4(normalize(normalMap.xyz), 1);
	}
	
	// Calculate the lighting AFTER the new normals have been applied via the normal map
	LightingResult light = ComputeLighting(input.WorldPosition, normalize(input.Normal));
	
	float4 emissive = Material.Emissive;
	float4 ambient = Material.Ambient * GlobalAmbient;
	float4 diffuse = Material.Diffuse * light.Diffuse;
	float4 specular = Material.Specular * light.Specular;
	
	return (emissive + ambient + diffuse + specular) * textureColor;
}
