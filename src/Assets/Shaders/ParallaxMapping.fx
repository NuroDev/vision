#include "Helper.fx"

//--------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------
Texture2D Textures[3] : register(t0);
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
	float4 WorldPos : POSITION;
    float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
    
    float3 ViewPosition : VIEW_POS;
    float3 PixelPosition : PIXEL_POS;
};

//--------------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------------
float2 ParallaxMapping(PS_INPUT input)
{
    //--------------------------------------------------------------------
    // Parallax Mapping - LearnOpenGL
    // Source: https://learnopengl.com/Advanced-Lighting/Parallax-Mapping
    //--------------------------------------------------------------------
    
    // Calculate to eye vector
    float3 toEye = normalize(input.ViewPosition - input.PixelPosition);
	
    // Compute constant derivitives
    const float2 dx = ddx(input.Tex);
    const float2 dy = ddy(input.Tex);

    // Linearly interpolate the number of depth layers
    float numLayers = (int) lerp(Material.MinSamples, Material.MaxSamples, dot(toEye, input.Normal));
    
    // Calculate the size of each layer
    float layerDepth = 1.0f / numLayers;
    
    // Depth of the current layer
    float currentLayerDepth = 0.0f;

    // The degree to shift the texture co-ords for each layer
    float2 deltaTexCoords = (toEye.xy * Material.HeightScale) / numLayers;

    // Get initial texture co-ords
    float2 currentTexCoords = input.Tex;
    
    // Sample the heightmap using a gradient to influence the way the sample location is calculated
    float currentDepthMapValue = Textures[2].SampleGrad(PointSampler, input.Tex, dx, dy).r;
    
    while (currentLayerDepth < currentDepthMapValue)
    {
        // Offset the texture co-ords
        currentTexCoords += deltaTexCoords;
        
        // Sample current texture co-ord depth value
        currentDepthMapValue = Textures[2].SampleGrad(PointSampler, currentTexCoords, dx, dy).r;
        
        // Get the depth of the next layer
        currentLayerDepth += layerDepth;
    }
    
    // Return if parallax occlusion mapping is disabled
    if (!Material.UseParallaxOcclusion)
        return currentTexCoords;
    
    // Get the texture coordinates before collision (reverse operations)
    float2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // Get the depth before & after collision for linear interpolation
    float afterDepth = currentDepthMapValue - currentLayerDepth;
    float2 beforeDepth = Textures[2].SampleGrad(PointSampler, prevTexCoords, dx, dy).r - currentLayerDepth + layerDepth;
 
    // Interpolate texture coordinates 
    float weight = afterDepth / (afterDepth - beforeDepth);
    float2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

//--------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;
	
    // Convert the position from linear space to world space
	output.Position = mul(input.Position, World);
    output.WorldPos = output.Position;
	output.Position = mul(output.Position, View);
	output.Position = mul(output.Position, Projection);

    // Convert input normals from model space to world space
	output.Normal = mul(input.Normal, World).xyz;
    
    // Convert tangents to world space
    input.Tangent = mul(input.Tangent, World);
	
	// Pass texture straight over
	output.Tex = input.Tex;
    
    // Calculate TBN Matrix (Calculate bi-normals on the fly)
    float3 N = output.Normal;
    float3 T = normalize(input.Tangent - N * dot(input.Tangent, N));
    float3 B = cross(T, N);
    float3x3 TBN = transpose(float3x3(normalize(T), normalize(B), normalize(N)));

    // Convert eye position to world space & then to tangent space
    float3 eyePos = mul(EyePosition, World).xyz;
    output.ViewPosition = mul(eyePos, TBN);
    
    // Get the pixel position based on the world position, converted to tangent space
    output.PixelPosition = mul(output.WorldPos.xyz, TBN);
    
    // Convert input normals from world space to tangent space
    output.Normal = mul(output.Normal, TBN);

	return output;
}

//--------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_TARGET
{
    float4 textureColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	
    if (Material.UseParallaxMap)
        input.Tex = ParallaxMapping(input);
    
    if (input.Tex.x > 1.0 || input.Tex.y > 1.0 || input.Tex.x < 0.0 || input.Tex.y < 0.0)
        discard;
    
    if (Material.UseTexture)
        textureColor = Textures[0].Sample(PointSampler, input.Tex);
	
    if (Material.UseNormalMap)
    {
		// Sample the normal map texture
        float3 normalMap = Textures[1].Sample(PointSampler, input.Tex);

		// Transform normal vector to range [-1, 1]
        normalMap = (normalMap * 2.0f) - 1.0f;

		// Fix: Inverse normal map due to incorrect texture UVs
        normalMap.y *= -1;

		// Apply newly calculated normals to input normals
		// so they can be used to calculate the lighting
        input.Normal = normalize(normalMap.xyz);
    }
	
    LightingResult light = ComputeLighting(input.WorldPos, normalize(input.Normal));
    
    float4 emissive = Material.Emissive;
    float4 ambient = Material.Ambient * GlobalAmbient;
    float4 diffuse = Material.Diffuse * light.Diffuse;
    float4 specular = Material.Specular * light.Specular;
    
    return (emissive + ambient + diffuse + specular) * textureColor;
}
