#include "pch.h"
#include "Renderer.h"
#include "DataTypes.h"
#include "Mesh.h"
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>
#include "Camera.h"
#include "Texture.h"

namespace dae {

	Renderer::Renderer(SDL_Window* pWindow)
		: m_pWindow(pWindow)
	{
		//Initialize
		SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

		m_pCamera = new Camera(Vector3(0.0f, 0.0f, -50.0f), 45.0f);
		m_pCamera->Initialize(45.0f, Vector3(0.0f, 0.0f, -10.0f), (float(m_Width) / float(m_Height)));

		//Initialize DirectX pipeline
		const HRESULT result = InitializeDirectX();
		if (result == S_OK)
		{
			m_IsInitialized = true;
			std::cout << GREEN_TEXT("DirectX is initialized and ready!\n");

			/*const std::vector<Vertex> vertices
			{
				{ {-3.0f, 3.0f, 2.0f}, { 0.0f, 0.0f} },
				{ {0.0f, 3.0f, 2.0f},  { 0.5f, 0.0f} },
				{ {3.0f, 3.0f, 2.0f},  { 1.0f, 0.0f} },
				{ {-3.0f, 0.0f, 2.0f}, { 0.0f, 0.5f} },
				{ {0.0f, 0.0f, 2.0f},  { 0.5f, 0.5f} },
				{ {3.0f, 0.0f, 2.0f},  { 1.0f, 0.5f} },
				{ {-3.0f, -3.0f, 2.0f},{ 0.0f, 1.0f} },
				{ {0.0f, -3.0f, 2.0f}, { 0.5f, 1.0f} },
				{ {3.0f, -3.0f, 2.0f}, { 1.0f, 1.0f} }
			};

			const std::vector<uint32_t> indices { 3, 0, 4, 1, 5, 2, 6, 3, 7, 4, 8, 5 };*/

			Mesh* pMesh{ new Mesh(m_pDevice, L"Resources/PosCol3D.fx", "Resources/vehicle.obj")};
			m_pDiffuseMap = new Texture(m_pDevice, "Resources/vehicle_diffuse.png");
			pMesh->SetDiffuseMap(m_pDiffuseMap);
			m_Meshes.push_back(pMesh);
		}
		else
		{
			std::cout << "DirectX initialization failed!\n";
		}
	}

	Renderer::~Renderer()
	{
		delete m_pCamera;

		for (auto pMesh : m_Meshes)
		{
			delete pMesh;
		}

		if (m_pDeviceContext)
		{
			m_pDeviceContext->ClearState();
			m_pDeviceContext->Flush();
			m_pDeviceContext->Release();
		}

		SAFE_RELEASE(m_pDevice);
		SAFE_RELEASE(m_pSwapChain);
		SAFE_RELEASE(m_pDepthStencilBuffer);
		SAFE_RELEASE(m_pDepthStencilView);
		SAFE_RELEASE(m_pRenderTargetBuffer);
	}

	void Renderer::Update(const Timer* pTimer)
	{
		m_pCamera->Update(pTimer);

		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_KEYUP:
				if (e.key.keysym.scancode == SDL_SCANCODE_F4)
				{
					for (const auto& mesh : m_Meshes)
					{
						mesh->CycleFilteringMethods();
					}
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F5)
				{
					m_EnableRotating = !m_EnableRotating;
				}
				break;
			}
		}

		if (m_EnableRotating)
		{
			for (const auto& mesh : m_Meshes)
			{
				mesh->RotateY(m_MeshRotationSpeed * TO_RADIANS * pTimer->GetElapsed());
			}
		}

		for (const auto& mesh : m_Meshes)
		{
			mesh->SetMatrix(*m_pCamera);
		}
	}

	void Renderer::Render() const
	{
		if (!m_IsInitialized)
			return;

		constexpr float color[4]{ 0.0f, 0.0f, 0.3f, 1.0f };
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		for (const auto& mesh : m_Meshes)
		{
			mesh->Draw(m_pDeviceContext);
		}
		
		m_pSwapChain->Present(0, 0);
	}

	HRESULT Renderer::InitializeDirectX()
	{
		D3D_FEATURE_LEVEL featureLevel{ D3D_FEATURE_LEVEL_11_1 };
		uint32_t createDeviceFlags{ 0 };

#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		HRESULT result{ D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, &featureLevel,
					   1, D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pDeviceContext) };
		if (FAILED(result))
		{
			std::cerr << "Error: " << std::hex << result << std::endl;
			return result;
		}

		IDXGIFactory1* pDxgiFactory{};
		result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&pDxgiFactory));
		if (FAILED(result))
		{
			std::cerr << "Error: " << std::hex << result << std::endl;
			return result;
		}

		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChainDesc.BufferDesc.Width = m_Width;
		swapChainDesc.BufferDesc.Height = m_Height;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.Windowed = TRUE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

		SDL_SysWMinfo sysWMInfo{};
		SDL_GetVersion(&sysWMInfo.version);
		SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
		swapChainDesc.OutputWindow = sysWMInfo.info.win.window;

		if (m_pDevice != nullptr)
			result = pDxgiFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);

		if (FAILED(result))
		{
			std::cerr << "Error: " << std::hex << result << std::endl;
			return result;
		}

		D3D11_TEXTURE2D_DESC depthStencilDesc{};
		depthStencilDesc.Width = m_Width;
		depthStencilDesc.Height = m_Height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = depthStencilDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		result = m_pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStencilBuffer);
		if (FAILED(result))
		{
			std::cerr << "Error: " << std::hex << result << std::endl;
			return result;
		}

		result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
		if (FAILED(result))
		{
			std::cerr << "Error: " << std::hex << result << std::endl;
			return result;
		}

		result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
		if (FAILED(result))
		{
			std::cerr << "Error: " << std::hex << result << std::endl;
			return result;
		}

		result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, nullptr, &m_pRenderTargetView);
		if (FAILED(result))
		{
			std::cerr << "Error: " << std::hex << result << std::endl;
			return result;
		}

		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

		D3D11_VIEWPORT viewport{};
		viewport.Width = static_cast<float>(m_Width);
		viewport.Height = static_cast<float>(m_Height);
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		m_pDeviceContext->RSSetViewports(1, &viewport);

		return result;
	}
}
