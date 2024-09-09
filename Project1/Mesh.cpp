#include "Mesh.h"
#include "d3dUtil.h"

using namespace DirectX;
const D3D11_INPUT_ELEMENT_DESC VertexPosNormalTex::inputLayout[3] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

MeshData processMesh(aiMesh* mesh, const aiScene* scene, Microsoft::WRL::ComPtr<ID3D11Device> m_pd3dDevice)
{
    MeshData myMeshData;
    unsigned int nums = mesh->mNumVertices;
    VertexPosNormalTex* vertices = new VertexPosNormalTex[nums];
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        
        vertices[i].pos = XMFLOAT3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        if(mesh->mNormals)
            vertices[i].normal= XMFLOAT3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        if (mesh->mTextureCoords[0])
        {
            vertices[i].tex = XMFLOAT2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else
            vertices[i].tex=XMFLOAT2(0.0f,0.0f);
    }
    D3D11_BUFFER_DESC vbd;
    ZeroMemory(&vbd, sizeof(vbd));
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(VertexPosNormalTex)*nums;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = vertices;
    m_pd3dDevice->CreateBuffer(&vbd, &InitData, myMeshData.m_pVertexBuffer.GetAddressOf());
    delete[]vertices;
    nums = 0;
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            nums++;
    }
    DWORD* indices = new DWORD[nums];
    int indice = 0;
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices[indice] = face.mIndices[j];
            indice++;
        }
    }
    D3D11_BUFFER_DESC ibd;
    ZeroMemory(&ibd, sizeof(ibd));
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(DWORD)*nums;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    InitData.pSysMem = indices;
    m_pd3dDevice->CreateBuffer(&ibd, &InitData, myMeshData.m_pIndexBuffer.GetAddressOf());
    myMeshData.indexNum = nums;
    myMeshData.indexNum = nums;
    return myMeshData;
}

void processNode(aiNode* node, const aiScene* scene, std::vector<MeshData>& MeshDatas, Microsoft::WRL::ComPtr<ID3D11Device> m_pd3dDevice)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        MeshDatas.push_back(processMesh(mesh, scene, m_pd3dDevice));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, MeshDatas, m_pd3dDevice);
    }
}

std::vector<MeshData> loadModel(std::string path, Microsoft::WRL::ComPtr<ID3D11Device> m_pd3dDevice)
{
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

   
    std::vector<MeshData> MeshDatas;
    processNode(scene->mRootNode, scene,MeshDatas, m_pd3dDevice);
	return MeshDatas;
}
