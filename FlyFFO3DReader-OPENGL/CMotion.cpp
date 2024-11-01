#include "CMotion.h"
#include <GL/glew.h>


CMotion :: CMotion()
{
	Init();
}

CMotion :: ~CMotion()
{
	Destroy();
}

void	CMotion :: Init( void )
{
	m_pMotion = NULL;
	m_pPath = NULL;
	m_pBoneFrame = NULL;
	m_fPerSlerp = 0.5f;
	m_nMaxEvent = 0;
	m_pBoneInfo = NULL;
	memset( m_vEvent, 0, sizeof(m_vEvent) );
	m_pAttr = NULL;
	m_pBoneInfo = NULL;
}

void	CMotion :: Destroy( void )
{
	int		i;

	if( m_pBoneFrame )
	{
		for( i = 0; i < m_nMaxBone; i ++ )
			m_pBoneFrame[i].m_pFrame = NULL;
	}
	Init();
}

int CMotion::LoadMotion(LPCTSTR szFileName)
{
	int nNumBone;
	int nNumFrame;
	int nVer;

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

	resFp.Read(&nVer, sizeof(int), 1);  // version
	if (nVer != VER_MOTION)
	{
		resFp.Close();
		return 0;
	}

	// Read ID
	resFp.Read(&m_nID, sizeof(int), 1);

	if (strlen(szFileName) > sizeof(m_szName))
	{
		resFp.Close();
		return 0;
	}

	// Store filename
	strcpy(m_szName, szFileName);

	resFp.Read(&m_fPerSlerp, sizeof(float), 1);
	resFp.Seek(32, SEEK_CUR);  // reserved

	resFp.Read(&nNumBone, sizeof(int), 1);   // Read number of bones
	resFp.Read(&nNumFrame, sizeof(int), 1);  // Read number of frames
	m_nMaxFrame = nNumFrame;
	m_nMaxBone = nNumBone;

	if (nNumFrame <= 0)
	{
		resFp.Close();
		return 0;
	}

	// Read path
	int nTemp;
	resFp.Read(&nTemp, sizeof(int), 1);  // Read if path exists
	if (nTemp)
	{
		m_pPath = new glm::vec3[nNumFrame];  // Use glm::vec3 for path
		resFp.Read(m_pPath, sizeof(glm::vec3) * nNumFrame, 1);  // Read path data
	}

	// Read TM_Animation data
	ReadTM(&resFp, nNumBone, nNumFrame);

	// Read motion attributes
	resFp.Read(m_pAttr, sizeof(MOTION_ATTR) * nNumFrame, 1);

	resFp.Read(&m_nMaxEvent, sizeof(int), 1);  // Read max events
	if (m_nMaxEvent > 0)
		resFp.Read(m_vEvent, sizeof(glm::vec3) * m_nMaxEvent, 1);  // Use glm::vec3 for events

	resFp.Close();

	// m_vPivot = m_pMotion[0].m_vPos;  // Uncomment if needed

	return 1;
}


//
//  MotionÀÇ coreºÎºÐÀ» ÀÐÀ½. ¿ÜºÎ¿¡¼­ ´Üµ¶ È£Ãâ °¡´É.
//
void CMotion::ReadTM(CResFile *file, int nNumBone, int nNumFrame) {
	int nNumSize;
	int nFrame;
	int i;

	m_nMaxBone = nNumBone; // LoadMotion()을 통해 총본수는 고정되어 있으며, 이 후 부수적인 것으로 처리
	m_nMaxFrame = nNumFrame;

	//--- 뼈대정보 배열 동적 할당.
	m_pBoneInfo = new BONE[nNumBone]; // 총 본수 만큼 동적 할당
	memset(m_pBoneInfo, 0, sizeof(BONE) * nNumBone); // zero clear

	int nLen;
	for (i = 0; i < nNumBone; i++) {
		file->Read(&nLen, 4, 1);
		if (nLen > 32) {
			file->Read(m_pBoneInfo[i].m_szName, nLen, 1); // bone node 이름
		}
		file->Read(&m_pBoneInfo[i].m_mInverseTM, sizeof(glm::mat4), 1); // Inv NODE TM, using glm::mat4
		file->Read(&m_pBoneInfo[i].m_mLocalTM, sizeof(glm::mat4), 1); // LocalTM, using glm::mat4
		file->Read(&m_pBoneInfo[i].m_nParentIdx, 4, 1); // parent bone index
	}

	// 부모 인덱스에 따른 부모 포인터 설정
	for (i = 0; i < nNumBone; i++) {
		if (m_pBoneInfo[i].m_nParentIdx == -1) // 루트 본이면 NULL
			m_pBoneInfo[i].m_pParent = NULL;
		else
			m_pBoneInfo[i].m_pParent = &m_pBoneInfo[m_pBoneInfo[i].m_nParentIdx];
	}

	file->Read(&nNumSize, 4, 1); // 애니메이션 프레임 수
	//--- 애니메이션 데이터 동적 할당.
	m_pMotion = new TM_ANIMATION[nNumSize]; // 애니메이션 데이터
	m_pBoneFrame = new BONE_FRAME[nNumBone];
	m_pAttr = new MOTION_ATTR[nNumFrame];
	memset(m_pAttr, 0, sizeof(MOTION_ATTR) * nNumFrame); // 초기화

	TM_ANIMATION *p = m_pMotion;
	int nCnt = 0;

	// 뼈대에 대한 애니메이션 데이터 읽기
	for (i = 0; i < nNumBone; i++) {
		file->Read(&nFrame, 4, 1);
		if (nFrame == 1) { // 프레임 수가 1인 경우
			m_pBoneFrame[i].m_pFrame = p;
			file->Read(m_pBoneFrame[i].m_pFrame, sizeof(TM_ANIMATION) * nNumFrame, 1); // 애니메이션 데이터
			p += nNumFrame;
			nCnt += nNumFrame;
		}
		else { // 프레임 수가 1이 아닌 경우
			file->Read(&(m_pBoneFrame[i].m_mLocalTM), sizeof(glm::mat4), 1); // 로컬 TM
			m_pBoneFrame[i].m_pFrame = NULL; // 프레임이 없음
		}
	}

	if (nCnt != nNumSize) {
		// printf("%s : frame size error", m_szName); // 오류 확인 메시지
	}
}

//
// »À´ëÀÇ ¾Ö´Ï¸ÞÀÌ¼Ç
//
void CMotion::AnimateBone(glm::mat4* pmUpdateBone, CMotion* pMotionOld, float fFrameCurrent, int nNextFrame, int nFrameOld, bool bMotionTrans, float fBlendWeight) {
	int i;
	BONE* pBone = m_pBoneInfo;          // Pointer to bone information
	TM_ANIMATION* pFrame = nullptr;      // Current frame animation
	TM_ANIMATION* pNext = nullptr;       // Next frame animation
	TM_ANIMATION* pFrameOld = nullptr;   // Previous frame animation
	glm::mat4* pMatBones = pmUpdateBone; // Updated bone matrix pointer
	int nCurrFrame;
	float fSlp;
	glm::quat qSlerp;
	glm::vec3 vSlerp;
	BONE_FRAME* pBoneFrame = nullptr;
	BONE_FRAME* pBoneFrameOld = nullptr; // Previous bone frame
	glm::mat4 m1, m2;

	if (pMotionOld == nullptr) // If no previous motion is provided, use current motion
		pMotionOld = this;

	// Update for the number of bones
	i = m_nMaxBone;

	// Calculate current and next frame
	nCurrFrame = static_cast<int>(fFrameCurrent); // Current frame index
	fSlp = fFrameCurrent - static_cast<float>(nCurrFrame); // Frame interpolation factor

	pBoneFrame = m_pBoneFrame;
	pBoneFrameOld = pMotionOld->m_pBoneFrame; // Previous bone frame

	// Iterate through each bone
	while (i--) {
		if (pBoneFrameOld->m_pFrame)
			pFrameOld = &pBoneFrameOld->m_pFrame[nFrameOld]; // Previous frame data
		else
			pFrameOld = nullptr;

		if (pBoneFrame->m_pFrame) { // If current frame data exists
			pFrame = &pBoneFrame->m_pFrame[nCurrFrame]; // Current frame data
			pNext = &pBoneFrame->m_pFrame[nNextFrame]; // Next frame data

			// Perform quaternion slerp and vector lerp
			qSlerp = glm::slerp(pFrame->m_qRot, pNext->m_qRot, fSlp);
			vSlerp = glm::mix(pFrame->m_vPos, pNext->m_vPos, fSlp);

			if (bMotionTrans) {
				float fBlendSlp = fBlendWeight;

				if (pFrameOld) {
					qSlerp = glm::slerp(pFrameOld->m_qRot, qSlerp, fBlendSlp); // Blend with old rotation
					vSlerp = glm::mix(pFrameOld->m_vPos, vSlerp, fBlendSlp); // Blend with old position
				}
				else {
					glm::quat qLocal;
					glm::vec3 vLocal;
					glm::mat4 pmLocalTM = pBoneFrameOld->m_mLocalTM; // Local transformation matrix

					qLocal = glm::quat_cast(pmLocalTM); // Convert matrix to quaternion
					vLocal = glm::vec3(pmLocalTM[3]); // Extract translation vector

					qSlerp = glm::slerp(qLocal, qSlerp, fBlendSlp); // Blend with local rotation
					vSlerp = glm::mix(vLocal, vSlerp, fBlendSlp); // Blend with local position
				}
			}

			// Create translation matrix
			m1 = glm::translate(glm::mat4(1.0f), vSlerp); // Create translation matrix

			// Create rotation matrix
			m2 = glm::mat4_cast(qSlerp); // Convert quaternion to rotation matrix

			m2 = m2 * m1; // Combine translation and rotation

			if (pBone->m_pParent)
				m2 = m2 * pMatBones[pBone->m_nParentIdx]; // Apply parent's transformation
		}
		else { // If there is no current frame data
			if (bMotionTrans) { // If blending is needed
				float fBlendSlp = fBlendWeight;
				glm::quat qLocal;
				glm::vec3 vLocal;
				glm::mat4 pmLocalTM = pBoneFrame->m_mLocalTM; // Local transformation matrix

				qLocal = glm::quat_cast(pmLocalTM); // Convert matrix to quaternion
				vLocal = glm::vec3(pmLocalTM[3]); // Extract translation vector

				if (pFrameOld) {
					qSlerp = glm::slerp(pFrameOld->m_qRot, qLocal, fBlendSlp); // Blend with previous rotation
					vSlerp = glm::mix(pFrameOld->m_vPos, vLocal, fBlendSlp); // Blend with previous position
				}
				else {
					glm::quat qLocal2;
					glm::vec3 vLocal2;
					glm::mat4 pmLocalTM2 = pBoneFrameOld->m_mLocalTM; // Previous local transformation matrix

					qLocal2 = glm::quat_cast(pmLocalTM2); // Convert to quaternion
					vLocal2 = glm::vec3(pmLocalTM2[3]); // Extract translation

					qSlerp = glm::slerp(qLocal2, qLocal, fBlendSlp); // Blend with previous local rotation
					vSlerp = glm::mix(vLocal2, vLocal, fBlendSlp); // Blend with previous local position
				}

				m1 = glm::translate(glm::mat4(1.0f), vSlerp); // Create translation matrix
				m2 = glm::mat4_cast(qSlerp); // Create rotation matrix

				m2 = m2 * m1; // Combine translation and rotation
			}
			else {
				m2 = pBoneFrame->m_mLocalTM; // Just use local transformation
			}

			if (pBone->m_pParent)
				m2 = m2 * pMatBones[pBone->m_nParentIdx]; // Apply parent's transformation
		}

		*pmUpdateBone = m2; // Update the final transformation matrix

		pBoneFrame++;
		pBoneFrameOld++;
		pBone++;
		pmUpdateBone++;
	}
}