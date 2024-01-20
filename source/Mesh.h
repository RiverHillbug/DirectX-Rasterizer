#pragma once

namespace dae {

	class Mesh final
	{
	public:

		Mesh() = default;
		Mesh(struct ID3D11Device* pDevice, const std::wstring& effectFile, std::vector<Vertex>& newVertices, std::vector<uint32_t>& newIndices);
		Mesh(struct ID3D11Device* pDevice, const std::wstring& effectFile, const std::string& objFile);
		~Mesh();

		Mesh(const Mesh& other) = delete;
		Mesh& operator=(const Mesh& other) = delete;
		Mesh(Mesh&& other) = delete;
		Mesh& operator=(Mesh&& other) = delete;

		void Draw(struct ID3D11DeviceContext* pDeviceContext) const;
		void RotateX(float pitch);
		void RotateY(float yaw);
		void RotateZ(float roll);

		void SetMatrix(const struct Camera& camera);
		void SetDiffuseMap(const class dae::Texture* pDiffuseMap);
		void CycleFilteringMethods();

	private:
		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};
		std::vector<Vertex_Out> vertices_out{};

		class Effect* m_pEffect;
		struct ID3D11Buffer* m_pVertexBuffer{};
		struct ID3D11Buffer* m_pIndexBuffer{};
		uint32_t m_IndicesCount{};

		Matrix m_TranslationMatrix{ Vector3::UnitX, Vector3::UnitY, Vector3::UnitZ, Vector3::Zero };
		Matrix m_RotationMatrix{ Vector3::UnitX, Vector3::UnitY, Vector3::UnitZ, Vector3::Zero };
		Matrix m_ScaleMatrix{ Vector3::UnitX, Vector3::UnitY, Vector3::UnitZ, Vector3::Zero };
	};
}