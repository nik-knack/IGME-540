#include "ShaderIncludes.hlsli"

cbuffer VertexShaderData : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 projection;
    float4x4 worldInvTranspose;
};

// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// 
// - Input is exactly one vertex worth of data (defined by a struct)
// - Output is a single struct of data to pass down the pipeline
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
VertexToPixel main( VertexShaderInput input )
{
	VertexToPixel output;

    float4 worldPosition = mul(world, float4(input.localPosition, 1.0f));
    float4 viewPosition = mul(view, worldPosition);
	
    output.screenPosition = mul(projection, viewPosition);
    output.uv = input.uv;
    output.normal = mul((float3x3)worldInvTranspose, input.normal);
    output.tangent = mul((float3x3)world, input.tangent);
    output.worldPosition = worldPosition.xyz;

	return output;
}