#include "CTextureManager.hpp"

CTextureManager::CTextureManager() {
	m_pMaterial.resize(MAX_MATERIAL); // Redimensiona o vetor de materiais
	for (auto& material : m_pMaterial) {
		material.m_bActive = false;
		material.strBitMapFileName.clear(); // Limpa o nome do arquivo
	}
	m_nMaxTexture = 0;
}

CTextureManager::~CTextureManager() {
	DeleteDeviceObjects();
}

HRESULT CTextureManager::DeleteDeviceObjects() {
	for (auto& material : m_pMaterial) {
		if (material.m_bActive) {
			// Libera a textura OpenGL se ativa
			if (material.m_Texture != 0) {
				glDeleteTextures(1, &material.m_Texture); // Libera a textura
				material.m_Texture = 0; // Define o ID da textura como 0
			}
		}
		material.m_bActive = false; // Define como inativo
		material.strBitMapFileName.clear(); // Limpa o nome do arquivo
	}
	m_nMaxTexture = 0; // Reseta o contador de texturas
	return S_OK; // Retorna sucesso
}

int CTextureManager::DeleteMaterial(GLuint textureID) {
	if (m_nMaxTexture == 0) return false; // Verifica se não há texturas

	for (size_t i = 0; i < m_pMaterial.size(); i++) {
		if (m_pMaterial[i].m_bActive && m_pMaterial[i].m_Texture == textureID) {
			if (m_pMaterial[i].m_nUseCnt == 1) { // Se o contador de uso é 1
				// Libera a textura OpenGL
				if (m_pMaterial[i].m_Texture != 0) {
					glDeleteTextures(1, &m_pMaterial[i].m_Texture); // Libera a textura
					m_pMaterial[i].m_Texture = 0; // Define o ID da textura como 0
				}
				m_pMaterial[i].m_bActive = false; // Marca como inativo
				m_pMaterial[i].strBitMapFileName.clear(); // Limpa o nome do arquivo
				m_nMaxTexture--; // Decrementa o contador de texturas
				return true; // Retorna sucesso
			}
		}
	}
	return false; // Retorna falha se o material não foi encontrado
}

MATERIAL* CTextureManager::AddMaterial(const glm::vec4& diffuseColor, const std::string& fileName) {
	MATERIAL* pMList = nullptr;

	// Verifica se o material já existe
	for (auto& material : m_pMaterial) {
		if (material.m_bActive) {
			if (material.strBitMapFileName == fileName) { // Se o nome do arquivo já existe
				material.m_nUseCnt++; // Incrementa o contador de uso
				return &material; // Retorna o material existente
			}
		}
		else {
			pMList = &material; // Armazena a referência ao primeiro material inativo
			break; // Para de procurar, pois encontramos um inativo
		}
	}

	if (pMList == nullptr) {
		std::cerr << "CTextureManager::AddMaterial: Máximo de materiais alcançado." << std::endl;
		return nullptr; // Retorna null se não houver espaço
	}

	// Configura o material
	pMList->m_bActive = true;
	//pMList->m_Diffuse = diffuseColor; // Define a cor difusa
	pMList->m_nUseCnt = 1; // Inicializa o contador de uso
	pMList->strBitMapFileName = fileName; // Copia o nome do arquivo

	// Aqui você deve carregar a textura e atribuí-la a pMList->m_Texture
	// Exemplo fictício de carregamento de textura:
	// pMList->m_Texture = LoadTexture(fileName); // Você deve implementar LoadTexture

	m_nMaxTexture++;

	return pMList; // Retorna o material adicionado
}

MATERIAL* CTextureManager::GetMaterial(int nIdx) {
	if (nIdx < 0 || nIdx >= m_pMaterial.size()) return nullptr; // Verifica índice válido
	return &m_pMaterial[nIdx]; // Retorna o material pelo índice
}

GLuint CTextureManager::GetTexture(int nIdx) {
	if (nIdx < 0 || nIdx >= m_pMaterial.size()) return 0; // Verifica índice válido
	return m_pMaterial[nIdx].m_Texture; // Retorna a textura pelo índice
}