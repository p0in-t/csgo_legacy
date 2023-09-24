#pragma once
#include "CGlobalVarsBase.h"
#include "ClientClass.h"
#include "bitbuf.h"

struct vrect_t
{
	int				x, y, width, height;
	vrect_t* pnext;
};

enum ClientFrameStage_t
{
	FRAME_UNDEFINED = -1,			// (haven't run any frames yet)
	FRAME_START,

	// A network packet is being recieved
	FRAME_NET_UPDATE_START,
	// Data has been received and we're going to start calling PostDataUpdate
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	// Data has been received and we've called PostDataUpdate on all data recipients
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	// We've received all packets, we can now do interpolation, prediction, etc..
	FRAME_NET_UPDATE_END,

	// We're about to start rendering the scene
	FRAME_RENDER_START,
	// We've finished rendering the scene.
	FRAME_RENDER_END,

	FRAME_NET_FULL_FRAME_UPDATE_ON_REMOVE
};

// Used by RenderView
enum RenderViewInfo_t
{
	RENDERVIEW_UNSPECIFIED = 0,
	RENDERVIEW_DRAWVIEWMODEL = (1 << 0),
	RENDERVIEW_DRAWHUD = (1 << 1),
	RENDERVIEW_SUPPRESSMONITORRENDERING = (1 << 2),
};

abstract_class IBaseClientDLL //implemented in chlclient
{
public:
	// Connect appsystem components, get global interfaces, don't run any other init code
	virtual int				Connect(CreateInterfaceFn appSystemFactory, CGlobalVarsBase * pGlobals) = 0;
	virtual void            Disconnect() = 0;

	// run other init code here
	virtual int				Init(CreateInterfaceFn appSystemFactory, CGlobalVarsBase* pGlobals) = 0;

	virtual void			PostInit() = 0;

	// Called once when the client DLL is being unloaded
	virtual void			Shutdown(void) = 0;

	// Called at the start of each level change
	virtual void			LevelInitPreEntity(char const* pMapName) = 0;
	// Called at the start of a new level, after the entities have been received and created
	virtual void			LevelInitPostEntity() = 0;
	// Called at the end of a level
	virtual void			LevelShutdown(void) = 0;

	// Request a pointer to the list of client datatable classes
	virtual ClientClass* GetAllClasses(void) = 0;

	// Called once per level to re-initialize any hud element drawing stuff
	virtual int				HudVidInit(void) = 0;
	// Called by the engine when gathering user input
	virtual void			HudProcessInput(bool bActive) = 0;
	// Called oncer per frame to allow the hud elements to think
	virtual void			HudUpdate(bool bActive) = 0;
	// Reset the hud elements to their initial states
	virtual void			HudReset(void) = 0;
	// Display a hud text message
	virtual void			HudText(const char* message) = 0;

	// Draw the console overlay?
	virtual bool			ShouldDrawDropdownConsole() = 0;

	// Mouse Input Interfaces
	// Activate the mouse (hides the cursor and locks it to the center of the screen)
	virtual void			IN_ActivateMouse(void) = 0;
	// Deactivates the mouse (shows the cursor and unlocks it)
	virtual void			IN_DeactivateMouse(void) = 0;
	// This is only called during extra sound updates and just accumulates mouse x, y offets and recenters the mouse.
	//  This call is used to try to prevent the mouse from appearing out of the side of a windowed version of the engine if 
	//  rendering or other processing is taking too long
	virtual void			IN_Accumulate(void) = 0;
	// Reset all key and mouse states to their initial, unpressed state
	virtual void			IN_ClearStates(void) = 0;
	// If key is found by name, returns whether it's being held down in isdown, otherwise function returns false
	virtual bool			IN_IsKeyDown(const char* name, bool& isdown) = 0;
	// Raw keyboard signal, if the client .dll returns 1, the engine processes the key as usual, otherwise,
	//  if the client .dll returns 0, the key is swallowed.
	virtual int				IN_KeyEvent(int eventcode, ButtonCode_t keynum, const char* pszCurrentBinding) = 0;

	// This function is called once per tick to create the player CUserCmd (used for prediction/physics simulation of the player)
	// Because the mouse can be sampled at greater than the tick interval, there is a separate input_sample_frametime, which
	//  specifies how much additional mouse / keyboard simulation to perform.
	virtual void			CreateMove(
								int sequence_number,			// sequence_number of this cmd
								float input_sample_frametime,	// Frametime for mouse input sampling
								bool active) = 0;				// True if the player is active (not paused)

	// If the game is running faster than the tick_interval framerate, then we do extra mouse sampling to avoid jittery input
	//  This code path is much like the normal move creation code, except no move is created
	virtual void			ExtraMouseSample(float frametime, bool active) = 0;

	// Encode the delta (changes) between the CUserCmd in slot from vs the one in slot to.  The game code will have
	//  matching logic to read the delta.
	virtual bool			WriteUsercmdDeltaToBuffer(int nSlot, bf_write* buf, int from, int to, bool isnewcommand) = 0;
	// Demos need to be able to encode/decode CUserCmds to memory buffers, so these functions wrap that
	virtual void			EncodeUserCmdToBuffer(int nSlot, bf_write& buf, int slot) = 0;
	virtual void			DecodeUserCmdFromBuffer(int nSlot, bf_read& buf, int slot) = 0;

	// Set up and render one or more views (e.g., rear view window, etc.).  This called into RenderView below
	virtual void			View_Render(vrect_t* rect) = 0;

	// Allow engine to expressly render a view (e.g., during timerefresh)
	// See IVRenderView.h, PushViewFlags_t for nFlags values
	virtual void			RenderView(const void* view, int nClearFlags, int whatToDraw) = 0;

	// Apply screen fade directly from engine
	virtual void			View_Fade(void* pSF) = 0;

	// The engine has parsed a crosshair angle message, this function is called to dispatch the new crosshair angle
	virtual void			SetCrosshairAngle(const QAngle& angle) = 0;

	// Sprite (.spr) model handling code
	// Load a .spr file by name
	virtual void			InitSprite(void* pSprite, const char* loadname) = 0;
	// Shutdown a .spr file
	virtual void			ShutdownSprite(void* pSprite) = 0;
	// Returns sizeof( CEngineSprite ) so the engine can allocate appropriate memory
	virtual int				GetSpriteSize(void) const = 0;

	// Called when a player starts or stops talking.
	// entindex is -1 to represent the local client talking (before the data comes back from the server). 
	// entindex is -2 to represent the local client's voice being acked by the server.
	// entindex is GetPlayer() when the server acknowledges that the local client is talking.
	virtual void			VoiceStatus(int entindex, int iSsSlot, bool bTalking) = 0;

	// returns false if the player can't hear the other client due to game rules (eg. the other team)
	virtual bool			PlayerAudible(int iPlayerIndex) = 0;

	// Networked string table definitions have arrived, allow client .dll to 
	//  hook string changes with a callback function ( see INetworkStringTableClient.h )
	virtual void			InstallStringTableCallback(char const* tableName) = 0;

	// Notification that we're moving into another stage during the frame.
	virtual void			FrameStageNotify(ClientFrameStage_t curStage) = 0;

	// The engine has received the specified user message, this code is used to dispatch the message handler
	virtual bool			DispatchUserMessage(int msg_type, std::int32_t nFlags, int size, const void* msg) = 0;	// New protobuf version

	// Save/restore system hooks
	virtual CSaveRestoreData* SaveInit(int size) = 0;
	virtual void			SaveWriteFields(CSaveRestoreData*, const char*, void*, void*, void*, int) = 0;
	virtual void			SaveReadFields(CSaveRestoreData*, const char*, void*, void*, void*, int) = 0;
	virtual void			PreSave(CSaveRestoreData*) = 0;
	virtual void			Save(CSaveRestoreData*) = 0;
	virtual void			WriteSaveHeaders(CSaveRestoreData*) = 0;
	virtual void			ReadRestoreHeaders(CSaveRestoreData*) = 0;
	virtual void			Restore(CSaveRestoreData*, bool) = 0;
	virtual void			DispatchOnRestore() = 0;

	// Hand over the StandardRecvProxies in the client DLL's module.
	virtual CStandardRecvProxies* GetStandardRecvProxies() = 0;

	// save game screenshot writing
	virtual void			WriteSaveGameScreenshot(const char* pFilename) = 0;

	// Given a list of "S(wavname) S(wavname2)" tokens, look up the localized text and emit
	//  the appropriate close caption if running with closecaption = 1
	virtual void			EmitSentenceCloseCaption(char const* tokenstream) = 0;
	// Emits a regular close caption by token name
	virtual void			EmitCloseCaption(char const* captionname, float duration) = 0;

	// Returns true if the client can start recording a demo now.  If the client returns false,
	// an error message of up to length bytes should be returned in errorMsg.
	virtual bool			CanRecordDemo(char* errorMsg, int length) const = 0;

	// Give the Client a chance to do setup/cleanup.
	virtual void			OnDemoRecordStart(char const* pDemoBaseName) = 0;
	virtual void			OnDemoRecordStop() = 0;
	virtual void			OnDemoPlaybackStart(char const* pDemoBaseName) = 0;
	virtual void			OnDemoPlaybackRestart() = 0;
	virtual void			OnDemoPlaybackStop() = 0;
	virtual void			SetDemoPlaybackHighlightXuid(std::uint64_t xuid, bool bLowlights) = 0;
	virtual void			ShowHighlightSkippingMessage(bool bState, int nCurrentTick = 0, int nTickStart = 0, int nTickStop = 0) = 0;

	// Demo polish callbacks.
	virtual void			RecordDemoPolishUserInput(int nCmdIndex) = 0;

	// Cache replay ragdolls
	virtual bool			CacheReplayRagdolls(const char* pFilename, int nStartTick) = 0;

	// Send a message to the Replay UI
	virtual void			ReplayUI_SendMessage(void* pMsg) = 0;

	// Get the client replay factory
	virtual void* GetReplayFactory() = 0;

	// Clear out the local player's replay pointer so it doesn't get deleted
	virtual void			ClearLocalPlayerReplayPtr() = 0;

	// Get client screen dimensions
	virtual int				GetScreenWidth() = 0;
	virtual int				GetScreenHeight() = 0;

	// Added interface
	// save game screenshot writing
	virtual void			WriteSaveGameScreenshotOfSize(const char* pFilename, int width, int height, bool bCreatePowerOf2Padded = false, bool bWriteVTF = false) = 0;

	// Write a .VTF screenshot to disk for the replay system
	virtual void			WriteReplayScreenshot(void* params) = 0;

	// Reallocate memory for replay screenshots - called if user changes resolution or if the convar "replay_screenshotresolution" changes
	virtual void			UpdateReplayScreenshotCache() = 0;

	// Gets the current view
	virtual bool			GetPlayerView(void* playerView) = 0;

	virtual bool			ShouldHideLoadingPlaque(void) = 0;

	virtual void			InvalidateMdlCache() = 0;

	virtual void			IN_SetSampleTime(float frametime) = 0;

	virtual void			OnActiveSplitscreenPlayerChanged(int nNewSlot) = 0;
	// We are entering into/leaving split screen mode (or # of players is changing)
	virtual void			OnSplitScreenStateChanged() = 0;

	virtual int				GetSpectatorTarget(void* pObserverMode) = 0;

	virtual void			CenterStringOff() = 0;

	virtual void			OnScreenSizeChanged(int nOldWidth, int nOldHeight) = 0;

	virtual void InstantiateMaterialProxy(const char* proxyName) = 0;

	virtual void	GetFullscreenClientDLLVPanel(void) = 0;

	// The engine wants to mark two entities as touching
	virtual void			MarkEntitiesAsTouching(void* e1, void* e2) = 0;

	virtual void			OnKeyBindingChanged(ButtonCode_t buttonCode, char const* pchKeyName, char const* pchNewBinding) = 0;

	virtual void			SetBlurFade(float scale) = 0;

	virtual void			ResetHudCloseCaption() = 0;

	// Called by the engine to allow the new GameUI to handle key events
	// Function must return true if the key event was handled
	virtual bool			HandleGameUIEvent(const void* event) = 0;

	// check if the client needs to alter non-entity associated sound locations (portal2, multiple sources, etc.)
	virtual bool			GetSoundSpatialization(void* info) = 0;

	// sending a hint to the hud
	virtual void			Hud_SaveStarted() = 0;

	virtual void			ShutdownMovies() = 0;

	virtual void			GetStatus(char* buffer, int bufsize) = 0;

	virtual bool			IsBuildWRThreaded(void) = 0;
	virtual void			QueueBuildWorldListJob(void* pJob) = 0;
	virtual void			CacheFrustumData(const void* frustum, const void*, void*, int) = 0;
	virtual const void* GetBuildViewFrustum(void) const = 0;
	virtual const void* GetBuildViewAeraFrustums(void) const = 0;

	virtual bool IsSubscribedMap(const char* pchMapName, bool bOnlyOnDisk) = 0;
	virtual bool IsFeaturedMap(const char* pchMapName, bool bOnlyOnDisk) = 0;

	// Gets file info for a workshop map, downloads latest version if needed
	virtual void DownloadCommunityMapFile(void* id) = 0;

	// Return download progress from 0.0 - 1.0, or -1.0 on error
	virtual float GetUGCFileDownloadProgress(void* id) = 0;

	virtual void RecordUIEvent(const char* szEvent) = 0;

	// Inventory access
	virtual float FindInventoryItemWithMaxAttributeValue(char const* szItemType, char const* szAttrClass) = 0;

	// Inventory access
	virtual void DetermineSubscriptionKvToAdvertise(void** kvLocalPlayer) = 0;

	// Evidence validation
	virtual bool ValidateSignedEvidenceHeader(char const* szKey, void const* pvHeader, void** pPlaybackParameters) = 0;
	virtual void PrepareSignedEvidenceData(void* pvData, int numBytes, void const* pPlaybackParameters) = 0;
	virtual bool ShouldSkipEvidencePlayback(void const* pPlaybackParameters) = 0;

	// Scaleform slot controller
	virtual void* GetScaleformSlotInitController() = 0;

	virtual bool IsConnectedUserInfoChangeAllowed(void* pCvar) = 0;

	virtual char const* GetRichPresenceStatusString() = 0;

	// Returns true if the client can stop recording a demo now.  If the client returns false,
	// an error message of up to length bytes should be returned in errorMsg.
	virtual bool			CanStopRecordDemo(char* errorMsg, int length) const = 0;

	virtual void OnHltvReplay(const void* msg) = 0;
	virtual void OnHltvReplayTick() = 0;
	virtual int GetHltvReplayDelay() = 0;

	virtual void OnDemoPlaybackTimeJump() = 0;
	virtual void OnCommandDuringPlayback(char const* cmd) = 0;

	virtual void RetireAllPlayerDecals(bool bRenderContextValid) = 0;
	virtual void EngineGotvSyncPacket(const void* pPkt) = 0; // Engine asking GC for a sync packet
	virtual int GetInEyeEntity() const = 0;
	virtual void OnTickPre(int tickcount) = 0;
};