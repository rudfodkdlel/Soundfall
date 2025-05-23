#pragma once

namespace Engine
{
	typedef struct tagEngineDesc
	{
		HINSTANCE		hInstance;
		HWND			hWnd;
		bool			isWindowed;
		unsigned int	iWinSizeX;
		unsigned int	iWinSizeY;
		unsigned int	iNumLevels;
	}ENGINE_DESC;

	typedef struct tagKeyFrame
	{
		/* 행렬이 아닌이유? : 상태와 상태 사이를 보간해주기위해서. */
		XMFLOAT3		vScale;
		XMFLOAT4		vRotation;
		XMFLOAT3		vTranslation;

		/* 이 상태를 취해야하는 재생위치  */
		float			fTrackPosition;
	}KEYFRAME;



	typedef struct ENGINE_DLL tagVertexPositionTexcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		static const unsigned int					iNumElements = { 2 };

		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXPOSTEX;

	typedef struct ENGINE_DLL tagVertexPositionNormalTexcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int					iNumElements = { 3 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXNORTEX;

	typedef struct ENGINE_DLL tagVertexMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT3		vTangent;
		XMFLOAT2		vTexcoord;

		static const unsigned int					iNumElements = { 4 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];


	}VTXMESH;

	typedef struct ENGINE_DLL tagVertexAnimMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT3		vTangent;
		XMFLOAT2		vTexcoord;
		XMUINT4			vBlendIndices;
		XMFLOAT4		vBlendWeights;

		static const unsigned int					iNumElements = { 6 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXANIMMESH;

	// 모델 저장, 불러오기 용
	typedef struct tagModelDesc {
		unsigned int iNumMeshes;
		unsigned int iNumMaterials;
		unsigned int iBoneNums; // 전체 뼈 개수
		unsigned int iNumAnimations;
	}MODEL_DESC;

	typedef struct tagMeshDesc {
		char szName[MAX_PATH];
		unsigned int iMaterialIndex;
		unsigned int iNumVertices;
		unsigned int iNumIndices;

		// AnimMesh에서 필요함
		unsigned int iNumBones;
		XMFLOAT4X4 BoneMatrices[512];
		vector<int> BoneIndices;
		vector<XMFLOAT4X4> OffsetMatrices;

		// vertex 정보들
		vector<VTXMESH> NonAnimMesh;
		vector<VTXANIMMESH> AnimMesh;
		// 삼각형 만들때 필요한 점들 미리 저장
		vector<unsigned int> iIndices;
	}MESH_DESC;

	typedef struct tagMaterialPathDesc {
		wchar_t szTexturePath[MAX_PATH];
		unsigned int iTextureType;
	}MATERIAL_PATH_DESC;

	typedef struct tagMaterialDesc {
		unsigned int iNumSRVs[27];
		vector<MATERIAL_PATH_DESC> TexturePaths[27];
	}MATERIAL_DESC;

	typedef struct tagBoneDesc {
		char szName[MAX_PATH];
		XMFLOAT4X4 Transformation;
		int iParentBoneIndex;
	}BONE_DESC;

	typedef struct tagChannelDesc {
		char szName[MAX_PATH];
		unsigned int iNumKeyFrames;
		unsigned int	 iBoneIndex;
		vector<KEYFRAME> KeyFrames;
	}CHANNEL_DESC;

	typedef struct tagAnimationDesc {
		unsigned int iNumChannels;
		float		 fTickPerSecond;
		float		 fDuration;
		vector<CHANNEL_DESC> Channels;
	}ANIM_DESC;

	// 오브젝트 저장 불러오기 용
	typedef struct tagObjectSaveDesc
	{
		XMFLOAT4X4  matWorld;
		wstring		szPrototypetag;
		int			PrototypeLevelIndex;

	}OBJECT_SAVE_DESC;

}