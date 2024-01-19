#include "pch.h"
#include "Effect.h"
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>
#include "Matrix.h"

Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
	m_pEffect = LoadEffect(pDevice, assetFile);

	if (m_pEffect != nullptr)
	{
		m_pTechnique = m_pEffect->GetTechniqueByName("DefaultTechnique");
	}

	if (!m_pTechnique->IsValid())
	{
		std::wcout << L"Technique not valid.\n";
	}

	m_pWorldViewProjection = m_pEffect->GetVariableByName("g_WorldViewProjection")->AsMatrix();

	if (!m_pWorldViewProjection->IsValid())
	{
		std::wcout << L"m_pWorldViewProjection is not valid.\n";
	}
}

Effect::~Effect()
{

}

bool Effect::CreateInputLayout(ID3D11Device* pDevice, const D3D11_INPUT_ELEMENT_DESC* vertexDesc, uint32_t elementsCount)
{
	D3DX11_PASS_DESC passDesc{};
	m_pTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

	HRESULT result{ pDevice->CreateInputLayout(vertexDesc, elementsCount, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_pInputLayout) };

	if (FAILED(result))
	{
		std::cerr << "Error: " << std::hex << result << std::endl;
		return false;
	}

	return true;
}

ID3DX11Effect* Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring & assetFile)
{
	HRESULT result;
	ID3D10Blob* pErrorBlob{ nullptr };
	ID3DX11Effect* pEffect;

	DWORD shaderFlags = 0;

#if defined (DEBUG) || defined (_DEBUG)
	shaderFlags = D3DCOMPILE_DEBUG;
	shaderFlags = D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	result = D3DX11CompileEffectFromFile(assetFile.c_str(), nullptr, nullptr, shaderFlags, 0, pDevice, &pEffect, &pErrorBlob);

	if (FAILED(result))
	{
		if (pErrorBlob != nullptr)
		{
			const char* pErrors = static_cast<char*>(pErrorBlob->GetBufferPointer());
			std::wstringstream ss;

			for (unsigned int i{ 0 }; i < pErrorBlob->GetBufferSize(); i++)
				ss << pErrors[i];

			OutputDebugStringW(ss.str().c_str());

			pErrorBlob->Release();
			pErrorBlob = nullptr;

			std::wcout << ss.str() << std::endl;
		}
		else
		{
			std::wstringstream ss;
			ss << "EffectLoader: Failed to CreateEffectFromFile!\nPath: " << assetFile;
			std::wcout << ss.str() << std::endl;
			return nullptr;
		}
	}

	return pEffect;
}

void Effect::SetMatrix(const dae::Matrix& matrix)
{
	m_pWorldViewProjection->SetMatrix((float*)&matrix);
}
