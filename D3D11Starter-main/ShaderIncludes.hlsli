#ifndef __GGP_SHADER_INCLUDES__ // Each .hlsli file needs a unique identifier!
#define __GGP_SHADER_INCLUDES__
#define LGIHT_TYPE_DIRECTIONAL 0
#define LGIHT_TYPE_POINT 1
#define LGIHT_TYPE_SPOT 2
#define MAX_LIGHTS 6

struct VertexShaderInput
{
    float3 localPosition    : POSITION; 
    float2 uv               : TEXCOORD0; 
    float3 normal           : NORMAL0; 
};

struct VertexToPixel
{
    float4 screenPosition   : SV_POSITION;
    float2 uv               : TEXCOORD0;
    float3 normal           : NORMAL0;
    float3 worldPosition    : TEXCOORD1;
};

struct Light
{
    int     Type;
    float3  Direction;
    float   Range;
    float3  Position;
    float   Intensity;
    float3  Color;
    float   SpotInnerAngle;
    float   SpotOuterAngle;
    float2  Padding;
};

float Attenuate(Light light, float3 worldPos)
{
    float dist = distance(light.Position, worldPos);
    float att = saturate(1.0 - (dist * dist / (light.Range * light.Range)));
    return att * att;
}

float Diffuse(float3 normal, float3 dirToLight)
{
    return saturate(dot(normal, dirToLight));
}

float SpecularPhong(float3 normal, float3 dirToLight, float3 dirToCamera, float shininess)
{
    float3 reflected = reflect(dirToLight, normal);
    return pow(saturate(dot(reflected, dirToCamera)), shininess);
}

#endif