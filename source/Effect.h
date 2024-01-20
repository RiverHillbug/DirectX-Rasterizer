
#pragma once

namespace dae
{
	class Texture;
}

class Effect
{
public:
	Effect(struct ID3D11Device* pDevice, const std::wstring& assetFile);
	~Effect();

	bool CreateInputLayout(struct ID3D11Device* pDevice, const struct D3D11_INPUT_ELEMENT_DESC* vertexDesc, uint32_t elementsCount);
	static struct ID3DX11Effect* LoadEffect(struct ID3D11Device* pDevice, const std::wstring& assetFile);
	inline struct ID3DX11Effect* GetEffect() const { return m_pEffect; }
	inline struct ID3DX11EffectTechnique* GetTechnique() const { return m_pTechnique; }
	inline struct ID3D11InputLayout* GetInputLayout() const{ return m_pInputLayout; }
	void SetMatrix(const struct dae::Matrix& matrix);
	void SetDiffuseMap(const dae::Texture* pDiffuseMap);

	void CycleFilteringMethods();

private:

	enum class FilteringMethod
	{
		Point,
		Linear,
		Anisotropic,
	};

	FilteringMethod m_FilteringMethod{ FilteringMethod::Point };

	const LPCSTR m_PointFilteringMethodName{ "PointFilteringTechnique" };
	const LPCSTR m_LinearFilteringMethodName{ "LinearFilteringTechnique" };
	const LPCSTR m_AnisotropicFilteringMethodName{ "AnisotropicFilteringTechnique" };

	LPCSTR m_FilteringMethodName{ m_PointFilteringMethodName };

	struct ID3DX11Effect* m_pEffect;
	struct ID3DX11EffectTechnique* m_pTechnique;
	struct ID3D11InputLayout* m_pInputLayout{};
	struct ID3DX11EffectMatrixVariable* m_pWorldViewProjection;
	struct ID3DX11EffectShaderResourceVariable* m_pDiffuseMap;
};
