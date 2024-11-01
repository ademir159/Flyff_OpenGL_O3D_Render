#pragma once
#include "Utils.h"
#include "file.hpp"




struct TM_ANIMATION
{
	glm::quat m_qRot;   // Use glm::quat for quaternion representation
	glm::vec3 m_vPos;   // Use glm::vec3 for 3D vector representation
};

struct	MOTION_ATTR
{
	DWORD	m_dwAttr;

	int		m_nSndID;
	float	m_fFrame;
};



struct	BONE_FRAME
{
	TM_ANIMATION	*m_pFrame;
	glm::mat4		m_mLocalTM;
};

struct BONE;
struct BONE
{
	BONE		*m_pParent;			// »óÀ§ ºÎ¸ð ³ëµå

//	int			m_nID;				// °èÃþ¼ø¼­ ´ë·Î ºÎ¿©µÈ ¹øÈ£ - ¾îÂ÷ÇÇ ÇÊ¿ä¾øÀ»°Å °°´Ù.
	int			m_nParentIdx;		// ºÎ¸ð ÀÎµ¦½º
	char		m_szName[32];		// * ½ºÆ®¸µÇüÅÂÀÇ ³ëµå ÀÌ¸§
	glm::mat4		m_mTM;		// ase¿øº» TM
	glm::mat4		m_mInverseTM;	// * ¹Ì¸® º¯È¯µÈ ÀÎ¹ö½ºTM
	glm::mat4		m_mLocalTM;	// * ¹Ì¸®º¯È¯µÈ ·ÎÄÃ TM
};



class CMotion
{
private:
	TM_ANIMATION	*m_pMotion;			// »À´ë¼ø¼­´ë·Î °¢°¢ÀÇ ¾Ö´Ï¸ÞÀÌ¼ÇÀÌ µé¾îÀÖ´Ù.  ¾Ö´Ï°¡ ¾ø´Â »À´ë´Â LocalTM¸¸ ÀÖ´Ù.
	MOTION_ATTR		*m_pAttr;			// ÇÁ·¹ÀÓÅ©±â¸¸Å­ ÇÒ´ç.  °¢ ÇÁ·¹ÀÓ¼Ó¼º.
	int		m_nMaxFrame;				// ¸ð¼Ç ÇÁ·¹ÀÓ¼ö
public:
	int		m_nID;						// ¸ð¼ÇÀÇ °íÀ¯ ID(ÆÄÀÏ³»¿¡ ÀÖ´Ù)
	char	m_szName[64];				// ¸ð¼Ç ÆÄÀÏ¸í - ³ªÁß¿¡ ÀÌ°Ç ºüÁø´Ù.
	float	m_fPerSlerp;				//
	int		m_nMaxBone;					// ÀÌ°ÍÀº ¾îµð±îÁö³ª È®ÀÎ¿ë º¯¼öÀÏ»ÓÀÌ´Ù.  ½ÇÁ¦ º»ÀÇ °¹¼ö°¡ ÇÊ¿äÇÏ´Ù¸é CBone¿¡¼­ ÀÐ¾î¾ß ÇÑ´Ù.
	glm::vec3	m_vPivot;				// 0,0,0(¹ß)±âÁØ¿¡¼­ °ñ¹Ý(·çÆ®)ÀÇ ÁÂÇ¥.  0¹øÅ°ÀÇ ÁÂÇ¥¿Í °°´Ù.
	BONE_FRAME	*m_pBoneFrame;		// »À´ë°¹¼ö ¸¸Å­ ÇÒ´ç. ·ÎµùÀÌ ³¡³­ÈÄ »ç¿ëÇÏ±â ÁÁµµ·Ï Æ÷ÀÎÅÍ¸¦ Àç¹èÄ¡ÇÑ´Ù.  ÇÁ·¹ÀÓÀÌ ¾ø´Â°÷Àº NULL
	BONE		*m_pBoneInfo;		// »À´ë±¸¼ºÁ¤º¸
	int		m_nMaxEvent;
	glm::vec3	m_vEvent[4];
	glm::vec3		*m_pPath;			// path

	CMotion();
	~CMotion();

	void Init( void );
	void Destroy( void );

	MOTION_ATTR *GetMotionAttr( int nNumFrm ) { return m_pAttr + nNumFrm; }
	DWORD	GetAttr( int nNumFrm )
	{
		return m_pAttr[ nNumFrm ].m_dwAttr;
	}
//#if 0
	DWORD	IsAttrHit( float fOldFrm, float fNumFrm )
	{
		MOTION_ATTR		*pAttr = &m_pAttr[ (int)fNumFrm ];
		if( pAttr->m_dwAttr & MA_HIT )
			if( fOldFrm < pAttr->m_fFrame && pAttr->m_fFrame <= fNumFrm )	// ÀÌÀü ÇÁ·¹ÀÓÀÌ¶û ÇöÀç ÇÁ·¹ÀÓ »çÀÌ¿¡ Å¸Á¡ÀÌ µé¾îÀÖ¾ú´Â°¡.
				return pAttr->m_dwAttr;
			return 0;
	}
	MOTION_ATTR *IsAttrSound( float fOldFrm, float fNumFrm )
	{
		MOTION_ATTR		*pAttr = &m_pAttr[ (int)fNumFrm ];
		if( pAttr->m_dwAttr & MA_SOUND )
			if( fOldFrm < pAttr->m_fFrame && pAttr->m_fFrame <= fNumFrm )	// ÀÌÀü ÇÁ·¹ÀÓÀÌ¶û ÇöÀç ÇÁ·¹ÀÓ »çÀÌ¿¡ Å¸Á¡ÀÌ µé¾îÀÖ¾ú´Â°¡.
				return pAttr;
			return NULL;
	}
	DWORD	IsAttrQuake( float fOldFrm, float fNumFrm )
	{
		MOTION_ATTR		*pAttr = &m_pAttr[ (int)fNumFrm ];
		if( pAttr->m_dwAttr & MA_QUAKE )
			if( fOldFrm < pAttr->m_fFrame && pAttr->m_fFrame <= fNumFrm )	// ÀÌÀü ÇÁ·¹ÀÓÀÌ¶û ÇöÀç ÇÁ·¹ÀÓ »çÀÌ¿¡ Å¸Á¡ÀÌ µé¾îÀÖ¾ú´Â°¡.
				return pAttr->m_dwAttr;
			return 0;
	}
	void	SetAttr( float fNumFrm, DWORD dwAttr )
	{
		m_pAttr[ (int)fNumFrm ].m_dwAttr |= dwAttr;
		m_pAttr[ (int)fNumFrm ].m_fFrame = fNumFrm;
	}
	void	ResetAttr( int nNumFrm, DWORD dwAttr ) { m_pAttr[ nNumFrm ].m_dwAttr &= (~dwAttr); }
	TM_ANIMATION*	GetFrame( int nNumFrm ) { return &m_pMotion[nNumFrm]; }
	int		GetMaxFrame( void ) { return m_nMaxFrame; }

	int		LoadMotion( LPCTSTR strFileName );			// ÆÄÀÏ¿¡¼­ ¸ð¼ÇÆÄÀÏÀ» ÀÐÀ½
	void	ReadTM( CResFile *file, int nNumBone, int nNumFrame );

	void AnimateBone(glm::mat4* pmUpdateBone, CMotion* pMotionOld, float fFrameCurrent, int nNextFrame, int nFrameOld, bool bMotionTrans, float fBlendWeight);

};


enum GMTYPE
{
	GMT_ERROR = -1,
	GMT_NORMAL,
	GMT_SKIN,
	GMT_BONE
};

enum VBTYPE
{
	VBT_ERROR = -1,
	VBT_NORMAL,
	VBT_BUMP,
};

struct GMOBJECT;
//
// ¹öÅØ½º¿Í ¸éÁ¤º¸¸¦ °®´Â ¸Þ½¬ÇüÅÂÀÇ µ¥ÀÌÅ¸ ±¸Á¶
// Geometry Object
//

typedef struct _GLWMATERIAL9 {
	glm::vec4   Diffuse;        /* Diffuse color RGBA */
	glm::vec4   Ambient;        /* Ambient color RGB */
	glm::vec4   Specular;       /* Specular 'shininess' */
	glm::vec4   Emissive;       /* Emissive color RGB */
	float   Power;          /* Sharpness if specular highlight */
} GLWMATERIAL9;

struct MATERIAL {
	GLWMATERIAL9* material;//glm::vec4 m_Diffuse;                    // Diffuse color (instead of D3DMATERIAL9)
	GLuint m_Texture;                        // OpenGL texture ID
	std::string strBitMapFileName;          // Bitmap file name
	// No need for a device pointer in OpenGL as it's managed differently
	int m_nUseCnt;                          // Usage count for resource management
	bool m_bActive;                          // Active flag
};

#define MAX_MATERIALS 16
struct	MATERIAL_BLOCK
{
	int		m_nStartVertex;
	int		m_nPrimitiveCount;
	int		m_nTextureID;
	DWORD	m_dwEffect;
	int		m_nAmount;
	int		m_nMaxUseBone;
	int		m_UseBone[MAX_VS_BONE];
};
struct GMOBJECT {  // base object
	GMTYPE m_Type;                     // GMOBJECT Type
	int m_nVertexSize;                 // sizeof(FVF)
	int m_dwFVF;                       // FVF ID
	int m_nMaxUseBone;                 // Max bones used
	int m_UseBone[MAX_VS_BONE];        // Bones used
	int m_nID;                         // Object ID
	bool m_bOpacity;                   // Opacity flag
	bool m_bBump;                      // Bump mapping flag
	bool m_bRigid;                     // Rigid flag
	bool m_bLight;                     // Light flag
	int m_nMaxVertexList;              // Read
	int m_nMaxFaceList;                // Read
	int m_nMaxMtrlBlk;                 // Read
	int m_nMaxMaterial;                // Read
	GMTYPE m_ParentType;               // Parent type
	int m_nParentIdx;                  // Parent index
	GMOBJECT* m_pParent;               // Parent pointer
	glm::mat4 m_mLocalTM;              // Local transform
	glm::mat4 m_mInverseTM;            // Inverse transform

	glm::vec3* m_pVertexList;          // Local vertex data
	int* m_pPhysiqueVertex;             // Physics data
	MATERIAL_BLOCK* m_pMtrlBlk;       // Material blocks
	GLuint* m_pMtrlBlkTexture; // Material textures
	//std::vector<GLuint> m_pMtrlBlkTexture;
	
	TM_ANIMATION* m_pFrame;            // Animation data

	glm::vec3 m_vBBMin, m_vBBMax;      // Bounding box min, max

	bool m_bMaterial;                  // Material usage flag
	MATERIAL m_MaterialAry[MAX_MATERIALS]; // Array of materials

	void* m_pVB;                       // Vertex Buffer Object
	unsigned short* m_pIB;             // Index Buffer Object
	unsigned short* m_pIIB;            // Index buffer for instance data
	int m_nMaxVB;                      // Max Vertex Buffer size
	int m_nMaxIB;                      // Max Index Buffer size

	// OpenGL specific buffers
	GLuint m_gl_VB;                    // OpenGL Vertex Buffer Object
	GLuint m_gl_IB;                    // OpenGL Index Buffer Object

	// Temporary storage
	// SKINVERTEX *_pVB;                 // Not necessary in OpenGL
};

struct LOD_GROUP
{
	int			m_nMaxObject;
	GMOBJECT*	m_pObject;		// sizeof(GMOBJECT) * m_nMaxObject


	// °èÃþ±¸Á¶ ¾Ö´Ï¸ÞÀÌ¼ÇÈÄ °»½ÅµÈ ¸ÅÆ®¸¯½ºµéÀÌ ´ã±æ ÀÓ½Ã¹öÆÛ m_nMaxObject¸¸Å­ »ý¼º.
	// ¸¸¾à ½ºÅ²¸¸ ÀÖ´Â ¿ÀºêÁ§Æ®¶ó¸é ÀÌ°ÍÀº »ý¼ºµÇÁö ¾Ê´Â´Ù.
	glm::mat4	*_mUpdate;
};



