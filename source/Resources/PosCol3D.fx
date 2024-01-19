
//Input/Output Structs

float4x4 g_WorldViewProjection : WorldViewProjection;

struct VS_INPUT
{
	float3 position : POSITION;
	float3 color : COLOR; 
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float3 color : COLOR;
};

//Vertex Shader
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.position = float4(input.position, 1.0f);
	output.position = mul(g_WorldViewProjection, output.position);
	output.color = input.color;
	return output;
}

//Pixel Shader
float4 PS(VS_OUTPUT input) : SV_TARGET
{
	return float4(input.color, 1.0f);
}

//Technique
technique11 DefaultTechnique
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}