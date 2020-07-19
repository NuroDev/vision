#include "Helper.fx"

//--------------------------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------------------------
static const int NUM_TEXTURES = 6;
Texture2D Textures[NUM_TEXTURES] : register(t0);
SamplerState PointSampler : register(s0);

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Position : POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 WorldPosition : POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;
    
    // Convert the position from linear space to world space
    output.Position = mul(input.Position, World);
    output.WorldPosition = output.Position;
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    
    // Convert input normals from model space to world space
    output.Normal = mul(input.Normal, World).xyz;
    
    // Pass texture straight over
    output.Tex = input.Tex;
    
    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_TARGET
{    
    // Define how much to blend the layers together
    float blendAmount = Textures[0].Sample(PointSampler, input.Tex).r;
    
    // Final color to return
    float4 textureColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    // Get the initial color by sampling a base texture (EG: Light dirt)
    float4 color = Textures[1].Sample(PointSampler, input.Tex);
    
    // Sample the rest of the textures
    float4 colorLightDirt = Textures[1].Sample(PointSampler, input.Tex);    // Light Dirt
    float4 colorGrass = Textures[2].Sample(PointSampler, input.Tex);        // Grass
    float4 colorDarkDirt = Textures[3].Sample(PointSampler, input.Tex);     // Dark Dirt
    float4 colorSnow = Textures[4].Sample(PointSampler, input.Tex);         // Snow
    float4 colorStone = Textures[5].Sample(PointSampler, input.Tex);        // Stone    
    
    // Calculate the slope of this point based on the normal
    float slope = 1.0f - input.Normal.y;
    
     // Determine which texture to use based on height
    if (slope < 0.2)
    {
        blendAmount = slope / 0.2f;
        textureColor = lerp(colorGrass, colorLightDirt, blendAmount);
    }
	
    if ((slope < 0.7) && (slope >= 0.2f))
    {
        blendAmount = (slope - 0.2f) * (1.0f / (0.7f - 0.2f));
        textureColor = lerp(colorLightDirt, colorStone, blendAmount);
    }

    if (slope >= 0.7)
    {
        textureColor = colorStone;
    }
    
    // Calculate lighting
    LightingResult light = ComputeLighting(input.WorldPosition, normalize(input.Normal));
    float4 emissive = Material.Emissive;
    float4 ambient = Material.Ambient * GlobalAmbient;
    float4 diffuse = Material.Diffuse * light.Diffuse;
    float4 specular = Material.Specular * light.Specular;

    return saturate((emissive + ambient + diffuse + specular) * color) * textureColor;
}
