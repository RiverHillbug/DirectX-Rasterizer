#pragma once

namespace dae {

	class Mesh
	{
	public:

		Mesh() = default;
		Mesh(ID3D11Device* pDevice, const std::wstring& assetFile, std::vector<Vertex> newVertices, std::vector<uint32_t> newIndices);
		void Draw(ID3D11DeviceContext* pDeviceContext) const;

	private:
		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};
		std::vector<Vertex_Out> vertices_out{};

		class Effect* m_pEffect;
		ID3D11Buffer* m_pVertexBuffer{};
		ID3D11Buffer* m_pIndexBuffer{};
		uint32_t m_IndicesCount{};
	};
}
