#pragma once
#include "Color.h"
#include "Includes.h"

abstract_class C_PlayerResource
{
public:

	// Team data access 
	virtual int		GetTeamScore(int index);
	virtual const char* GetTeamName(int index);
	virtual const Color& GetTeamColor(int index);

	// Player data access
	virtual bool	IsConnected(int index);
	virtual bool	IsAlive(int index);
	virtual bool	IsFakePlayer(int index);
	virtual bool	IsLocalPlayer(int index);
	virtual bool	IsHLTV(int index);

	virtual const char* GetPlayerName(int index);
	virtual int		GetPing(int index);
	//	virtual int		GetPacketloss( int index );
	virtual int		GetKills(int index);
	virtual int		GetAssists(int index);
	virtual int		GetDeaths(int index);
	virtual int		GetTeam(int index);
	virtual int		GetPendingTeam(int index);
	virtual int		GetFrags(int index);
	virtual int		GetHealth(int index);
	virtual int		GetCoachingTeam(int index);

	virtual void	ClientThink();
	virtual	void	OnDataChanged(void* updateType);
	virtual void	DeviceLost(void);
	virtual void	DeviceReset(void* pDevice, void* pPresentParameters, void* pHWnd);
	virtual void	ScreenSizeChanged(int width, int height) { }
	virtual void	TeamChanged(void) { }
};