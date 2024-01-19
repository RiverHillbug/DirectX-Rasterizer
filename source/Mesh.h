#pragma once

namespace dae {

	class Mesh
	{
	public:

		Mesh() = default;
		Mesh(struct ID3D11Device* pDevice, const std::wstring& assetFile, std::vector<Vertex> newVertices, std::vector<uint32_t> newIndices);
		void Draw(struct ID3D11DeviceContext* pDeviceContext) const;

		//void SetMatrix(const struct Matrix& matrix);
		void SetMatrix(const struct Camera& camera);

	private:
		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};
		std::vector<Vertex_Out> vertices_out{};

		class Effect* m_pEffect;
		struct ID3D11Buffer* m_pVertexBuffer{};
		struct ID3D11Buffer* m_pIndexBuffer{};
		uint32_t m_IndicesCount{};

		Matrix m_WorldMatrix { Vector3(0.0f, 3.0f, 2.0f), Vector3(3.0f, -3.0f, 2.0f), Vector3(-3.0f, -3.0f, 2.0f), Vector3::Zero };
	};
}