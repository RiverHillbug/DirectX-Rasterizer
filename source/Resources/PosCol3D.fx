
//Global Variables
float3 g_CameraPosition : CAMERA;
float4x4 g_WorldViewProjection : WorldViewProjection;
float4x4 g_WorldMatrix : WORLD;
Texture2D g_DiffuseMap : DiffuseMap;
Texture2D g_NormalMap : NormalMap;
Texture2D g_SpecularMap : SpecularMap;
Texture2D g_GlossinessMap : GlossinessMap;

bool g_NormalMapEnabled = true;

float3 g_LightDirection = normalize(float3(0.577f, -0.577f, 0.577f));
float g_LightIntensity = 7.0f;
float g_Shininess = 25.0f;

float PI = float(3.14159265f);

SamplerState g_SampleStatePoint : SampleState
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap; // or Mirror, Clamp, Border
    AddressV = Wrap; // or Mirror, Clamp, Border
};

SamplerState g_SampleStateLinear : SampleState
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState g_SampleStateAnisotropic : SampleState
{
    Filter = ANISOTROPIC;
    AddressU = Wrap;
    AddressV = Wrap;
};

RasterizerState g_RasterizerState
{
	CullMode = none;
	FrontCounterClockwise = false;
};

//Input/Output Structs
struct VS_INPUT
{
	float3 position : POSITION;
	float2 UV : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION0;
	float4 worldPosition : TEXCOORD0;
	float2 UV : TEXCOORD1;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};

//Vertex Shader
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.position = float4(input.position, 1.0f);
	output.position = mul(output.position, g_WorldViewProjection);
	output.UV = input.UV;
	output.normal = mul(normalize(input.normal), (float3x3)g_WorldMatrix);
	output.tangent = mul(normalize(input.tangent), (float3x3)g_WorldMatrix);
	return output;
}

float4 Lambert(float kd, float4 cd)
{
	return float4((cd * kd) / PI);
}

float Phong(float ks, float exp, float3 light, float3 viewDirection, float3 normal)
{
	float3 reflection = reflect(light, normal);
	float cos = saturate(dot(reflection, viewDirection));
	float phongSpecularReflection;

	if (cos > 0)
	{
		phongSpecularReflection = ks * pow(cos, exp);
	}

	return phongSpecularReflection;
}

//Pixel Shader
float4 PS_Phong(VS_OUTPUT input, SamplerState samplerState) : SV_TARGET
{
	const float3 binormal = cross(input.normal, input.tangent);
	const float4 zeroVector = float4(0.0f, 0.0f, 0.0f, 1.0f);
	const float4x4 tangentSpaceAxis = float4x4( float4(input.tangent, 0.0f), float4(binormal, 0.0f), float4(input.normal, 0.0f), zeroVector );

	float3 sampledNormal = (2.0f * g_NormalMap.Sample(samplerState, input.UV).rgb - float3(1.0f, 1.0f, 1.0f));
	float4 normal;
	if(g_NormalMapEnabled)
	{
		normal = mul(float4(sampledNormal, 0.0f), tangentSpaceAxis);
	}
	else
	{
		normal = float4(input.normal, 0.0f);
	}

	float3 invertedViewDirection = normalize(g_CameraPosition - input.worldPosition.xyz);

	//Same math as in raytracer pretty much
	float observedArea = saturate(dot(normal, -g_LightDirection));
	float4 lambert = Lambert(1.0f, g_DiffuseMap.Sample(samplerState, input.UV));	//kd = 1, cd = sample
	float glossiness = g_Shininess * g_GlossinessMap.Sample(samplerState, input.UV).r;	//Every component in the glossiness map is the same as it is a greyscale map. Can just use r, g or b value.
	float4 phongSpecular = g_SpecularMap.Sample(samplerState, input.UV) * Phong(1.0f, glossiness, -g_LightDirection, invertedViewDirection, normal);

	 return float4((g_LightIntensity * lambert + phongSpecular) * observedArea);
}

float4 PS_Point(VS_OUTPUT input) : SV_TARGET
{
	//float4 diffuse = g_DiffuseMap.Sample(g_SampleStatePoint, input.UV);
	return PS_Phong(input, g_SampleStatePoint);
}

float4 PS_Linear(VS_OUTPUT input) : SV_TARGET
{
	//float4 diffuse = g_DiffuseMap.Sample(g_SampleStateLinear, input.UV);
	return PS_Phong(input, g_SampleStateLinear);
}

float4 PS_Anisotropic(VS_OUTPUT input) : SV_TARGET
{
	//float4 diffuse = g_DiffuseMap.Sample(g_SampleStateAnisotropic, input.UV);
	return PS_Phong(input, g_SampleStateAnisotropic);
}


//Techniques
technique11 PointFilteringTechnique
{
	pass p0
	{
		SetRasterizerState(g_RasterizerState);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Point()));
	}
}

technique11 LinearFilteringTechnique
{
	pass p0
	{
		SetRasterizerState(g_RasterizerState);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Linear()));
	}
}

technique11 AnisotropicFilteringTechnique
{
	pass p0
	{
		SetRasterizerState(g_RasterizerState);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Anisotropic()));
	}
}

/*technique11 DefaultTechnique
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Point()));
	}
}*/