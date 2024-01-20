
#pragma once

namespace dae
{
	class Texture final
	{
	public:
		Texture(ID3D11Device* pDevice, const std::string& path);
		~Texture();

		Texture(const Texture& other) = delete;
		Texture& operator=(const Texture& other) = delete;
		Texture(Texture&& other) = delete;
		Texture& operator=(Texture&& other) = delete;

		inline ID3D11ShaderResourceView* GetShaderResourceView() const { return m_pShaderResourceView; }

	private:
		ID3D11ShaderResourceView* m_pShaderResourceView{};
		ID3D11Texture2D* m_pTexture{};
	};
};
