#pragma once

#include "Utils.h"
#include "CTextureManager.hpp"
#include "file.hpp"

int LoadObject(LPCTSTR);

class LoadedObject {
private:
	char* m_szFileName;
	int m_nID, m_nMaxFrame, m_nMaxEvent, m_bLOD, m_nMaxBone, m_bSendVS, m_nMaxFace;
	glm::vec3 m_vBBMin, m_vBBMax, m_vEvent[MAX_MDL_EVENT];

	glm::float32 m_vForce1[3], m_vForce2[3], m_vForce3[3], m_vForce4[3];

	float m_fScrlU, m_fScrlV, m_fPerSlerp;
	glm::mat4* m_pBaseBone, *m_pBaseBoneInv;
	CMotion* m_pMotion;
	LOD_GROUP m_Group[MAX_GROUP];
	MOTION_ATTR* m_pAttr;
	BOOL m_nHavePhysique;
	CTextureManager g_TextureMng;
	GMOBJECT* pObject;

	GMOBJECT m_CollObject; // Objeto de colisão

	std::vector<int> indices; // Índices do objeto
	std::vector<float> vertexList; // Lista de vértices

	std::vector<float> vertexUV; // Lista de UVS

	std::vector<std::pair<float, float>> uvs; // Coordenadas UV
	std::string texName; // Nome da textura

public:
	void Init(void);

	LoadedObject();
	LoadedObject(const char*);
	~LoadedObject(); // Declaração do destrutor

	// Funções para obter dados
	const float* getVertexData() const { return vertexList.data(); }
	const int* getIndexData() const { return indices.data(); }

	size_t getVertexCount() const { return vertexList.size() / 3; } // Assumindo 3 componentes por vértice (x, y, z)
	size_t getIndexCount() const { return indices.size(); }

	std::vector<std::pair<float, float>> getUvs() const { return uvs; }

	size_t getVertexUVCount() const { return vertexUV.size() / 2; } // Assumindo 2 componentes por vértice (x, y)
	const float* getVertexUVData() const { return vertexUV.data(); }

	const char* getTextureName() const { return texName.c_str(); }

	int LoadTMAni(CResFile*, GMOBJECT*);
	int LoadObject(LPCTSTR);
	int LoadGMObject(CResFile*, GMOBJECT*);

	// Método de acesso a UVs (removido duplicado)
	// std::vector<std::pair<float, float>> getUvs() const {
	//     return this->uvs;
	// }
};
