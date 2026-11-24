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

Texture2D Albedo            : register(t0);
Texture2D NormalMap         : register(t1);
Texture2D RoughnessMap      : register(t2);
Texture2D MetalnessMap      : register(t3);
SamplerState BasicSampler   : register(s0);
//Texture2D SurfaceTexture    : register(t0);

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
    float2 adjustedUV = input.uv * uvScale + uvOffset;
    
    float3 N = normalize(input.normal);
    float3 T = normalize(input.tangent);
    T = normalize(T - dot(T, N) * N);
    float3 B = cross(T, N);
    float3x3 TBN = float3x3(T, B, N); 
    float3 unpackedNormal = NormalMap.Sample(BasicSampler, adjustedUV).rgb * 2.0f - 1.0f;
    input.normal = normalize(mul(unpackedNormal, TBN));
	
    // Albedo: gama-UNcorrect
    float3 albedo = pow(Albedo.Sample(BasicSampler, adjustedUV).rgb, 2.2f) * colorTint.rgb;
    
	// Roughtness and metalness 
    float roughness = RoughnessMap.Sample(BasicSampler, adjustedUV).r;
    float metalness = MetalnessMap.Sample(BasicSampler, adjustedUV).r;
    
    // Specular color
    float3 specColor = lerp(0.04f, albedo, metalness);
    
    // Ambient is indirect diffuse
    float3 total = ambientColor * albedo * (1.0f - metalness);
    
    for (int i = 0; i < 6; i++)
    {
        Light light = lights[i];
        switch (light.Type)
        {
            case 0:
                total += DirectionalLight(light, input.normal, input.worldPosition, cameraPosition, roughness, metalness, albedo, specColor);
                break;
            case 1:
                total += PointLight(light, input.normal, input.worldPosition, cameraPosition, roughness, metalness, albedo, specColor);
                break;
            case 2:
                total += SpotLight(light, input.normal, input.worldPosition, cameraPosition, roughness, metalness, albedo, specColor);
                break;
        }
    }
    
    // Gamma-correct the final color
    return float4(pow(total, 1.0f / 2.2f), 1);
}