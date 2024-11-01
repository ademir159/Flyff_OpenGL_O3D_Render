//#define __YENV
#define __VER 15
#include "LoadedObject.hpp"
//#include "CMotion.h"

void LoadedObject::Init()
{
	this->m_bLOD = 0;
	memset(&m_CollObject, 0, sizeof(m_CollObject));
	m_CollObject.m_Type = GMT_ERROR;
}
LoadedObject::LoadedObject(const char* fileName)
{
	Init();
	int result = this->LoadObject(fileName);

	char test[30];
	sprintf(test, "%i", result);
//	MessageBox(NULL, test, "", MB_OK);
}

LoadedObject::LoadedObject()
{
	Init();
}
LoadedObject::~LoadedObject() {
	// Ensure all dynamically allocated memory is freed
	if (m_pBaseBone) {
		delete[] m_pBaseBone;
		m_pBaseBone = nullptr;
	}
	if (m_pMotion) {
		delete m_pMotion;
		m_pMotion = nullptr;
	}
	// Clean up other resources as necessary
}


int LoadedObject::LoadObject(LPCTSTR szFileName)
{

	int		j, nTemp;
	int		nVer;
	glm::float32	vTemp[3];
	BOOL	bNormalObj = FALSE;

	CResFile resFp;
	const char* procName = "rb";
	TCHAR RB[128];
	_tcscpy(RB, procName);


	BOOL bRet = resFp.Open(szFileName, RB);
	if (bRet == FALSE)
	{
		resFp.Close();
		return 0;
	}

	if (bRet == FALSE)
	{
		cout << "No such file as: " << szFileName << endl;
		return 0;
	}


	char szName[MAX_PATH];
	char buff[MAX_PATH];
	char cLen;
	resFp.Read(&cLen, 1, 1);
	resFp.Read(buff, cLen, 1);
	for (j = 0; j < cLen; j++)
		buff[j] = buff[j] ^ (char)0xcd;


	if (cLen >= 64)
	{
		return 0;
	}

	buff[cLen] = 0;

	resFp.Read(&nVer, 4, 1);
	if (nVer < VER_MESH)
	{
		resFp.Close();
		return 0;
	}
	resFp.Read(&m_nID, 4, 1);		// Serial ID
	resFp.Read(&m_vForce1, sizeof(m_vForce1), 1);  // Force vectors
	resFp.Read(&m_vForce2, sizeof(m_vForce2), 1);



	if (nVer >= 22)
	{
		resFp.Read(&m_vForce3, sizeof(glm::vec3), 1);
		resFp.Read(&m_vForce4, sizeof(glm::vec3), 1);
	}

	resFp.Read(&m_fScrlU, sizeof(float), 1);
	resFp.Read(&m_fScrlV, sizeof(float), 1);
	resFp.Seek(16, SEEK_CUR);  // reserved

	resFp.Read(&m_vBBMin, sizeof(glm::vec3), 1);  // Bounding box min
	resFp.Read(&m_vBBMax, sizeof(glm::vec3), 1);
	resFp.Read(&m_fPerSlerp, sizeof(float), 1);  // Per slerp
	resFp.Read(&m_nMaxFrame, 4, 1);  // Max animation frame
	resFp.Read(&m_nMaxEvent, 4, 1);
	if (m_nMaxEvent > 0)
		resFp.Read(m_vEvent, sizeof(glm::vec3) * m_nMaxEvent, 1);

	resFp.Read(&nTemp, 4, 1);



	cout << nTemp << endl;

	if (nTemp)
	{
		m_CollObject.m_Type = GMT_NORMAL;
		LoadGMObject(&resFp, &m_CollObject);
	}
	resFp.Read(&m_bLOD, 4, 1);  // LOD check

	// Object loading and allocation
	resFp.Read(&m_nMaxBone, 4, 1);
	if (m_nMaxBone > 0) {
		m_pBaseBone = new glm::mat4[m_nMaxBone * 2];
		m_pBaseBoneInv = m_pBaseBone + m_nMaxBone;
		resFp.Read(m_pBaseBone, sizeof(glm::mat4) * m_nMaxBone, 1);
		resFp.Read(m_pBaseBoneInv, sizeof(glm::mat4) * m_nMaxBone, 1);
		if (m_nMaxFrame > 0) {
			m_pMotion = new CMotion;
			m_pMotion->ReadTM(&resFp, m_nMaxBone, m_nMaxFrame);
		}
		resFp.Read(&m_bSendVS, 4, 1);
	}

	int nMaxGroup = (m_bLOD) ? MAX_GROUP : 1;  // LOD check
	LOD_GROUP* pGroup;
	int nPoolSize, nDebugSize = 0;
	resFp.Read(&nPoolSize, 4, 1);  // Pool size
	GMOBJECT* pPool = new GMOBJECT[nPoolSize];
	memset(pPool, 0, sizeof(GMOBJECT) * nPoolSize);


	for (int i = 0; i < nMaxGroup; i++) {
		pGroup = &m_Group[i];
		resFp.Read(&pGroup->m_nMaxObject, 4, 1);  // Geometry object count

		pGroup->m_pObject = new GMOBJECT[nPoolSize];//pPool;  // Assign object pool
		pPool += pGroup->m_nMaxObject;

		nDebugSize += pGroup->m_nMaxObject;
		if (nDebugSize > nPoolSize)
			printf("LoadObject : ¸Þ¸ð¸® ¿À¹öÇÃ·Î¿ì %d, %d", nPoolSize, nDebugSize);

		memset(pGroup->m_pObject, 0, sizeof(GMOBJECT) * pGroup->m_nMaxObject);

		for (j = 0; j < pGroup->m_nMaxObject; j++)
			pGroup->m_pObject[j].m_nID = -1;


		// Load geometry objects
		cout << pGroup->m_nMaxObject << endl;
		for (j = 0; j < pGroup->m_nMaxObject; j++) {
			GMOBJECT* pObject = &pGroup->m_pObject[j];

			int nType;
			resFp.Read(&nType, 4, 1);  // Object type
			pObject->m_Type = (GMTYPE)(nType & 0xffff);
			if (nType & 0x80000000) pObject->m_bLight = TRUE;

			resFp.Read(&pObject->m_nMaxUseBone, 4, 1);
			if (pObject->m_nMaxUseBone > 0)
				resFp.Read(pObject->m_UseBone, sizeof(int) * pObject->m_nMaxUseBone, 1);

			// Set vertex size
#if !defined(__YENV)
			if (pObject->m_Type == GMT_SKIN) {
				pObject->m_nVertexSize = sizeof(SKINVERTEX);
			}
			else {
				pObject->m_nVertexSize = sizeof(NORMALVERTEX);
			}
#endif //__YENV

			resFp.Read(&pObject->m_nID, 4, 1);  // Object ID
			int nParentIdx;
			resFp.Read(&nParentIdx, 4, 1);  // Parent index
			pObject->m_nParentIdx = nParentIdx;

			if (nParentIdx != -1) {
				resFp.Read(&pObject->m_ParentType, 4, 1);  // Parent type
				switch (pObject->m_ParentType) {
				case GMT_BONE: pObject->m_pParent = NULL; break;
				case GMT_NORMAL: pObject->m_pParent = &pGroup->m_pObject[nParentIdx]; break;
				}
			}

			resFp.Read(&pObject->m_mLocalTM, sizeof(glm::mat4), 1);  // Local transform
			if (LoadGMObject(&resFp, pObject) == 0)
			{
			}

			if (pObject->m_Type == GMT_SKIN) {
				pObject->m_nVertexSize = sizeof(SKINVERTEX);
			}
			else {
				pObject->m_nVertexSize = sizeof(NORMALVERTEX);
			}

			if (pObject->m_pPhysiqueVertex == NULL) bNormalObj = TRUE;
			else m_nHavePhysique = TRUE;

			if (pObject->m_Type == GMT_NORMAL) {
				if (m_nMaxFrame > 0) LoadTMAni(&resFp, pObject);
			}
		}
	}


	if (bNormalObj) {
		glm::mat4* pmPool = new glm::mat4[nPoolSize];
		for (int i = 0; i < nMaxGroup; i++) {
			m_Group[i]._mUpdate = pmPool;  // Assign update matrix
			pmPool += m_Group[i].m_nMaxObject;  // Move pointer forward

			// Initialize each matrix to identity
			for (j = 0; j < m_Group[i].m_nMaxObject; j++) {
				m_Group[i]._mUpdate[j] = glm::mat4(1.0f);
			}
		}
	}

	if (nVer >= 21) {
		int nAttr = 0;
		resFp.Read(&nAttr, 4, 1);
		if (nAttr == m_nMaxFrame) {
			if (m_nMaxFrame > 0)
				m_pAttr = new MOTION_ATTR[m_nMaxFrame];
			else
				m_pAttr = NULL;
			resFp.Read(m_pAttr, sizeof(MOTION_ATTR) * m_nMaxFrame, 1);
		}
	}

	resFp.Close();
	return 1;
}

int LoadedObject::LoadGMObject(CResFile *file, GMOBJECT* pObject)
{
	DWORD dwTemp;
	int i;

	// Ler os limites da caixa delimitadora
	file->Read(&pObject->m_vBBMin, sizeof(glm::vec3), 1);
	cout << "gets" << endl;
	file->Read(&pObject->m_vBBMax, sizeof(glm::vec3), 1);

	file->Read(&pObject->m_bOpacity, 4, 1);
	file->Read(&pObject->m_bBump, 4, 1);
	file->Read(&pObject->m_bRigid, 4, 1);
	file->Seek(28, SEEK_CUR);

	// Ler os tamanhos das listas
	file->Read(&pObject->m_nMaxVertexList, 4, 1);
	file->Read(&pObject->m_nMaxVB, 4, 1);
	file->Read(&pObject->m_nMaxFaceList, 4, 1);
	file->Read(&pObject->m_nMaxIB, 4, 1);

	m_nMaxFace += pObject->m_nMaxFaceList;

	if (pObject->m_Type == GMT_SKIN)
	{
		pObject->m_pVertexList = new glm::vec3[pObject->m_nMaxVertexList];
		SKINVERTEX *pVB = new SKINVERTEX[pObject->m_nMaxVB];
		pObject->m_pVB = pVB;
	}
	else
	{
		pObject->m_pVertexList = new glm::vec3[pObject->m_nMaxVertexList];
		NORMALVERTEX *pVB = new NORMALVERTEX[pObject->m_nMaxVB];

		pObject->m_pVB = pVB;
	}
	pObject->m_pIB = new WORD[pObject->m_nMaxIB + pObject->m_nMaxVB];
	pObject->m_pIIB = pObject->m_pIB + pObject->m_nMaxIB;


	file->Read(pObject->m_pVertexList, sizeof(glm::vec3) * pObject->m_nMaxVertexList, 1);

	SKINVERTEX * SpVB = new SKINVERTEX[pObject->m_nMaxVB];
	NORMALVERTEX * NpVB = new NORMALVERTEX[pObject->m_nMaxVB];

	if (pObject->m_Type == GMT_SKIN)
		file->Read(SpVB, sizeof(SKINVERTEX) * pObject->m_nMaxVB, 1);
	else
		file->Read(NpVB, sizeof(NORMALVERTEX) * pObject->m_nMaxVB, 1);


	file->Read(pObject->m_pIB, sizeof(WORD) * pObject->m_nMaxIB, 1);
	file->Read(pObject->m_pIIB, sizeof(WORD) * pObject->m_nMaxVB, 1);

	/*
	if (pObject->m_Type == GMT_SKIN && this->vertexList.size() == 0)
	{
		for (int k = pObject->m_nMaxVB; k > 0 ; k--) {
			std::pair<float, float> uv(NpVB[k].tu, NpVB[k].tv);
			this->uvs.push_back(uv);
		}
	}
	*/

	//vertexUV
	if (pObject->m_Type == GMT_SKIN && this->vertexList.size() == 0) {
		for (int k = 0; k < pObject->m_nMaxVB; k++) {
			this->vertexList.push_back(SpVB[k].position.x);
			this->vertexList.push_back(SpVB[k].position.y);
			this->vertexList.push_back(SpVB[k].position.z);

			this->vertexUV.push_back(SpVB[k].tu);
			this->vertexUV.push_back(SpVB[k].tv);

			//std::pair<float, float> uv(SpVB[k].tu, SpVB[k].tv);
			//this->uvs.push_back(uv);
		}
		for (int k = 0; k < pObject->m_nMaxVB; k++) {
			this->vertexList.push_back(SpVB[k].position.x);
			this->vertexList.push_back(SpVB[k].position.y);
			this->vertexList.push_back(SpVB[k].position.z);

			this->vertexUV.push_back(SpVB[k].tu);
			this->vertexUV.push_back(SpVB[k].tv);

		//	std::pair<float, float> uv(SpVB[k].tu, SpVB[k].tv);
			//this->uvs.push_back(uv);
		}
	}
	else if (this->vertexList.size() == 0) {
		for (int k = 0; k < pObject->m_nMaxVB; k++) {
			this->vertexList.push_back(NpVB[k].position.x);
			this->vertexList.push_back(NpVB[k].position.y);
			this->vertexList.push_back(NpVB[k].position.z);

			this->vertexUV.push_back(NpVB[k].tu);
			this->vertexUV.push_back(NpVB[k].tv);

			std::pair<float, float> uv(NpVB[k].tu, NpVB[k].tv);
			this->uvs.push_back(uv);
		}
	}

	if (this->indices.size() == 0) {
		for (int k = 0; k < pObject->m_nMaxIB; k++) {
			this->indices.push_back(pObject->m_pIB[k]);
		}
	}



	file->Read(&dwTemp, 4, 1);
	if (dwTemp) {
		pObject->m_pPhysiqueVertex = new int[pObject->m_nMaxVertexList];
		file->Read(pObject->m_pPhysiqueVertex, sizeof(int) * pObject->m_nMaxVertexList, 1);
	}

	// Ler dados do material
	GLWMATERIAL9 mMaterial;
	char szBitmap[256];
	int nLen;
	MATERIAL *mMaterialAry[16] = { nullptr };
	int bIsMaterial;

	file->Read(&bIsMaterial, 4, 1);
	pObject->m_bMaterial = bIsMaterial;


	if (bIsMaterial)
	{
		float r, g, b, a;

		file->Read(&pObject->m_nMaxMaterial, 4, 1);


		if (pObject->m_nMaxMaterial == 0)
			pObject->m_nMaxMaterial = 1;




		for (int i = 0; i < pObject->m_nMaxMaterial; i++)
		{
			file->Read(&mMaterial, sizeof(GLWMATERIAL9), 1);
			file->Read(&nLen, 4, 1);

			if (nLen > sizeof(szBitmap))
			{
				printf("CObject3D::LoadGMObject: %s bitmap file name length exceeds buffer size: length = %d\n", m_szFileName, nLen);
				return 0; // Falha na leitura
			}
			file->Read(szBitmap, nLen, 1);



			if (i == 0)
			{
				this->texName = szBitmap;
			}




			strlwr(szBitmap);
			pObject->m_MaterialAry[i].material = &mMaterial;
			pObject->m_MaterialAry[i].strBitMapFileName.assign(szBitmap);
		}
	}

	// Ler blocos de material
	file->Read(&pObject->m_nMaxMtrlBlk, 4, 1);
	if (pObject->m_nMaxMtrlBlk >= 32)
	{
		printf("Número máximo de blocos de material excedido: %s", m_szFileName);
		return 0;
	}




	if (pObject->m_nMaxMtrlBlk > 0)
	{
		pObject->m_pMtrlBlk = new MATERIAL_BLOCK[pObject->m_nMaxMtrlBlk];
		pObject->m_pMtrlBlkTexture = new GLuint[pObject->m_nMaxMtrlBlk * 8];
		memset(pObject->m_pMtrlBlkTexture, 0, sizeof(GLuint) * (pObject->m_nMaxMtrlBlk * 8));


		// Ler dados dos blocos de material
		file->Read(pObject->m_pMtrlBlk, sizeof(MATERIAL_BLOCK) * pObject->m_nMaxMtrlBlk, 1);

		// Loop para atribuir texturas
		for (int i = 0; i < pObject->m_nMaxMtrlBlk; i++)
		{
			if (bIsMaterial)
			{
				int textureID = pObject->m_pMtrlBlk[i].m_nTextureID;


				// Verificar se o ID da textura é válido
				if (textureID >= 0 && textureID < MAX_MATERIALS && mMaterialAry[textureID])
				{
				//	pObject->m_pMtrlBlkTexture[i] = mMaterialAry[textureID]->m_Texture; // Atribuindo o ID da textura OpenGL
				}
			}
		}
	}
	return 1; // Sucesso
}



int	LoadedObject::LoadTMAni( CResFile *file, GMOBJECT *pObject )
{
	int	bFrame;
	file->Read( &bFrame, 4, 1 );
	if( bFrame == 0 )	return 0;

	pObject->m_pFrame = new TM_ANIMATION[ m_nMaxFrame ];
	file->Read( pObject->m_pFrame, sizeof(TM_ANIMATION) * m_nMaxFrame, 1 );

	return 1;
}
/*
std::vector<float> LoadedObject::getVerticeList(){
    return this->vertexList;
}

std::vector<int> LoadedObject::getIndiceList(){
    return this->indices;
}


std::vector< pair <float, float> > LoadedObject::getUvs(){
    return this->uvs;
}

const char * LoadedObject::getTextureName(){
    return this->texName.c_str();
}
*/
