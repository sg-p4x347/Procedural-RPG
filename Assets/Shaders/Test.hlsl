//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

cbuffer ObjectVars : register(b0)
{
	float4x4 LocalToWorld4x4;
	float4x4 LocalToProjected4x4;
	float4x4 WorldToLocal4x4;
	float4x4 WorldToView4x4;
	float4x4 UVTransform4x4;
	float3 EyePosition;
};

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 main(PixelInputType input) : SV_TARGET
{
	return input.color;
}

//struct P2F
//{
//	float4 fragment : SV_Target;
//};
//P2F main(PixelInputType pixel)
//{
//	P2F result;
//	result.fragment = pixel.color;
//	return result;
//}