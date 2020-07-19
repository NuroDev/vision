#include "Helper.fx"
#include "Effects.fx"

//--------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------
Texture2D Texture : register(t0);
SamplerState PointSampler : register(s0)
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

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
// Blur (@TotallyRonja)
// Source: https://www.ronja-tutorials.com/2018/08/27/postprocessing-blur.html
//--------------------------------------------------------------------
float4 Blur(float4 diffuse, float2 texCoord, float aspectRatio)
{
    float sum = 0;
    float stDevSquared = 0;
    
    // The blur will break if the standard deviation is equal to zero
    if (Effects.Blur_Gausian)
    {
        if (Effects.Blur_StandardDeviation == 0)
            return Texture.Sample(PointSampler, texCoord);
        
        stDevSquared = Effects.Blur_StandardDeviation * Effects.Blur_StandardDeviation;
        sum = Effects.Blur_Samples;
    }
    
    for (float i = 0; i < Effects.Blur_Samples; ++i)
    {
        float offset = (i / (Effects.Blur_Samples - 1) - 0.5) * Effects.Blur_Size;
        
        float2 UV = texCoord + float2(0, offset);
        
        if (!Effects.Blur_Gausian) {
            diffuse += Texture.Sample(PointSampler, UV);
        } else {
            float gaussian = (1 / sqrt(2 * PI * stDevSquared)) * pow(E, -((offset * offset) / 2 * stDevSquared));
            sum += gaussian;
            diffuse += Texture.Sample(PointSampler, UV) * gaussian;
        }
    }
    
    diffuse /= Effects.Blur_Samples;
    
    return diffuse;
}

float2 Pixelate(float2 TexCoord)
{    
    TexCoord.x *= Effects.ScreenWidth.x;
    TexCoord.y *= Effects.ScreenHeight;
    
    TexCoord.x = round(TexCoord.x / Effects.PixelationFactor) * Effects.PixelationFactor;
    TexCoord.y = round(TexCoord.y / Effects.PixelationFactor) * Effects.PixelationFactor;
    
    TexCoord.x /= Effects.ScreenWidth;
    TexCoord.y /= Effects.ScreenHeight;
    
    return TexCoord;
}

float4 ColorTint(float4 diffuse, float3 color)
{    
    if (Effects.ColorTint_GrayScale)
        diffuse = dot(diffuse.rgb, color);
    
    if (Effects.ColorTint_Invert)
        diffuse = 1.0f - diffuse;
    
    //--------------------------------------------------------------------
    // Sepia Tone Shader (RB Whitaker)
    // Source: http://rbwhitaker.wikidot.com/post-processing-effects
    //--------------------------------------------------------------------
    if (Effects.ColorTint_Sepia)
    {
        float4 initialDiffuse = diffuse;        
        diffuse.r = (initialDiffuse.r * 0.393) + (initialDiffuse.g * 0.769) + (initialDiffuse.b * 0.189);
        diffuse.g = (initialDiffuse.r * 0.349) + (initialDiffuse.g * 0.686) + (initialDiffuse.b * 0.168);
        diffuse.b = (initialDiffuse.r * 0.272) + (initialDiffuse.g * 0.534) + (initialDiffuse.b * 0.131);
    }
    
    //--------------------------------------------------------------------
    // Gameboy Shader (Andre Odendaal) 
    // Source: http://www.shaderslab.com/demo-98---game-boy-effect.html
    //--------------------------------------------------------------------
    if (Effects.ColorTint_GameBoy)
    {
        if (diffuse.r <= 0.25)
            diffuse = float4(0.06, 0.22, 0.06, 1.0);
        else if (diffuse.r > 0.75)
            diffuse = float4(0.6, 0.74, 0.06, 1.0);
        else if (diffuse.r > 0.25 && diffuse.r <= 0.5)
            diffuse = float4(0.19, 0.38, 0.19, 1.0);
        else
            diffuse = float4(0.54, 0.67, 0.06, 1.0);
    }

    diffuse.rgb *= color;
    
    return diffuse;    
}

//--------------------------------------------------------------------
// Vignette (@zachsaw)
// Source: https://github.com/zachsaw/RenderScripts/blob/master/RenderScripts/ImageProcessingShaders/SweetFX/Vignette.hlsl
//--------------------------------------------------------------------
float4 Vignette(float4 diffuse, float2 TexCoord)
{    
    const float2 VignetteCenter = float2(0.500, 0.500);
    
    // Set the center
    float2 tc = TexCoord - VignetteCenter;

    // Sample the pixel
    float4 pixel = Texture.Sample(PointSampler, TexCoord);
    
    // Adjust the ratio
    tc *= float2((pixel.y / pixel.x), Effects.VignetteRatio);

    // Calculate the distance
    tc /= Effects.VignetteRadius;
    float v = dot(tc, tc);

    // Apply the vignette
    diffuse.rgb *= (1.0 + pow(v, Effects.VignetteSlope * 0.5) * Effects.VignetteAmount); // pow - multiply
    
    return diffuse;
}

// --------------------------------------------------------------------
// Color Space Conversions (Nvidia Shader Library)
// Source: http://developer.download.nvidia.com/shaderlibrary/webpages/hlsl_shaders.html
// --------------------------------------------------------------------
float3 rgb_to_yuv(float3 RGB)
{
    float y = dot(RGB, float3(0.299, 0.587, 0.114));
    float u = (RGB.z - y) * 0.565;
    float v = (RGB.x - y) * 0.713;
    
    return float3(y, u, v);
}

float3 yuv_to_rgb(float3 YUV)
{
    float u = YUV.y;
    float v = YUV.z;
    
    return float3(
        YUV.x + 1.403 * v,              // R
        YUV.x - 0.344 * u - 1.403 * v,  // G
        YUV.x + 1.770 * u               // B
    );
}

float __min_channel(float3 v)
{
    float t = (v.x < v.y) ? v.x : v.y;
    t = (t < v.z) ? t : v.z;
    return t;
}

float __max_channel(float3 v)
{
    float t = (v.x > v.y) ? v.x : v.y;
    t = (t > v.z) ? t : v.z;
    return t;
}

float3 rgb_to_hsv(float3 RGB)
{
    float3 HSV = (0.0).xxx;
    
    float minVal = __min_channel(RGB);
    float maxVal = __max_channel(RGB);
    
    float delta = maxVal - minVal; // Delta RGB value 
    
    HSV.z = maxVal;
    
    // If gray, leave H & S at zero
    if (delta != 0)
    {
        HSV.y = delta / maxVal;
        float3 delRGB;
        delRGB = (((maxVal.xxx - RGB) / 6.0) + (delta / 2.0)) / delta;
        if (RGB.x == maxVal)
            HSV.x = delRGB.z - delRGB.y;
        else if (RGB.y == maxVal)
            HSV.x = (1.0 / 3.0) + delRGB.x - delRGB.z;
        else if (RGB.z == maxVal)
            HSV.x = (2.0 / 3.0) + delRGB.y - delRGB.x;
        if (HSV.x < 0.0)
        {
            HSV.x += 1.0;
        }
        if (HSV.x > 1.0)
        {
            HSV.x -= 1.0;
        }
    }
    
    return (HSV);
}

float3 hsv_to_rgb(float3 HSV)
{    
    float3 RGB = HSV.z;
    
    if (HSV.y != 0)
    {
        float var_h = HSV.x * 6;
        float var_i = floor(var_h); // Or ... var_i = floor( var_h )
        float var_1 = HSV.z * (1.0 - HSV.y);
        float var_2 = HSV.z * (1.0 - HSV.y * (var_h - var_i));
        float var_3 = HSV.z * (1.0 - HSV.y * (1 - (var_h - var_i)));
        if (var_i == 0)
        {
            RGB = float3(HSV.z, var_3, var_1);
        }
        else if (var_i == 1)
        {
            RGB = float3(var_2, HSV.z, var_1);
        }
        else if (var_i == 2)
        {
            RGB = float3(var_1, HSV.z, var_3);
        }
        else if (var_i == 3)
        {
            RGB = float3(var_1, var_2, HSV.z);
        }
        else if (var_i == 4)
        {
            RGB = float3(var_3, var_1, HSV.z);
        }
        else
        {
            RGB = float3(HSV.z, var_1, var_2);
        }
    }
    
    return (RGB);
}

float3 rgb_to_cmy(float3 rgbColor)
{
    return (float3(1, 1, 1) - rgbColor);
}

float3 cmy_to_rgb(float3 cmyColor)
{
    return (float3(1, 1, 1) - cmyColor);
}

//--------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;
    
    output.Position = input.Position;
    output.Tex = input.Tex;
    
    // Shrink the quad and offset to the
    // bottom right corner of the screen
    if (Effects.PictureInPicture)
    {
        output.Position.x /= 2;
        output.Position.y /= 2;
        output.Position.x += 0.5;
        output.Position.y -= 0.5;
    }
    
    return output;
}

//--------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_TARGET
{
    if (Effects.Pixelate)
        input.Tex = Pixelate(input.Tex);
    
    float4 color = Texture.Sample(PointSampler, input.Tex);
    
    if (Effects.ColorTint)
        color = ColorTint(color, Effects.ColorTint_Color);
    
    if (Effects.Blur)
        color = Blur(color, input.Tex, (Effects.ScreenHeight / Effects.ScreenWidth));
    
    if (Effects.Vignette)
        color = Vignette(color, input.Tex);
    
    if (Effects.ColorSpace == 1)
        color = float4(rgb_to_hsv(color.xyz), color.w);
    if (Effects.ColorSpace == 2)
    {
        color -= float4(0.0f, 0.5f, 0.5f, 0.0f);
        color = float4(rgb_to_yuv(color.xyz), color.w);
    }
    if (Effects.ColorSpace == 3)
        color = float4(rgb_to_cmy(color.xyz), color.w);
    
    return color;
}
