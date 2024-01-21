#include "pch.h"
#include "DataTypes.h"
#include "Effect.h"
#include "Mesh.h"
#include "Camera.h"
#include "Texture.h"
#include "Utils.h"

namespace dae {

	Mesh::Mesh(ID3D11Device* pDevice, const std::wstring& effectFile, std::vector<Vertex> newVertices, std::vector<uint32_t> newIndices)
		: m_Vertices{ newVertices }
		, m_Indices{ newIndices }
		, m_pEffect{ new Effect(pDevice, effectFile) }
	{
		static constexpr uint32_t elementsCount{ 4 };
		D3D11_INPUT_ELEMENT_DESC vertexDesc[elementsCount]{};

		vertexDesc[0].SemanticName = "POSITION";
		vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[0].AlignedByteOffset = 0;
		vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[1].SemanticName = "TEXCOORD";
		vertexDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		vertexDesc[1].AlignedByteOffset = 12;
		vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[2].SemanticName = "NORMAL";
		vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[2].AlignedByteOffset = 20;
		vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[3].SemanticName = "TANGENT";
		vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[3].AlignedByteOffset = 32;
		vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		m_pEffect->CreateInputLayout(pDevice, vertexDesc, elementsCount);

		D3D11_BUFFER_DESC bufferDesc{};
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.ByteWidth = sizeof(Vertex) * static_cast<uint32_t>(m_Vertices.size());
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA	initData{};
		initData.pSysMem = m_Vertices.data();

		HRESULT result = pDevice->CreateBuffer(&bufferDesc, &initData, &m_pVertexBuffer);
		if (FAILED(result))
		{
			std::cerr << "Error: " << std::hex << result << std::endl;
			return;
		}

		m_IndicesCount = static_cast<uint32_t>(m_Indices.size());

		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.ByteWidth = sizeof(uint32_t) * m_IndicesCount;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		initData.pSysMem = m_Indices.data();

		result = pDevice->CreateBuffer(&bufferDesc, &initData, &m_pIndexBuffer);
		if (FAILED(result))
		{
			std::cerr << "Error: " << std::hex << result << std::endl;
			return;
		}
	}

	Mesh::Mesh(struct ID3D11Device* pDevice, const std::wstring& effectFile, const std::string& objFile)
		: m_pEffect{ new Effect(pDevice, effectFile) }
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		if (!Utils::ParseOBJ(objFile, vertices, indices, true))
		{
			std::cout << "Invalid file!\n";
			return;
		}

		static constexpr uint32_t elementsCount{ 4 };
		D3D11_INPUT_ELEMENT_DESC vertexDesc[elementsCount]{};

		vertexDesc[0].SemanticName = "POSITION";
		vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[0].AlignedByteOffset = 0;
		vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[1].SemanticName = "TEXCOORD";
		vertexDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		vertexDesc[1].AlignedByteOffset = 12;
		vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[2].SemanticName = "NORMAL";
		vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[2].AlignedByteOffset = 20;
		vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[3].SemanticName = "TANGENT";
		vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[3].AlignedByteOffset = 32;
		vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		m_pEffect->CreateInputLayout(pDevice, vertexDesc, elementsCount);

		D3D11_BUFFER_DESC bufferDesc{};
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.ByteWidth = sizeof(Vertex) * static_cast<uint32_t>(vertices.size());
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA	initData{};
		initData.pSysMem = vertices.data();

		HRESULT result = pDevice->CreateBuffer(&bufferDesc, &initData, &m_pVertexBuffer);
		if (FAILED(result))
		{
			std::cerr << "Error: " << std::hex << result << std::endl;
			return;
		}

		m_IndicesCount = static_cast<uint32_t>(indices.size());

		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.ByteWidth = sizeof(uint32_t) * m_IndicesCount;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		initData.pSysMem = indices.data();

		result = pDevice->CreateBuffer(&bufferDesc, &initData, &m_pIndexBuffer);
		if (FAILED(result))
		{
			std::cerr << "Error: " << std::hex << result << std::endl;
			return;
		}
	}

	Mesh::~Mesh()
	{
		delete m_pEffect;
		SAFE_RELEASE(m_pIndexBuffer);
		SAFE_RELEASE(m_pVertexBuffer);
	}

	void Mesh::Draw(ID3D11DeviceContext* pDeviceContext) const
	{
		//1. Set Primitive Topology
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//2. Set Input Layout
		pDeviceContext->IASetInputLayout(m_pEffect->GetInputLayout());

		//3. Set VertexBuffer
		constexpr UINT stride = sizeof(Vertex);
		constexpr UINT offset = 0;
		pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

		//4. Set IndexBuffer
		pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		//5. Draw
		D3DX11_TECHNIQUE_DESC techniqueDesc{};
		m_pEffect->GetTechnique()->GetDesc(&techniqueDesc);

		for (UINT p{ 0 }; p < techniqueDesc.Passes; ++p)
		{
			m_pEffect->GetTechnique()->GetPassByIndex(p)->Apply(0, pDeviceContext);
			pDeviceContext->DrawIndexed(m_IndicesCount, 0, 0);
		}
	}

	void Mesh::RotateX(float pitch)
	{
		m_RotationMatrix = Matrix::CreateRotationX(pitch) * m_RotationMatrix;
	}
	void Mesh::RotateY(float yaw)
	{
		m_RotationMatrix = Matrix::CreateRotationY(yaw) * m_RotationMatrix;
	}
	void Mesh::RotateZ(float roll)
	{
		m_RotationMatrix = Matrix::CreateRotationZ(roll) * m_RotationMatrix;
	}

	void Mesh::SetMatrix(const Camera& camera)
	{
		const Matrix worldMatrix{ m_ScaleMatrix *( m_RotationMatrix * m_TranslationMatrix) };
		const Matrix worldViewProjectionMatrix{ worldMatrix * (camera.viewMatrix * camera.projectionMatrix) };

		m_pEffect->SetMatrix(worldViewProjectionMatrix);
	}

	void Mesh::SetWorldMatrix()
	{
		const Matrix worldMatrix{ m_ScaleMatrix * (m_RotationMatrix * m_TranslationMatrix) };
		m_pEffect->SetWorldMatrix(worldMatrix);
	}

	void Mesh::SetDiffuseMap(const dae::Texture* pDiffuseMap)
	{
		m_pEffect->SetDiffuseMap(pDiffuseMap);
	}

	void Mesh::SetNormalMap(const class dae::Texture* pNormalMap)
	{
		m_pEffect->SetNormalMap(pNormalMap);
	}

	void Mesh::SetSpecularMap(const class dae::Texture* pSpecularMap)
	{
		m_pEffect->SetSpecularMap(pSpecularMap);
	}

	void Mesh::SetGlossinessMap(const class dae::Texture* pGlossinessMap)
	{
		m_pEffect->SetGlossinessMap(pGlossinessMap);
	}

	void Mesh::CycleFilteringMethods()
	{
		m_pEffect->CycleFilteringMethods();
	}

	void Mesh::ToggleNormalMap()
	{
		m_pEffect->ToggleNormalMap();
	}
}
