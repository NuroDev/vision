//--------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------
#define PI 3.14159265359
#define E 2.71828182846

//--------------------------------------------------------------------
// Effects Parameters
//--------------------------------------------------------------------
struct _Effects
{
    bool Blur;
    bool Blur_Gausian;
    int Blur_Samples;
    float Blur_Size;
    float Blur_StandardDeviation;

    bool ColorTint;
    bool ColorTint_GrayScale;
    bool ColorTint_Invert;
    float3 ColorTint_Color;
    bool ColorTint_GameBoy;
    bool ColorTint_Sepia;
    
    bool Pixelate;
    float PixelationFactor;
    
    bool Vignette;
    float VignetteAmount;
    float VignetteRadius;
    float VignetteRatio;
    int VignetteSlope;
    
    int ColorSpace;
    
    bool ChromaticAberration;
    float ChromaticAberrationFactor;
    
    bool PictureInPicture;
    int ScreenWidth;
    int ScreenHeight;
};

//--------------------------------------------------------------------
// Buffers
//--------------------------------------------------------------------
cbuffer EffectsProperties : register(b3)
{
    _Effects Effects;
}
