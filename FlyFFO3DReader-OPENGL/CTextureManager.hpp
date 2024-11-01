#include "Utils.h"

class CTextureManager {
protected:
	int m_nMaxTexture; // Máximo de texturas ativas

public:
	std::vector<MATERIAL> m_pMaterial; // Vetor de materiais

	CTextureManager();  // Construtor
	~CTextureManager(); // Destrutor

	HRESULT DeleteDeviceObjects(); // Libera objetos de dispositivo

	int DeleteMaterial(GLuint textureID); // Deleta material por ID de textura
	MATERIAL* AddMaterial(const glm::vec4& diffuseColor, const std::string& fileName); // Adiciona um material
	MATERIAL* GetMaterial(int nIdx); // Obtém material por índice
	GLuint GetTexture(int nIdx); // Obtém textura por índice
};