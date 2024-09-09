#pragma once
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "winmm.lib")

#include <wrl/client.h>
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <vector>

struct VertexPosNormalTex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 tex;
	static const D3D11_INPUT_ELEMENT_DESC inputLayout[3];
};

struct MeshData
{
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	ComPtr<ID3D11Buffer> m_pVertexBuffer;		// 顶点缓冲区
	ComPtr<ID3D11Buffer> m_pIndexBuffer;// 索引缓冲区
	unsigned int indexNum;
};
class Mesh {
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	MeshData Meshdata;
	ComPtr<ID3D11VertexShader> m_pVertexShader;	// 顶点着色器
	ComPtr<ID3D11PixelShader> m_pPixelShader;//片元着色器
	ComPtr<ID3D11ShaderResourceView> m_diffuseTexture;	// 纹理
};
MeshData processMesh(aiMesh* mesh, const aiScene* scene, Microsoft::WRL::ComPtr<ID3D11Device> m_pd3dDevice);
void processNode(aiNode* node, const aiScene* scene, std::vector<MeshData>& MeshDatas, Microsoft::WRL::ComPtr<ID3D11Device> m_pd3dDevice);
std::vector<MeshData> loadModel(std::string path, Microsoft::WRL::ComPtr<ID3D11Device> m_pd3dDevice);