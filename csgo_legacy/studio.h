#include "VMatrix.h"
#include "Vector4D.h"
#include "utlvector.h"

#ifndef STUDIOH
#define STUDIOH

#define MAXSTUDIOTRIANGLES	65536	// TODO: tune this
#define MAXSTUDIOVERTS		65536	// TODO: tune this
#define	MAXSTUDIOFLEXVERTS	10000	// max number of verts that can be flexed per mesh.  TODO: tune this

#define MAXSTUDIOSKINS		32		// total textures
#define MAXSTUDIOBONES		256		// total bones actually used
#define MAXSTUDIOFLEXDESC	1024	// maximum number of low level flexes (actual morph targets)
#define MAXSTUDIOFLEXCTRL	96		// maximum number of flexcontrollers (input sliders)
#define MAXSTUDIOPOSEPARAM	24
#define MAXSTUDIOBONECTRLS	4
#define MAXSTUDIOANIMBLOCKS 256

#define MAXSTUDIOBONEBITS	8		// NOTE: MUST MATCH MAXSTUDIOBONES

// NOTE!!! : Changing this number also changes the vtx file format!!!!!
#define MAX_NUM_BONES_PER_VERT 3

//Adrian - Remove this when we completely phase out the old event system.
#define NEW_EVENT_STYLE ( 1 << 10 )

#define BONE_CALCULATE_MASK			0x1F
#define BONE_PHYSICALLY_SIMULATED	0x01	// bone is physically simulated when physics are active
#define BONE_PHYSICS_PROCEDURAL		0x02	// procedural when physics is active
#define BONE_ALWAYS_PROCEDURAL		0x04	// bone is always procedurally animated
#define BONE_SCREEN_ALIGN_SPHERE	0x08	// bone aligns to the screen, not constrained in motion.
#define BONE_SCREEN_ALIGN_CYLINDER	0x10	// bone aligns to the screen, constrained by it's own axis.
#define BONE_WORLD_ALIGN			0x20	// bone is rigidly aligned to the world (but can still translate)

#define BONE_USED_MASK				0x000FFF00
#define BONE_USED_BY_ANYTHING		0x000FFF00
#define BONE_USED_BY_HITBOX			0x00000100	// bone (or child) is used by a hit box
#define BONE_USED_BY_ATTACHMENT		0x00000200	// bone (or child) is used by an attachment point
#define BONE_USED_BY_VERTEX_MASK	0x0003FC00
#define BONE_USED_BY_VERTEX_LOD0	0x00000400	// bone (or child) is used by the toplevel model via skinned vertex
#define BONE_USED_BY_VERTEX_LOD1	0x00000800	
#define BONE_USED_BY_VERTEX_LOD2	0x00001000  
#define BONE_USED_BY_VERTEX_LOD3	0x00002000
#define BONE_USED_BY_VERTEX_LOD4	0x00004000
#define BONE_USED_BY_VERTEX_LOD5	0x00008000
#define BONE_USED_BY_VERTEX_LOD6	0x00010000
#define BONE_USED_BY_VERTEX_LOD7	0x00020000
#define BONE_USED_BY_BONE_MERGE		0x00040000	// bone is available for bone merge to occur against it
#define BONE_ALWAYS_SETUP			0x00080000

#define BONE_USED_BY_VERTEX_AT_LOD(lod) ( BONE_USED_BY_VERTEX_LOD0 << (lod) )
#define BONE_USED_BY_ANYTHING_AT_LOD(lod) ( ( BONE_USED_BY_ANYTHING & ~BONE_USED_BY_VERTEX_MASK ) | BONE_USED_BY_VERTEX_AT_LOD(lod) )

#define MAX_NUM_LODS 8

#define BONE_TYPE_MASK				0x00F00000
#define BONE_FIXED_ALIGNMENT		0x00100000	// bone can't spin 360 degrees, all interpolation is normalized around a fixed orientation

#define BONE_HAS_SAVEFRAME_POS		0x00200000	// Vector48
#define BONE_HAS_SAVEFRAME_ROT64	0x00400000	// Quaternion64
#define BONE_HAS_SAVEFRAME_ROT32	0x00800000	// Quaternion32

class IDataCache;
class IMDLCache;
class CFeModel;
class CSoftbody;
class CSoftbodyEnvironment;

//TODO

struct mstudiobone_t
{
	int					sznameindex;
	inline char* const pszName(void) const { return ((char*)this) + sznameindex; }
	int		 			parent;		// parent bone
	int					bonecontroller[6];	// bone controller index, -1 == none

	// default values
	Vector				pos;
	Quaternion			quat;
	RadianEuler			rot;
	// compression scale
	Vector				posscale;
	Vector				rotscale;

	matrix3x4_t			poseToBone;
	Quaternion			qAlignment;
	int					flags;
	int					proctype;
	int					procindex;		// procedural rule
	mutable int			physicsbone;	// index into physically simulated bone
	inline void* pProcedure() const { if (procindex == 0) return NULL; else return  (void*)(((byte*)this) + procindex); };
	int					surfacepropidx;	// index into string tablefor property name
	inline char* const pszSurfaceProp(void) const { return ((char*)this) + surfacepropidx; }
	inline int			GetSurfaceProp(void) const { return surfacepropLookup; }

	int					contents;		// See BSPFlags.h for the contents flags
	int					surfacepropLookup;	// this index must be cached by the loader, not saved in the file
	int					unused[7];		// remove as appropriate

	mstudiobone_t() {}
};

struct studiohdr_t
{
	int					id;
	int					version;

	int				checksum;		// this has to be the same in the phy and vtx files to load!

	char				name[64];

	int					length;

	Vector				eyeposition;	// ideal eye position

	Vector				illumposition;	// illumination center

	Vector				hull_min;		// ideal movement hull size
	Vector				hull_max;

	Vector				view_bbmin;		// clipping bounding box
	Vector				view_bbmax;

	int					flags;

	int					numbones;			// bones
	int					boneindex;
	inline const mstudiobone_t* pBone(int i) const { return (mstudiobone_t*)(((byte*)this) + boneindex) + i; };

	int					numbonecontrollers;		// bone controllers
	int					bonecontrollerindex;

	int					numhitboxsets;
	int					hitboxsetindex;

	// file local animations? and sequences
//private:
	int					numlocalanim;			// animations/poses
	int					localanimindex;		// animation descriptions

	int					numlocalseq;				// sequences
	int					localseqindex;

	//private:
	mutable int			activitylistversion;	// initialization flag - have the sequences been indexed?
	mutable int			eventsindexed;

	// raw textures
	int					numtextures;
	int					textureindex;

	// raw textures search paths
	int					numcdtextures;
	int					cdtextureindex;
	inline char* pCdtexture(int i) const { return (((char*)this) + *((int*)(((byte*)this) + cdtextureindex) + i)); };

	// replaceable textures tables
	int					numskinref;
	int					numskinfamilies;
	int					skinindex;
	inline short* pSkinref(int i) const { return (short*)(((byte*)this) + skinindex) + i; };

	int					numbodyparts;
	int					bodypartindex;

	// queryable attachable points
//private:
	int					numlocalattachments;
	int					localattachmentindex;
	//public:
	int					GetNumAttachments(void) const;
	int					GetAttachmentBone(int i);
	// used on my tools in hlmv, not persistant
	void				SetAttachmentBone(int iAttachment, int iBone);

	// animation node to animation node transition graph
//private:
	int					numlocalnodes;
	int					localnodeindex;
	int					localnodenameindex;
	inline char* pszLocalNodeName(int iNode) const { return (((char*)this) + *((int*)(((byte*)this) + localnodenameindex) + iNode)); }
	inline byte* pLocalTransition(int i) const { return (byte*)(((byte*)this) + localnodeindex) + i; };

	int					numflexdesc;
	int					flexdescindex;

	int					numflexcontrollers;
	int					flexcontrollerindex;

	int					numflexrules;
	int					flexruleindex;

	int					numikchains;
	int					ikchainindex;

	int					nummouths;
	int					mouthindex;

	//private:
	int					numlocalposeparameters;
	int					localposeparamindex;
	//public:

	int					surfacepropindex;
	inline char* const pszSurfaceProp(void) const { return ((char*)this) + surfacepropindex; }
	inline int			GetSurfaceProp() const { return surfacepropLookup; }

	// Key values
	int					keyvalueindex;
	int					keyvaluesize;
	inline const char* KeyValueText(void) const { return keyvaluesize != 0 ? ((char*)this) + keyvalueindex : NULL; }

	int					numlocalikautoplaylocks;
	int					localikautoplaylockindex;

	// The collision model mass that jay wanted
	float				mass;
	int					contents;

	// external animations, models, etc.
	int					numincludemodels;
	int					includemodelindex;

	// implementation specific back pointer to virtual data. Relocated to studiohdr2_t
	int					unused_virtualModel;

	// for demand loaded animation blocks
	int					szanimblocknameindex;
	inline char* const pszAnimBlockName(void) const { return ((char*)this) + szanimblocknameindex; }
	int					numanimblocks;
	int					animblockindex;

	// Relocated to studiohdr2_t
	int					unused_animblockModel;

	int					bonetablebynameindex;
	inline const byte* GetBoneTableSortedByName() const { return (byte*)this + bonetablebynameindex; }

	// used by tools only that don't cache, but persist mdl's peer data
	// engine uses virtualModel to back link to cache pointers
	// Relocated to studiohdr2_t
	int                 unused_pVertexBase;
	int                 unused_pIndexBase;

	// if STUDIOHDR_FLAGS_CONSTANT_DIRECTIONAL_LIGHT_DOT is set,
	// this value is used to calculate directional components of lighting 
	// on static props
	byte				constdirectionallightdot;

	// set during load of mdl data to track *desired* lod configuration (not actual)
	// the *actual* clamped root lod is found in studiohwdata
	// this is stored here as a global store to ensure the staged loading matches the rendering
	byte				rootLOD;

	// set in the mdl data to specify that lod configuration should only allow first numAllowRootLODs
	// to be set as root LOD:
	//	numAllowedRootLODs = 0	means no restriction, any lod can be set as root lod.
	//	numAllowedRootLODs = N	means that lod0 - lod(N-1) can be set as root lod, but not lodN or lower.
	byte				numAllowedRootLODs;

	byte				unused[1];

	int					unused4; // zero out if version < 47

	int					numflexcontrollerui;
	int					flexcontrolleruiindex;

	float				flVertAnimFixedPointScale;

	mutable int			surfacepropLookup;	// this index must be cached by the loader, not saved in the file

	// FIXME: Remove when we up the model version. Move all fields of studiohdr2_t into studiohdr_t.
	int					studiohdr2index;

	// NOTE: No room to add stuff? Up the .mdl file format version 
	// [and move all fields in studiohdr2_t into studiohdr_t and kill studiohdr2_t],
	// or add your stuff to studiohdr2_t. See NumSrcBoneTransforms/SrcBoneTransform for the pattern to use.
	int					unused2[1];
};

class CStudioHdr
{
public:
	CStudioHdr(void);

public:
	inline bool IsVirtual(void) { return (m_pVModel != NULL); };
	inline bool IsValid(void) { return (m_pStudioHdr != NULL); };
	inline bool IsReadyForAccess(void) const { return (m_pStudioHdr != NULL); };
	inline const studiohdr_t* GetRenderHdr(void) const { return m_pStudioHdr; };
	CSoftbody* GetSoftbody() const { return m_pSoftbody; }
	void SetSoftbody(CSoftbody* pSoftbody) { m_pSoftbody = pSoftbody; }
private:
	studiohdr_t* m_pStudioHdr;
	void* m_pVModel;
	CSoftbody* m_pSoftbody;
	CUtlVector< const studiohdr_t* > m_pStudioHdrCache;

	int	m_nFrameUnlockCounter;
	int* m_pFrameUnlockCounter;
public:
	inline int			numbones(void) const { return m_pStudioHdr->numbones; };
	inline const mstudiobone_t* pBone(int i) const { return m_pStudioHdr->pBone(i); };

	inline int			GetNumBoneControllers(void) const { return m_pStudioHdr->numbonecontrollers; };

	inline int			numikchains() const { return m_pStudioHdr->numikchains; };
	inline int			GetNumIKChains(void) const { return m_pStudioHdr->numikchains; };

	inline int			numflexrules() const { return m_pStudioHdr->numflexrules; };

	inline int			numflexdesc() const { return m_pStudioHdr->numflexdesc; };

	inline int			numflexcontrollerui() const { return m_pStudioHdr->numflexcontrollerui; };

	inline int			numbonecontrollers() const { return m_pStudioHdr->numbonecontrollers; };

	inline int			numhitboxsets() const { return m_pStudioHdr->numhitboxsets; };


	inline int			numbodyparts() const { return m_pStudioHdr->numbodyparts; };

	inline int			numskinfamilies() const { return m_pStudioHdr->numskinfamilies; }

	inline Vector		eyeposition() const { return m_pStudioHdr->eyeposition; };

	inline int			flags() const { return m_pStudioHdr->flags; };

	inline char* const pszSurfaceProp(void) const { return m_pStudioHdr->pszSurfaceProp(); };
	inline int			GetSurfaceProp()const { return m_pStudioHdr->surfacepropLookup; }

	inline float		mass() const { return m_pStudioHdr->mass; };
	inline int			contents() const { return m_pStudioHdr->contents; }

	inline const byte* GetBoneTableSortedByName() const { return m_pStudioHdr->GetBoneTableSortedByName(); };

	inline Vector		illumposition() const { return m_pStudioHdr->illumposition; };

	inline Vector		hull_min() const { return m_pStudioHdr->hull_min; };		// ideal movement hull size
	inline Vector		hull_max() const { return m_pStudioHdr->hull_max; };

	inline Vector		view_bbmin() const { return m_pStudioHdr->view_bbmin; };		// clipping bounding box
	inline Vector		view_bbmax() const { return m_pStudioHdr->view_bbmax; };

	inline int			numtextures() const { return m_pStudioHdr->numtextures; };
public:
	inline int boneFlags(int iBone) const { return m_boneFlags[iBone]; }
	inline int boneParent(int iBone) const { return m_boneParent[iBone]; }

private:
	CUtlVector< int >  m_boneFlags;
	CUtlVector< int >  m_boneParent;
};

#endif