#include "pch.h"
#include "DataTypes.h"
#include "Effect.h"
#include "Mesh.h"

namespace dae {

	Mesh::Mesh(ID3D11Device* pDevice, const std::wstring& assetFile, std::vector<Vertex> newVertices, std::vector<uint32_t> newIndices)
		: vertices{ newVertices }
		, indices{ newIndices }
	{
		m_pEffect = new Effect(pDevice, assetFile);

		static constexpr uint32_t elementsCount{ 2 };
		D3D11_INPUT_ELEMENT_DESC vertexDesc[elementsCount]{};

		vertexDesc[0].SemanticName = "POSITION";
		vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[0].AlignedByteOffset = 0;
		vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[1].SemanticName = "COLOR";
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
}