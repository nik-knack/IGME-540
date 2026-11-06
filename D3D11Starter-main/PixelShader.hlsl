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
    float3 total = ambientColor * surfaceColor;
    
    for (int i = 0; i < 6; i++)
    {
        Light light = lights[i];
        switch (light.Type)
        {
            case 0:
                total += DirectionalLight(light, input.normal, input.worldPosition, cameraPosition, surfaceColor);
                break;
            case 1:
                total += PointLight(light, input.normal, input.worldPosition, cameraPosition, surfaceColor);
                break;
            case 2:
                total += SpotLight(light, input.normal, input.worldPosition, cameraPosition, surfaceColor);
                break;
        }
    }
    
    return float4(total, 1);
}