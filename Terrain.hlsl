Texture2D Texture1 : register( t0 );
Texture2D Texture2 : register( t1 );
Texture2D Texture3 : register( t2 );
Texture2D Texture4 : register( t3 );
Texture2D Texture5 : register( t4 );
Texture2D Texture6 : register( t5 );
Texture2D Texture7 : register( t6 );
Texture2D Texture8 : register( t7 );

TextureCube CubeTexture1 : register( t8 );
TextureCube CubeTexture2 : register( t9 );
TextureCube CubeTexture3 : register( t10 );
TextureCube CubeTexture4 : register( t11 );
TextureCube CubeTexture5 : register( t12 );
TextureCube CubeTexture6 : register( t13 );
TextureCube CubeTexture7 : register( t14 );
TextureCube CubeTexture8 : register( t15 );

SamplerState TexSampler : register( s0 );

static const float PI = 3.14159265f;

cbuffer MaterialVars : register (b0)
{
    float4 MaterialAmbient;
    float4 MaterialDiffuse;
    float4 MaterialSpecular;
    float4 MaterialEmissive;
    float MaterialSpecularPower;
};

cbuffer LightVars : register (b1)
{
    float4 AmbientLight;
    float4 LightColor[4];
    float4 LightAttenuation[4];
    float3 LightDirection[4];
    float LightSpecularIntensity[4];
    uint IsPointLight[4];
    uint ActiveLights;
}

cbuffer ObjectVars : register(b2)
{
    float4x4 LocalToWorld4x4;
    float4x4 LocalToProjected4x4;
    float4x4 WorldToLocal4x4;
    float4x4 WorldToView4x4;
    float4x4 UVTransform4x4;
    float3 EyePosition;
};

cbuffer MiscVars : register(b3)
{
    float ViewportWidth;
    float ViewportHeight;
    float Time;
};

struct A2V
{
    float4 pos : POSITION0;
    float3 normal : NORMAL0;
    float4 tangent : TANGENT0;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};

struct V2P
{
    float4 pos : SV_POSITION;
    float4 diffuse : COLOR;
    float2 uv : TEXCOORD0;
    float3 worldNorm : TEXCOORD1;
    float3 worldPos : TEXCOORD2;
    float3 toEye : TEXCOORD3;
    float4 tangent : TEXCOORD4;
    float3 normal : TEXCOORD5;
};

struct P2F
{
    float4 fragment : SV_Target;
};

//
// desaturate
//
float3 Desaturate(
    float3 color, 
    float3 luminance, 
    float percent
    )
{
    float3 desatColor = dot(color, luminance);
    return lerp(color, desatColor, percent);
}

//
// fresnel falloff
//
float Fresnel(
    float3 surfaceNormal,
    float3 toEye,
    float exp
    )
{
    float x = 1 - saturate(dot(surfaceNormal, toEye));
    return pow(x, exp);
}

//
// panning offset
//
float2 PanningOffset(
    float2 sourceUV,
    float time,
    float speedX,
    float speedY
    )
{
    float2 uvOffset = float2(time, time) * float2(speedX, speedY);

    return sourceUV + uvOffset;
}

//
// parallax offset
//
float2 ParallaxOffset(
    float2 sourceUV,
    float height,
    float depthScale,
    float depthPlane,
    float3 tangentCameraVector
    )
{
    float bias = -(depthScale * depthPlane);
    float heightAdj = (depthScale * height) + bias;

    return sourceUV + (tangentCameraVector.xy * heightAdj);
}

//
// rotate offset
//
float2 RotateOffset(
    float2 sourceUV,
    float time,
    float centerX,
    float centerY,
    float speed
    )
{
    float2 ray = sourceUV - float2(centerX, centerY);
    float theta = time * speed;

    float cosTheta = cos(theta);
    float sinTheta = sin(theta);

    float2x2 rotMatrix = float2x2(float2(cosTheta, -sinTheta), float2(sinTheta, cosTheta));

    return mul(rotMatrix, ray) + float2(centerX, centerY);
}

//
// lambert lighting function
//
float3 LambertLighting(
    float3 lightNormal,
    float3 surfaceNormal,
    float3 materialAmbient,
    float3 lightAmbient,
    float3 lightColor,
    float3 pixelColor
    )
{
    // compute amount of contribution per light
    float diffuseAmount = saturate(dot(lightNormal, surfaceNormal));
    float3 diffuse = diffuseAmount * lightColor * pixelColor;

    // combine ambient with diffuse
    return saturate((materialAmbient * lightAmbient) + diffuse);
}

//
// specular contribution function
//
float3 SpecularContribution(
    float3 toEye,
    float3 lightNormal,
    float3 surfaceNormal,
    float3 materialSpecularColor,
    float materialSpecularPower,
    float lightSpecularIntensity,
    float3 lightColor
    )
{
    // compute specular contribution
    float3 vHalf = normalize(lightNormal + toEye);
    float specularAmount = saturate(dot(surfaceNormal, vHalf));
    specularAmount = pow(specularAmount, max(materialSpecularPower,0.0001f)) * lightSpecularIntensity;
    float3 specular = materialSpecularColor * lightColor * specularAmount;
    
    return specular;
}
//
// combines a float3 RGB value with an alpha value into a float4
//
float4 CombineRGBWithAlpha(float3 rgb, float a) 
{ 
    return float4(rgb.r, rgb.g, rgb.b, a); 
}
//
// a configurable "S" shaped function. a changes height, b horizontal offset, and c for width
//
float Sigmoid(float x, float a, float b, float c) {
	return a / (1 + pow(100, -((x - b) / c)));
}
//
// fade one color into another
//
float3 Transition(float3 color1, float3 color2, float ratio) {
	return color1*ratio + color2*(1-ratio);
}
P2F main(V2P pixel)
{
    P2F result;

    // we need to normalize incoming vectors
    float3 surfaceNormal = normalize(pixel.normal);
    float3 surfaceTangent = normalize(pixel.tangent.xyz);
    //float3 worldNormal = normalize(pixel.worldNorm);
	float3 worldNormal = float3(0.0, 1.0, 0.0);
    float3 toEyeVector = normalize(pixel.toEye);

    // construct tangent matrix
    float3x3 localToTangent = transpose(float3x3(surfaceTangent, cross(surfaceNormal, surfaceTangent) * pixel.tangent.w, surfaceNormal));
    float3x3 worldToTangent = mul((float3x3)WorldToLocal4x4, localToTangent);

    // transform some vectors into tangent space
    float3 tangentLightDir = normalize(mul(LightDirection[0], worldToTangent));
    float3 tangentToEyeVec = normalize(mul(toEyeVector, worldToTangent));

   /* float3 local0 = worldNormal;
    float3 local1 = normalize(mul(local0, worldToTangent));
    float local3 = float4(pixel.worldPos.r, pixel.worldPos.g, pixel.worldPos.b, 0).g;
    float local5 = distance(local3, float4(float3(0.000000f,-64.000000f,0.000000f).r, float3(0.000000f,-64.000000f,0.000000f).g, float3(0.000000f,-64.000000f,0.000000f).b, 0));
    float3 local12 = (local5 >= 64.000000f ? (local5 > 64.000000f ? Texture2.Sample(TexSampler, pixel.uv).rgb : Texture1.Sample(TexSampler, pixel.uv).rgb) : Texture1.Sample(TexSampler, pixel.uv).rgb);
    float3 local13 = (local5 >= 72.000000f ? (local5 > 72.000000f ? float4(1.000000f,1.000000f,1.000000f,1.000000f).rgb : local12) : local12);
    float3 local14 = LambertLighting(tangentLightDir, local1, MaterialAmbient.rgb, AmbientLight.rgb, LightColor[0].rgb, local13);*/
	float angle = acos((dot(surfaceNormal, worldNormal)) / (length(surfaceNormal)*length(worldNormal)));
	float dirtStrength = (PI / 6.0 - angle)/(PI/24.0);
	dirtStrength = clamp(dirtStrength, 0, 1.0);
	// merge textures based on slope
	float3 color;
	float landRatio = clamp(pixel.worldPos.y, 0.0, 1.0);
	float snowRatio = clamp(pixel.worldPos.y - 64.0, 0.0, 1.0);
	// elevation based texturing
	if (pixel.worldPos.y >= 16) {
		// highlands
		color = Transition(Texture4.Sample(TexSampler, pixel.uv).rgb, Texture2.Sample(TexSampler, pixel.uv).rgb, snowRatio);
	}
	else if (pixel.worldPos.y >= 0) {
		// lowlands
		color = Transition(Texture2.Sample(TexSampler, pixel.uv).rgb, Texture1.Sample(TexSampler, pixel.uv).rgb, landRatio);
	}
	else {
		// uderwater
		color = Transition(Texture1.Sample(TexSampler, pixel.uv).rgb, Texture3.Sample(TexSampler, pixel.uv).rgb, dirtStrength);
	}
	// slope based texturing
	color = Transition(color, Texture3.Sample(TexSampler, pixel.uv).rgb, dirtStrength);
	// lighting
	color = LambertLighting(tangentLightDir, mul(worldNormal, worldToTangent), MaterialAmbient.rgb, AmbientLight.rgb, LightColor[0].rgb, color);
	color *= Sigmoid(distance(EyePosition, pixel.worldPos), 0.25, 128.0, 256.0) + 1;
	color.b += Sigmoid(distance(EyePosition, pixel.worldPos), 0.25, 128.0, 256.0);
	result.fragment = CombineRGBWithAlpha(color, 1.0);

    if (result.fragment.a == 0.0f) discard;

    return result;
}

