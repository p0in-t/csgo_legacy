#pragma once
#include "NetvarManager.h"
#include "Interfaces.h"
#include "Offsets.h"
#include "studio.h"
#include "bone_accessor.h"

#define MAX_AREA_STATE_BYTES		32
#define MAX_AREA_PORTAL_STATE_BYTES 24
#define GetLocal() reinterpret_cast<CBasePlayer*>(I.g_pClientEntityList->GetClientEntity(I.g_pEngineClient->GetLocalPlayer()))

// -------------------------------------------------------------------------------------------------------------- //
// CInterpolatedVar.
// -------------------------------------------------------------------------------------------------------------- //


class CPlayerLocalData
{
public:
	unsigned char			m_chAreaBits[MAX_AREA_STATE_BYTES];				// Area visibility flags.
	unsigned char			m_chAreaPortalBits[MAX_AREA_PORTAL_STATE_BYTES];// Area portal visibility flags.

	// BEGIN PREDICTION DATA COMPACTION (these fields are together to allow for faster copying in prediction system)
	int						m_nStepside;
	int						m_nOldButtons;
	float					m_flFOVRate;		// rate at which the FOV changes

	int						m_iHideHUD;			// bitfields containing sections of the HUD to hide
	int						m_nDuckTimeMsecs;
	int						m_nDuckJumpTimeMsecs;
	int						m_nJumpTimeMsecs;

	float					m_flFallVelocity;
	float					m_flOldFallVelocity;
	float					m_flStepSize;

	//probs needs padding here

	bool					m_bDucked;			// Set exactly between FinishDuck() and FinishUnDuck(); marks that our position may have been moved by ducking
	bool					m_bDucking;			// Set if we are currently in a duck transition (that is, m_bDucked != the state of the user-pressed duck button)
	float					m_flLastDuckTime;	// last time the player pressed duck

	bool					m_bInDuckJump;
	bool					m_bDrawViewmodel;
	bool					m_bWearingSuit;
	bool					m_bPoisoned;
	bool					m_bAllowAutoMovement;
	// END PREDICTION DATA COMPACTION

	bool					m_bInLanding;
	float					m_flLandingTime;
};

class CBaseEntity : public IClientEntity
{
public:
	Netvar(Spotted, bool, "DT_BaseEntity:m_bSpotted");
	Netvar(GetSimTime, float, "DT_BaseEntity:m_flSimulationTime");
	Netvar(GetCellBits, int, "DT_BaseEntity:m_cellbits");
	Netvar(GetCellX, int, "DT_BaseEntity:m_cellX");
	Netvar(GetCellY, int, "DT_BaseEntity:m_cellY");
	Netvar(GetCellZ, int, "DT_BaseEntity:m_cellZ");
	Netvar(GetVecOrigin, Vector, "DT_BaseEntity:m_vecOrigin");
	Netvar(GetAngRotation, QAngle, "DT_BaseEntity:m_angRotation");
	Netvar(GetModelIndex, int, "DT_BaseEntity:m_nModelIndex");
	Netvar(GetTeamNum, int, "DT_BaseEntity:m_iTeamNum");
	Netvar(GetName, char*, "DT_BaseEntity:m_iName");

	IClientUnknown* GetIClientUnknown()
	{
		return this;
	}

	bool IsDormant()
	{
		IClientUnknown* unknown = GetIClientUnknown();
		IClientNetworkable* networkable = unknown->GetClientNetworkable();

		return networkable->IsDormant();
	}
};

class CBaseAnimating : public CBaseEntity
{
public:
	Netvar(m_nSequence, int, "DT_BaseAnimating:m_nSequence");
	Netvar(m_nForceBone, int, "DT_BaseAnimating:m_nForceBone");
	Netvar(m_vecForce, Vector, "DT_BaseAnimating:m_vecForce");
	Netvar(m_bClientSideAnimation, int, "DT_BaseAnimating:m_bClientSideAnimation");

	//CStudioHdr* GetStudioHdrPtr() //0x293C found in getmodelptr
	//{
	//	return **reinterpret_cast<CStudioHdr***>((DWORD)this + 0x293C);
	//}

	//CStudioHdr* GetModelPtr()
	//{
	//	CStudioHdr* m_pStudioHdr = GetStudioHdrPtr();

	//	using LockStudioHdrFn = void(__thiscall*)(CBaseAnimating*);
	//	LockStudioHdrFn LockStudioHdr = reinterpret_cast<LockStudioHdrFn>(g_Pattern.GetLockStudioHdr());

	//	if (!m_pStudioHdr && GetModel())
	//	{
	//		LockStudioHdr(this);
	//	}

	//	return (m_pStudioHdr && m_pStudioHdr->IsValid()) ? m_pStudioHdr : NULL;
	//}

	Vector GetBonePosition(int iBone)
	{
		matrix3x4a_t boneMatrix[MAXSTUDIOBONES];

		if (this->SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, I.g_pGlobals->curtime))
		{
			Vector bone(boneMatrix[iBone][0][3], boneMatrix[iBone][1][3], boneMatrix[iBone][2][3]);

			return bone;
		}

		return Vector(NULL);
	}

	bool ComputeHitboxSurroundingBox(Vector* pVecWorldMins, Vector* pVecWorldMaxs)
	{
		using GetHitboxSurroundingBoxFn = void(__thiscall*)(CBaseAnimating*, Vector*, Vector*);
		GetHitboxSurroundingBoxFn ComputeHitboxSurroundingBox = reinterpret_cast<GetHitboxSurroundingBoxFn>(g_Pattern.GetHitboxSurroundingBox() + 1);

		ComputeHitboxSurroundingBox(this, pVecWorldMins, pVecWorldMaxs);
	}
};

class CBasePlayer : public CBaseAnimating
{
public:
	Netvar(GetLocalData, CPlayerLocalData, "DT_BasePlayer:localdata");
	Netvar(GetFov, int, "DT_BasePlayer:m_iFOV");
	Netvar(GetHealth, int, "DT_BasePlayer:m_iHealth");
	Netvar(GetAmmo, int, "DT_BasePlayer:m_iAmmo");
	Netvar(m_fFlags, int, "DT_BasePlayer:m_fFlags");
	Netvar(m_iObserverMode, int, "DT_BasePlayer:m_iObserverMode");
	Netvar(m_flDuckAmount, float, "DT_BasePlayer:m_flDuckAmount");
	Netvar(m_flDuckSpeed, float, "DT_BasePlayer:m_flDuckSpeed");
};