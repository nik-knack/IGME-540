#include "ShaderIncludes.hlsli"

cbuffer SkyVSData : register(b0)
{
    float4x4 view;
    float4x4 projection;
}

VertexToPixel_Sky main(VertexShaderInput input)
{
    VertexToPixel_Sky output;

    // Remove translation so the sky stays centered on the camera
    float4x4 viewNoTranslation = view;
    viewNoTranslation._14 = 0;
    viewNoTranslation._24 = 0;
    viewNoTranslation._34 = 0;

    matrix vp = mul(projection, viewNoTranslation);
    output.position = mul(vp, float4(input.localPosition, 1.0f));

    // Force depth to exactly 1.0 after the perspective divide
    output.position.z = output.position.w;

    // Direction from the center of the cube to this vertex
    output.sampleDir = input.localPosition;

    return output;
}