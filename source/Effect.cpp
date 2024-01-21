#include "pch.h"
#include "Effect.h"
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>
#include "Matrix.h"
#include "Texture.h"

Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
	m_pEffect = LoadEffect(pDevice, assetFile);

	if (m_pEffect != nullptr)
	{
		m_pTechnique = m_pEffect->GetTechniqueByName(m_FilteringMethodName);
	}

	if (!m_pTechnique->IsValid())
	{
		std::wcout << L"Technique not valid.\n";
	}

	m_pWorldViewProjectionMatrix = m_pEffect->GetVariableByName("g_WorldViewProjection")->AsMatrix();
	if (!m_pWorldViewProjectionMatrix->IsValid())
	{
		std::wcout << L"m_pWorldViewProjectionMatrix is not valid.\n";
	}

	m_pWorldMatrix = m_pEffect->GetVariableByName("g_WorldMatrix")->AsMatrix();
	if (!m_pWorldMatrix->IsValid())
	{
		std::wcout << L"m_pWorldMatrix is not valid.\n";
	}

	m_pDiffuseMap = m_pEffect->GetVariableByName("g_DiffuseMap")->AsShaderResource();
	if (!m_pDiffuseMap->IsValid())
	{
		std::wcout << L"g_DiffuseMap is not valid.\n";
	}

	m_pNormalMap = m_pEffect->GetVariableByName("g_NormalMap")->AsShaderResource();
	if (!m_pNormalMap->IsValid())
	{
		std::wcout << L"g_NormalMap is not valid.\n";
	}

	m_pSpecularMap = m_pEffect->GetVariableByName("g_SpecularMap")->AsShaderResource();
	if (!m_pSpecularMap->IsValid())
	{
		std::wcout << L"g_SpecularMap is not valid.\n";
	}

	m_pGlossinessMap = m_pEffect->GetVariableByName("g_GlossinessMap")->AsShaderResource();
	if (!m_pGlossinessMap->IsValid())
	{
		std::wcout << L"g_GlossinessMap is not valid.\n";
	}

	m_pNormalMapEnabled = m_pEffect->GetVariableByName("g_NormalMapEnabled")->AsScalar();
	if (!m_pNormalMapEnabled->IsValid())
	{
		std::wcout << L"g_NormalMapEnabled is not valid.\n";
	}
}

Effect::~Effect()
{
	if (m_pTechnique)
		m_pTechnique->Release();

	if (m_pEffect)
		m_pEffect->Release();
}

bool Effect::CreateInputLayout(ID3D11Device* pDevice, const D3D11_INPUT_ELEMENT_DESC* vertexDesc, uint32_t elementsCount)
{
	D3DX11_PASS_DESC passDesc{};
	m_pTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

	const HRESULT result{ pDevice->CreateInputLayout(vertexDesc, elementsCount, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_pInputLayout) };

	if (FAILED(result))
	{
		std::cerr << "Error: " << std::hex << result << std::endl;
		return false;
	}

	return true;
}

ID3DX11Effect* Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring & assetFile)
{
	ID3D10Blob* pErrorBlob{ nullptr };
	ID3DX11Effect* pEffect{ nullptr };

	DWORD shaderFlags = 0;

#if defined (DEBUG) || defined (_DEBUG)
	shaderFlags = D3DCOMPILE_DEBUG;
	shaderFlags = D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	const HRESULT result = D3DX11CompileEffectFromFile(assetFile.c_str(), nullptr, nullptr, shaderFlags, 0, pDevice, &pEffect, &pErrorBlob);
	if (FAILED(result))
	{
		pEffect = nullptr;

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
		}
	}

	return pEffect;
}

void Effect::SetMatrix(const dae::Matrix& matrix)
{
	m_pWorldViewProjectionMatrix->SetMatrix((float*)&matrix);
}

void Effect::SetWorldMatrix(const dae::Matrix& matrix)
{
	m_pWorldMatrix->SetMatrix((float*)&matrix);
}

void Effect::SetDiffuseMap(const dae::Texture* pDiffuseMap)
{
	if (m_pDiffuseMap->IsValid())
		m_pDiffuseMap->SetResource(pDiffuseMap->GetShaderResourceView());
}

void Effect::SetNormalMap(const dae::Texture* pNormalMap)
{
	if (m_pNormalMap->IsValid())
		m_pNormalMap->SetResource(pNormalMap->GetShaderResourceView());
}

void Effect::SetSpecularMap(const dae::Texture* pSpecularMap)
{
	if (m_pSpecularMap->IsValid())
		m_pSpecularMap->SetResource(pSpecularMap->GetShaderResourceView());
}

void Effect::SetGlossinessMap(const dae::Texture* pGlossinessMap)
{
	if (m_pGlossinessMap->IsValid())
		m_pGlossinessMap->SetResource(pGlossinessMap->GetShaderResourceView());
}

void Effect::CycleFilteringMethods()
{
	switch (m_FilteringMethod)
	{
	case Effect::FilteringMethod::Point:
		m_FilteringMethod = FilteringMethod::Linear;
		m_FilteringMethodName = m_LinearFilteringMethodName;
		std::cout << YELLOW_TEXT("Filtering Method: Linear\n");
		break;

	case Effect::FilteringMethod::Linear:
		m_FilteringMethod = FilteringMethod::Anisotropic;
		m_FilteringMethodName = m_AnisotropicFilteringMethodName;
		std::cout << YELLOW_TEXT("Filtering Method: Anisotropic\n");
		break;

	case Effect::FilteringMethod::Anisotropic:
		m_FilteringMethod = FilteringMethod::Point;
		m_FilteringMethodName = m_PointFilteringMethodName;
		std::cout << YELLOW_TEXT("Filtering Method: Point\n");
		break;
	}

	m_pTechnique = m_pEffect->GetTechniqueByName(m_FilteringMethodName);
}

void Effect::ToggleNormalMap()
{
	m_NormalMapEnabledValue = !m_NormalMapEnabledValue;
	m_pNormalMapEnabled->SetBool(&m_NormalMapEnabledValue);

	std::cout << YELLOW_TEXT("Normal map is ") << (m_NormalMapEnabledValue ? GREEN_TEXT("enabled.\n") : RED_TEXT("disabled.\n"));
}
