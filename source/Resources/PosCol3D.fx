
//Global Variables
float4x4 g_WorldViewProjection : WorldViewProjection;
Texture2D g_DiffuseMap : DiffuseMap;

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

//Input/Output Structs
struct VS_INPUT
{
	float3 position : POSITION;
	float2 UV : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 UV : TEXCOORD;
};

//Vertex Shader
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.position = float4(input.position, 1.0f);
	output.position = mul(g_WorldViewProjection, output.position);
	output.UV = input.UV;
	return output;
}

//Pixel Shader
float4 PS_Point(VS_OUTPUT input) : SV_TARGET
{
	float4 diffuse = g_DiffuseMap.Sample(g_SampleStatePoint, input.UV);
	return diffuse;
}

float4 PS_Linear(VS_OUTPUT input) : SV_TARGET
{
	float4 diffuse = g_DiffuseMap.Sample(g_SampleStateLinear, input.UV);
	return diffuse;
	//return float4(diffuse, 1.0f);
}

float4 PS_Anisotropic(VS_OUTPUT input) : SV_TARGET
{
	float4 diffuse = g_DiffuseMap.Sample(g_SampleStateAnisotropic, input.UV);
	return diffuse;
	//return float4(diffuse, 1.0f);
}

//Technique
technique11 PointFilteringTechnique
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Point()));
	}
}

technique11 LinearFilteringTechnique
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Linear()));
	}
}

technique11 AnisotropicFilteringTechnique
{
	pass p0
	{
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