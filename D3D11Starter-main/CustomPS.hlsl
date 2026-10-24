cbuffer PixelShaderData : register(b0)
{
    float4 colorTint;
};


struct VertexToPixel
{
    float4 screenPosition   : SV_POSITION;
    float2 uv               : TEXCOORD0;
    float3 normal           : NORMAL0;
};


float4 main(VertexToPixel input) : SV_TARGET
{
    // Split the UV into an 8 x 8 grid
    float2 cell = floor(input.uv * 8.0);
    
    // Alternate between 0 and 1
    float checker = fmod(cell.x + cell.y, 2.0);
    
    // Create alternating color tints on each square
    float3 color = lerp(colorTint.rgb * 0.25, colorTint.rgb, checker);
    
    return float4(color, 1);
}	