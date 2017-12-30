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

P2F main(V2P pixel)
{
    P2F result;

    // we need to normalize incoming vectors
    float3 surfaceNormal = normalize(pixel.normal);
    float3 surfaceTangent = normalize(pixel.tangent.xyz);
    float3 worldNormal = normalize(pixel.worldNorm);
    float3 toEyeVector = normalize(pixel.toEye);

    // construct tangent matrix
    float3x3 localToTangent = transpose(float3x3(surfaceTangent, cross(surfaceNormal, surfaceTangent) * pixel.tangent.w, surfaceNormal));
    float3x3 worldToTangent = mul((float3x3)WorldToLocal4x4, localToTangent);

    // transform some vectors into tangent space
    float3 tangentLightDir = normalize(mul(LightDirection[0], worldToTangent));
    float3 tangentToEyeVec = normalize(mul(toEyeVector, worldToTangent));

    // BEGIN GENERATED CODE
    result.fragment = CombineRGBWithAlpha(pixel.diffuse.rgb, pixel.diffuse.a);
    // END GENERATED CODE

    if (result.fragment.a == 0.0f) discard;

    return result;
}

