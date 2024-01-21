#pragma once

namespace dae {

	class Mesh final
	{
	public:

		Mesh() = default;
		Mesh(struct ID3D11Device* pDevice, const std::wstring& effectFile, std::vector<Vertex> newVertices, std::vector<uint32_t> newIndices);
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
		void SetWorldMatrix();
		void SetDiffuseMap(const class Texture* pDiffuseMap);
		void SetNormalMap(const class Texture* pNormalMap);
		void SetSpecularMap(const class Texture* pSpecularMap);
		void SetGlossinessMap(const class Texture* pGlossinessMap);
		void CycleFilteringMethods();
		void ToggleNormalMap();

	private:
		std::vector<Vertex> m_Vertices{};
		std::vector<uint32_t> m_Indices{};

		class Effect* m_pEffect{ nullptr };
		struct ID3D11Buffer* m_pVertexBuffer{ nullptr };
		struct ID3D11Buffer* m_pIndexBuffer{ nullptr };
		uint32_t m_IndicesCount{};

		Matrix m_TranslationMatrix{ Vector3::UnitX, Vector3::UnitY, Vector3::UnitZ, Vector3::Zero };
		Matrix m_RotationMatrix{ Vector3::UnitX, Vector3::UnitY, Vector3::UnitZ, Vector3::Zero };
		Matrix m_ScaleMatrix{ Vector3::UnitX, Vector3::UnitY, Vector3::UnitZ, Vector3::Zero };
	};
}