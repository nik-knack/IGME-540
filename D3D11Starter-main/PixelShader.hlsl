#include "ShaderIncludes.hlsli"

cbuffer ExternalData : register(b0)
{
    float4 colorTint;
    float2 uvScale;
    float2 uvOffset;
    float3 cameraPosition;
    float3 ambientColor;
    Light lights[6];
};

Texture2D SurfaceTexture : register(t0);
SamplerState BasicSampler : register(s0);

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
//  
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
    input.normal = normalize(input.normal);
    
    // Scale and offset the UV coords
    float2 adjustedUV = input.uv * uvScale + uvOffset;
	
	// Sample texture
    float3 surfaceColor = SurfaceTexture.Sample(BasicSampler, adjustedUV).rgb * colorTint.rgb;
    float3 ambient = ambientColor * surfaceColor;
	
    //return surfaceColor * colorTint;
    //return float4(input.normal, 1);
    return float4(lights[0].Color, 1);

}