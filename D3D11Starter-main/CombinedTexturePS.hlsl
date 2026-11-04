cbuffer ExternalData : register(b0)
{
    float4 colorTint;
    float2 uvScale;
    float2 uvOffset;
};

Texture2D BaseTexture : register(t0);
Texture2D OveralyTexture : register(t1);
SamplerState BasicSampler : register(s0);

struct VertexToPixel
{
    float4 screenPosition   : SV_POSITION;
    float2 uv               : TEXCOORD0;
    float3 normal           : NORMAL0;
};

float4 main(VertexToPixel input) : SV_TARGET
{
    float2 adjustedUV = input.uv * uvScale + uvOffset;
    
    float4 baseColor = BaseTexture.Sample(BasicSampler, adjustedUV);
    float4 overlayColor = OveralyTexture.Sample(BasicSampler, adjustedUV);
    
    // Combine both
    float4 combinedColor = baseColor * overlayColor;
    
    return combinedColor * colorTint;
}