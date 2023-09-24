#pragma once
#include "Vector4D.h"

#ifndef MODELINFO
#define MODELINFO

/*struct studiomeshdata_t;
struct decal_t;
struct msurface1_t;
struct msurfacelighting_t;
struct msurfacenormal_t;
class ITexture;
class CEngineSprite;

// !!! if this is changed, it must be changed in asm_draw.h too !!!
struct mvertex_t
{
	Vector		position;
};

// !!! if this is changed, it must be changed in asm_draw.h too !!!
struct medge_t
{
	unsigned short	v[2];
	//	unsigned int	cachededgeoffset;
};

class IMaterial;
class IMaterialVar;

// This is here for b/w compatibility with world surfaces that use
// WorldVertexTransition. We can get rid of it when we rev the engine.
#define TEXINFO_USING_BASETEXTURE2	0x0001

struct mtexinfo_t
{
	Vector4D	textureVecsTexelsPerWorldUnits[2];	// [s/t] unit vectors in world space. 
	// [i][3] is the s/t offset relative to the origin.
	Vector4D	lightmapVecsLuxelsPerWorldUnits[2];
	float		luxelsPerWorldUnit;
	float		worldUnitsPerLuxel;
	unsigned short flags;		// SURF_ flags.
	unsigned short texinfoFlags;// TEXINFO_ flags.
	IMaterial* material;

	mtexinfo_t(mtexinfo_t const& src)
	{
		// copy constructor needed since Vector4D has no copy constructor
		memcpy(this, &src, sizeof(mtexinfo_t));
	}
};

struct mnode_t
{
	// common with leaf
	int			contents;		// <0 to differentiate from leafs
	// -1 means check the node for visibility
	// -2 means don't check the node for visibility

	int			visframe;		// node needs to be traversed if current

	mnode_t* parent;
	short		area;			// If all leaves below this node are in the same area, then
	// this is the area index. If not, this is -1.
	short		flags;

	Vector		m_vecCenter;
	Vector		m_vecHalfDiagonal;

	// node specific
	cplane_t* plane;
	mnode_t* children[2];

	unsigned short		firstsurface;
	unsigned short		numsurfaces;
};


struct mleaf_t
{
public:

	// common with node
	int			contents;		// contents mask
	int			visframe;		// node needs to be traversed if current

	mnode_t* parent;

	short		area;
	short		flags;
	Vector		m_vecCenter;
	Vector		m_vecHalfDiagonal;


	// leaf specific
	short		cluster;
	short		leafWaterDataID;

	unsigned short		firstmarksurface;
	unsigned short		nummarksurfaces;

	short		nummarknodesurfaces;
	short		unused;

	unsigned short	dispListStart;			// index into displist of first displacement
	unsigned short	dispCount;				// number of displacements in the list for this leaf
};


struct mleafwaterdata_t
{
	float	surfaceZ;
	float	minZ;
	short	surfaceTexInfoID;
	short	firstLeafIndex;
};


struct mcubemapsample_t
{
	Vector origin;
	ITexture* pTexture;
	unsigned char size; // default (mat_envmaptgasize) if 0, 1<<(size-1) otherwise.
};


typedef struct mportal_s
{
	unsigned short* vertList;
	int				numportalverts;
	cplane_t* plane;
	unsigned short	cluster[2];
	//	int				visframe;
} mportal_t;


typedef struct mcluster_s
{
	unsigned short* portalList;
	int				numportals;
} mcluster_t;


struct mmodel_t
{
	Vector		mins, maxs;
	Vector		origin;		// for sounds or lights
	float		radius;
	int			headnode;
	int			firstface, numfaces;
};

struct mprimitive_t
{
	int	type;
	unsigned short	firstIndex;
	unsigned short	indexCount;
	unsigned short	firstVert;
	unsigned short	vertCount;
};

struct mprimvert_t
{
	Vector		pos;
	float		texCoord[2];
	float		lightCoord[2];
};

typedef dleafambientindex_t mleafambientindex_t;
typedef dleafambientlighting_t mleafambientlighting_t;

struct LightShadowZBufferSample_t
{
	float m_flTraceDistance;								// how far we traced. 0 = invalid
	float m_flHitDistance;									// where we hit
};

struct worldbrushdata_t
{
	int			numsubmodels;
	int			nWorldFaceCount;

	int			numplanes;
	cplane_t* planes;

	int			numleafs;		// number of visible leafs, not counting 0
	mleaf_t* leafs;

	int			numleafwaterdata;
	mleafwaterdata_t* leafwaterdata;

	int			numvertexes;
	mvertex_t* vertexes;

	int			numoccluders;
	doccluderdata_t* occluders;

	int			numoccluderpolys;
	doccluderpolydata_t* occluderpolys;

	int			numoccludervertindices;
	int* occludervertindices;

	int				numvertnormalindices;	// These index vertnormals.
	unsigned short* vertnormalindices;

	int			numvertnormals;
	Vector* vertnormals;

	int			numnodes;
	mnode_t* nodes;
	unsigned short* m_LeafMinDistToWater;

	int			numtexinfo;
	mtexinfo_t* texinfo;

	int			numtexdata;
	csurface_t* texdata;

	int         numDispInfos;
	HDISPINFOARRAY	hDispInfos;	// Use DispInfo_Index to get IDispInfos..

	int         numOrigSurfaces;
	msurface_t  *pOrigSurfaces;
	

	int			numsurfaces;
	msurface1_t* surfaces1;
	msurface2_t* surfaces2;
	msurfacelighting_t* surfacelighting;
	msurfacenormal_t* surfacenormals;
	unsigned short* m_pSurfaceBrushes;
	dfacebrushlist_t* m_pSurfaceBrushList;

	int			numvertindices;
	unsigned short* vertindices;

	int nummarksurfaces;
	SurfaceHandle_t* marksurfaces;

	ColorRGBExp32* lightdata;
	int					m_nLightingDataSize;

	int			numworldlights;
	dworldlight_t* worldlights;

	lightzbuffer_t* shadowzbuffers;

	// non-polygon primitives (strips and lists)
	int			numprimitives;
	mprimitive_t* primitives;

	int			numprimverts;
	mprimvert_t* primverts;

	int			numprimindices;
	unsigned short* primindices;

	int				m_nAreas;
	darea_t* m_pAreas;

	int				m_nAreaPortals;
	dareaportal_t* m_pAreaPortals;

	int				m_nClipPortalVerts;
	Vector* m_pClipPortalVerts;

	mcubemapsample_t* m_pCubemapSamples;
	int				   m_nCubemapSamples;

	int				m_nDispInfoReferences;
	unsigned short* m_pDispInfoReferences;

	mleafambientindex_t* m_pLeafAmbient;
	mleafambientlighting_t* m_pAmbientSamples;

	// specific technique that discards all the lightmaps after load
	// no lightstyles or dlights are possible with this technique
	bool		m_bUnloadedAllLightmaps;

	CMemoryStack* m_pLightingDataStack;

	int              m_nBSPFileSize;
};

// only models with type "mod_brush" have this data
struct brushdata_t
{
	worldbrushdata_t* pShared;
	int				firstmodelsurface;
	int				nummodelsurfaces;

	// track the union of all lightstyles on this brush.  That way we can avoid
	// searching all faces if the lightstyle hasn't changed since the last update
	int				nLightstyleLastComputedFrame;
	unsigned short	nLightstyleIndex;	// g_ModelLoader actually holds the allocated data here
	unsigned short	nLightstyleCount;

	unsigned short	renderHandle;
	unsigned short	firstnode;
};

// only models with type "mod_sprite" have this data
struct spritedata_t
{
	int				numframes;
	int				width;
	int				height;
	CEngineSprite* sprite;
};

struct model_t
{
	FileNameHandle_t	fnHandle;
	char				szPathName[MAX_OSPATH];

	int					nLoadFlags;		// mark loaded/not loaded
	int					nServerCount;	// marked at load

	modtype_t			type;
	int					flags;			// MODELFLAG_???

	// volume occupied by the model graphics	
	Vector				mins, maxs;
	float				radius;
	KeyValues* m_pKeyValues;
	union
	{
		brushdata_t		brush;
		MDLHandle_t		studio;
		spritedata_t	sprite;
	};

};

class IVModelInfo
{
public:
	virtual							~IVModelInfo(void) { }

	virtual const model_t* GetModel(int modelindex) const = 0;
	// Returns index of model by name
	virtual int						GetModelIndex(const char* name) const = 0;

	// Returns name of model
	virtual const char* GetModelName(const model_t* model) const = 0;
	virtual vcollide_t* GetVCollide(const model_t* model) const = 0;
	virtual vcollide_t* GetVCollide(int modelindex) const = 0;
	virtual void					GetModelBounds(const model_t* model, Vector& mins, Vector& maxs) const = 0;
	virtual	void					GetModelRenderBounds(const model_t* model, Vector& mins, Vector& maxs) const = 0;
	virtual int						GetModelFrameCount(const model_t* model) const = 0;
	virtual int						GetModelType(const model_t* model) const = 0;
	virtual void* GetModelExtraData(const model_t* model) = 0;
	virtual bool					ModelHasMaterialProxy(const model_t* model) const = 0;
	virtual bool					IsTranslucent(model_t const* model) const = 0;
	virtual bool					IsTranslucentTwoPass(const model_t* model) const = 0;
	virtual void					Unused0() {};
	virtual RenderableTranslucencyType_t ComputeTranslucencyType(const model_t* model, int nSkin, int nBody) = 0;
	virtual int						GetModelMaterialCount(const model_t* model) const = 0;
	virtual int 					GetModelMaterials(const model_t* model, int count, IMaterial** ppMaterial) = 0;
	virtual bool					IsModelVertexLit(const model_t* model) const = 0;
	virtual const char* GetModelKeyValueText(const model_t* model) = 0;
	virtual bool					GetModelKeyValue(const model_t* model, CUtlBuffer& buf) = 0; // supports keyvalue blocks in submodels
	virtual float					GetModelRadius(const model_t* model) = 0;

	virtual const studiohdr_t* FindModel(const studiohdr_t* pStudioHdr, void** cache, const char* modelname) const = 0;
	virtual const studiohdr_t* FindModel(void* cache) const = 0;
	virtual	virtualmodel_t* GetVirtualModel(const studiohdr_t* pStudioHdr) const = 0;
	virtual byte* GetAnimBlock(const studiohdr_t* pStudioHdr, int nBlock, bool bPreloadIfMissing) const = 0;
	virtual bool					HasAnimBlockBeenPreloaded(const studiohdr_t* pStudioHdr, int nBlock) const = 0;

	// Available on client only!!!
	virtual void					GetModelMaterialColorAndLighting(const model_t* model, Vector const& origin,
		QAngle const& angles, trace_t* pTrace,
		Vector& lighting, Vector& matColor) = 0;
	virtual void					GetIlluminationPoint(const model_t* model, IClientRenderable* pRenderable, Vector const& origin,
		QAngle const& angles, Vector* pLightingCenter) = 0;

	virtual int						GetModelContents(int modelIndex) const = 0;
	virtual studiohdr_t* GetStudiomodel(const model_t* mod) = 0;
	virtual int						GetModelSpriteWidth(const model_t* model) const = 0;
	virtual int						GetModelSpriteHeight(const model_t* model) const = 0;

	// Sets/gets a map-specified fade range (client only)
	virtual void					SetLevelScreenFadeRange(float flMinSize, float flMaxSize) = 0;
	virtual void					GetLevelScreenFadeRange(float* pMinArea, float* pMaxArea) const = 0;

	// Sets/gets a map-specified per-view fade range (client only)
	virtual void					SetViewScreenFadeRange(float flMinSize, float flMaxSize) = 0;

	// Computes fade alpha based on distance fade + screen fade (client only)
	virtual unsigned char			ComputeLevelScreenFade(const Vector& vecAbsOrigin, float flRadius, float flFadeScale) const = 0;
	virtual unsigned char			ComputeViewScreenFade(const Vector& vecAbsOrigin, float flRadius, float flFadeScale) const = 0;

	// both client and server
	virtual int						GetAutoplayList(const studiohdr_t* pStudioHdr, unsigned short** pAutoplayList) const = 0;

	// Gets a virtual terrain collision model (creates if necessary)
	// NOTE: This may return NULL if the terrain model cannot be virtualized
	virtual CPhysCollide* GetCollideForVirtualTerrain(int index) = 0;

	virtual bool					IsUsingFBTexture(const model_t* model, int nSkin, int nBody, void IClientRenderable* pClientRenderable) const = 0;

	virtual const model_t* FindOrLoadModel(const char* name) const = 0;

	virtual MDLHandle_t				GetCacheHandle(const model_t* model) const = 0;

	// Returns planes of non-nodraw brush model surfaces
	virtual int						GetBrushModelPlaneCount(const model_t* model) const = 0;
	virtual void					GetBrushModelPlane(const model_t* model, int nIndex, cplane_t& plane, Vector* pOrigin) const = 0;
	virtual int						GetSurfacepropsForVirtualTerrain(int index) = 0;
	virtual bool					UsesEnvCubemap(const model_t* model) const = 0;
	virtual bool					UsesStaticLighting(const model_t* model) const = 0;

	// Returns index of model by name, dynamically registered if not already known.
	virtual int						RegisterDynamicModel(const char* name, bool bClientSide) = 0;
	virtual int						RegisterCombinedDynamicModel(const char* pszName, MDLHandle_t Handle) = 0;
	virtual void					UpdateCombinedDynamicModel(int nModelIndex, MDLHandle_t Handle) = 0;

	virtual int						BeginCombinedModel(const char* pszName, bool bReuseExisting) = 0;
	virtual bool					SetCombineModels(int nModelIndex, const CUtlVector< SCombinerModelInput_t >& vecModelsToCombine) = 0;
	virtual bool					FinishCombinedModel(int nModelIndex, CombinedModelLoadedCallback pFunc, void* pUserData = NULL) = 0;
	virtual void					ReleaseCombinedModel(int nModelIndex) = 0;

	virtual bool					IsDynamicModelLoading(int modelIndex) = 0;
	virtual void					AddRefDynamicModel(int modelIndex) = 0;
	virtual void					ReleaseDynamicModel(int modelIndex) = 0;

	// Registers callback for when dynamic model has finished loading.
	virtual bool					RegisterModelLoadCallback(int modelindex, IModelLoadCallback* pCallback, bool bCallImmediatelyIfLoaded = true) = 0;
	virtual void					UnregisterModelLoadCallback(int modelindex, IModelLoadCallback* pCallback) = 0;

	// Poked by engine
	virtual void					OnLevelChange() = 0;

	virtual KeyValues* GetModelKeyValues(const model_t* pModel) = 0;

	virtual void					UpdateViewWeaponModelCache(const char** ppWeaponModels, int nWeaponModels) = 0;
	virtual void					TouchWorldWeaponModelCache(const char** ppWeaponModels, int nWeaponModels) = 0;
};


class IVModelInfoClient : public IVModelInfo
{
public:
	// Returns a model as a client-side index which is stable across server changes
	virtual int						GetModelClientSideIndex(const char* name) const = 0;

	// Poked by engine
	virtual void					OnDynamicModelStringTableChanged(int nStringIndex, const char* pString, const void* pData) = 0;

	// Reference and Unload
	// Don't assume Reference flags - doing this for now
	virtual model_t* ReferenceModel(const char* name) = 0;
	virtual void					UnreferenceModel(model_t* model) = 0;
	virtual void					UnloadUnreferencedModels(void) = 0;
};*/

#endif // !MODELINFO