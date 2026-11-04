#ifndef __GGP_SHADER_INCLUDES__ // Each .hlsli file needs a unique identifier!
#define __GGP_SHADER_INCLUDES__
// ALL of your code pieces (structs, functions, etc.) go here!

struct VertexShaderInput
{
    float3 localPosition : POSITION; // XYZ position
    float2 uv : TEXCOORD; // UV coordinates
    float3 normal : NORMAL; // Normal vector
};

struct VertexToPixel
{
    float4 screenPosition : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
};

#endif