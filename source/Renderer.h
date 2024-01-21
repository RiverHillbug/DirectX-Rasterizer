#pragma once

namespace dae
{
	class Renderer final
	{
	public:
		Renderer(struct SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(const Timer* pTimer);
		void Render() const;
		inline void ToggleRotation() { m_EnableRotating = !m_EnableRotating; }
		void CycleFilteringMethods();
		void ToggleNormalMap();
		void StartFastRotation();
		void StopFastRotation();

	private:
		struct SDL_Window* m_pWindow{};
		struct Camera* m_pCamera{ nullptr };
		const float m_MeshRotationSpeed{ 30.0f };
		const float m_MeshFastRotationSpeed{ 60.0f };

		int m_Width{};
		int m_Height{};

		bool m_IsInitialized{ false };

		class Effect* m_pEffect{ nullptr };

		std::vector<class Mesh*> m_Meshes{};
		
		float m_CurrentRotationSpeed{ m_MeshRotationSpeed };
		bool m_EnableRotating{ true };

		//DIRECTX
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;
		IDXGISwapChain* m_pSwapChain;
		ID3D11Texture2D* m_pDepthStencilBuffer;
		ID3D11DepthStencilView* m_pDepthStencilView;
		ID3D11Resource* m_pRenderTargetBuffer;
		ID3D11RenderTargetView* m_pRenderTargetView;

		HRESULT InitializeDirectX();
		//...
	};
}
