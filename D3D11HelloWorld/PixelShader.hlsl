// Struct representing pixel shader input
struct VertexToPixel
{
	float4 screenPosition : SV_POSITION;
	float4 color : COLOR;
};
// --------------------------------------------------------
// The entry point for our pixel shader
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	return input.color;
}