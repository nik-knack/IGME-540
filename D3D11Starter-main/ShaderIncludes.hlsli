#ifndef __GGP_SHADER_INCLUDES__ // Each .hlsli file needs a unique identifier!
#define __GGP_SHADER_INCLUDES__
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2
#define MAX_LIGHTS 6
#define PI 3.14159265359f
#define MIN_ROUGHNESS 0.0001f

struct VertexShaderInput
{
    float3 localPosition    : POSITION; 
    float2 uv               : TEXCOORD0; 
    float3 normal           : NORMAL0; 
    float3 tangent          : TANGENT0;
};

struct VertexToPixel
{
    float4 screenPosition   : SV_POSITION;
    float2 uv               : TEXCOORD0;
    float3 normal           : NORMAL0;
    float3 worldPosition    : TEXCOORD1;
    float3 tangent          : TANGENT0;
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

struct VertexToPixel_Sky
{
    float4 position         : SV_POSITION;
    float3 sampleDir        : DIRECTION;
};


// Helper functions

// Normal Distribution function
float D_GGX(float3 normal, float3 halfVector, float roughness)
{
    float NdotH = saturate(dot(normal, halfVector));
    float NdotH2 = NdotH * NdotH;
    float a = roughness * roughness;
    float a2 = max(a * a, MIN_ROUGHNESS);
    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;
    return a2 / denom;
}

// Fresnel functions
float F_Schlick(float3 viewDir, float3 halfVector, float3 f0)
{
    float VdotH = saturate(dot(viewDir, halfVector));
    return f0 + (1.0f - f0) * pow(1.0f - VdotH, 5.0f);
}

// Geometric shadowing function
float G_Schlick(float3 normal, float3 viewDir, float roughness)
{
    float k = pow(roughness + 1.0f, 2.0f) / 8.0f;
    float NdotV = saturate(dot(normal, viewDir));
    return NdotV / (NdotV * (1.0f - k) + k);
}

// Cook-Torrance specular
float3 MicrofacetBRDF(float3 normal, float3 lightDir, float3 viewDir, float roughness, float3 f0, out float3 F)
{
    roughness = max(roughness, MIN_ROUGHNESS);
    float h = normalize(lightDir + viewDir);
    
    float D = D_GGX(normal, h, roughness);
    F       = F_Schlick(viewDir, h, f0);
    float G = G_Schlick(normal, viewDir, roughness) * G_Schlick(normal, lightDir, roughness);
    
    float NdotV = max(saturate(dot(normal, viewDir)), MIN_ROUGHNESS);   
    return (D * F * G) / (4.0f * NdotV); 
}

// Conserve energy for diffuse
float3 DiffuseEnergyConservation(float3 diffuse, float3 F, float roughness)
{ 
    return diffuse * (1.0f - F) * (1.0f - roughness);
}

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

float3 PBRLighting(float3 normal, float3 toLight, float3 toCam, float roughness, 
                   float metalness, float3 surfaceColor, float3 specColor)
{
    float diff = Diffuse(normal, toLight);
    float3 F;
    float3 spec = MicrofacetBRDF(normal, toLight, toCam, roughness, specColor, F);
    float3 balancedDiff = DiffuseEnergyConservation(diff, F, roughness);
    return balancedDiff * surfaceColor + spec;
}

float3 DirectionalLight(Light light, float3 normal, float3 worldPos, float3 camPos, 
                        float roughness, float metalness, float3 surfaceColor, float3 specColor)
{
    float3 toLight = normalize(-light.Direction);
    float3 toCam = normalize(camPos - worldPos);
    return PBRLighting(normal, toLight, toCam, roughness, metalness, surfaceColor, specColor) * light.Intensity * light.Color;
}

float3 PointLight(Light light, float3 normal, float3 worldPos, float3 camPos,
                        float roughness, float metalness, float3 surfaceColor, float3 specColor)
{
    float3 toLight = normalize(light.Position - worldPos);
    float3 toCam = normalize(camPos - worldPos);
    return PBRLighting(normal, toLight, toCam, roughness, metalness, surfaceColor, specColor) 
        * Attenuate(light, worldPos) * light.Intensity * light.Color;
}

float3 SpotLight(Light light, float3 normal, float3 worldPos, float3 camPos,
                        float roughness, float metalness, float3 surfaceColor, float3 specColor)
{
    float3 dirFromLight = normalize(worldPos - light.Position);
    float pixelCos = saturate(dot(dirFromLight, normalize(light.Direction)));
    float cosOuter = cos(light.SpotOuterAngle);
    float cosInner = cos(light.SpotInnerAngle);
    float falloff = saturate((pixelCos - cosOuter) / (cosInner - cosOuter));
    return PointLight(light, normal, worldPos, camPos, roughness, metalness, surfaceColor, specColor) * falloff;
}

#endif