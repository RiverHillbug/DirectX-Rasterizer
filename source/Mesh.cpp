#include "pch.h"
#include "DataTypes.h"
#include "Effect.h"
#include "Mesh.h"
#include "Camera.h"
#include "Texture.h"
#include "Utils.h"

namespace dae {

	Mesh::Mesh(ID3D11Device* pDevice, const std::wstring& effectFile, std::vector<Vertex>& newVertices, std::vector<uint32_t>& newIndices)
		: vertices{ newVertices }
		, indices{ newIndices }
	{
		m_pEffect = new Effect(pDevice, effectFile);

		static constexpr uint32_t elementsCount{ 2 };
		D3D11_INPUT_ELEMENT_DESC vertexDesc[elementsCount]{};

		vertexDesc[0].SemanticName = "POSITION";
		vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[0].AlignedByteOffset = 0;
		vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[1].SemanticName = "TEXCOORD";
		vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[1].AlignedByteOffset = 12;
		vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

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

	Mesh::Mesh(struct ID3D11Device* pDevice, const std::wstring& effectFile, const std::string& objFile)
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		if (!Utils::ParseOBJ(objFile, vertices, indices, true))
		{
			std::cout << "Invalid file!\n";
		}

		m_pEffect = new Effect(pDevice, effectFile);

		static constexpr uint32_t elementsCount{ 2 };
		D3D11_INPUT_ELEMENT_DESC vertexDesc[elementsCount]{};

		vertexDesc[0].SemanticName = "POSITION";
		vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[0].AlignedByteOffset = 0;
		vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[1].SemanticName = "TEXCOORD";
		vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[1].AlignedByteOffset = 12;
		vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

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

		if (m_pVertexBuffer)
			m_pVertexBuffer->Release();

		if (m_pIndexBuffer)
			m_pIndexBuffer->Release();
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
		const Matrix world{ m_ScaleMatrix *( m_RotationMatrix * m_TranslationMatrix) };
		const Matrix worldViewProjection{ world * (camera.viewMatrix * camera.projectionMatrix) };

		m_pEffect->SetMatrix(worldViewProjection);
	}

	void Mesh::SetDiffuseMap(const dae::Texture* pDiffuseMap)
	{
		m_pEffect->SetDiffuseMap(pDiffuseMap);
	}

	void Mesh::CycleFilteringMethods()
	{
		m_pEffect->CycleFilteringMethods();
	}

}
