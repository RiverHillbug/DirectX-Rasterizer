#include "pch.h"
#include "Texture.h"

namespace dae {

	dae::Texture::Texture(ID3D11Device* pDevice, const std::string& path)
	{
		// Load image at specified path
		SDL_Surface* pSurface = IMG_Load(path.c_str());
		if (pSurface == nullptr)
		{
			std::cerr << "Texture: error when calling IMG_Load: " << SDL_GetError() << std::endl;
		}

		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
		D3D11_TEXTURE2D_DESC textureDesc{};
		textureDesc.Width = pSurface->w;
		textureDesc.Height = pSurface->h;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = format;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = pSurface->pixels;
		initData.SysMemPitch = static_cast<UINT>(pSurface->pitch);
		initData.SysMemSlicePitch = static_cast<UINT>(pSurface->h * pSurface->pitch);

		HRESULT result = pDevice->CreateTexture2D(&textureDesc, &initData, &m_pTexture);

		if (FAILED(result))
		{
			std::cerr << "Error: " << std::hex << result << std::endl;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
		SRVDesc.Format = format;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;

		if (m_pTexture)
			result = pDevice->CreateShaderResourceView(m_pTexture, &SRVDesc, &m_pShaderResourceView);

		if (FAILED(result))
		{
			std::cerr << "Error: " << std::hex << result << std::endl;
		}

		SDL_FreeSurface(pSurface);
	}

	dae::Texture::~Texture()
	{
		SAFE_RELEASE(m_pTexture);
		SAFE_RELEASE(m_pShaderResourceView);
	}
}