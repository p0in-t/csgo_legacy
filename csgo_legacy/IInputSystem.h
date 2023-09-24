#pragma once
#include "Includes.h"

#define JOYSTICK_AXIS_INTERNAL( _joystick, _axis ) ( JOYSTICK_FIRST_AXIS + ((_joystick) * MAX_JOYSTICK_AXES) + (_axis) )
#define JOYSTICK_AXIS( _joystick, _axis ) ( (AnalogCode_t)JOYSTICK_AXIS_INTERNAL( _joystick, _axis ) )

enum
{
#ifdef _PS3
	MAX_JOYSTICKS = 7,
#else
	MAX_JOYSTICKS = 4,
#endif
	MOUSE_BUTTON_COUNT = 5,
	MAX_NOVINT_DEVICES = 2,
};

#if !defined( _X360 )
#define INVALID_USER_ID		-1
#else
#define INVALID_USER_ID		XBX_INVALID_USER_ID
#endif

enum JoystickAxis_t
{
	JOY_AXIS_X = 0,
	JOY_AXIS_Y,
	JOY_AXIS_Z,
	JOY_AXIS_R,
	JOY_AXIS_U,
	JOY_AXIS_V,
	MAX_JOYSTICK_AXES,
};

enum AnalogCode_t
{
	ANALOG_CODE_INVALID = -1,
	MOUSE_X = 0,
	MOUSE_Y,
	MOUSE_XY,		// Invoked when either x or y changes
	MOUSE_WHEEL,

	JOYSTICK_FIRST_AXIS,
	JOYSTICK_LAST_AXIS = JOYSTICK_AXIS_INTERNAL(MAX_JOYSTICKS - 1, MAX_JOYSTICK_AXES - 1),

	ANALOG_CODE_LAST,
};

struct InputEvent_t
{
	int m_nType;				// Type of the event (see InputEventType_t)
	int m_nTick;				// Tick on which the event occurred
	int m_nData;				// Generic 32-bit data, what it contains depends on the event
	int m_nData2;				// Generic 32-bit data, what it contains depends on the event
	int m_nData3;				// Generic 32-bit data, what it contains depends on the event
};

struct AppSystemInfo_t
{
	const char* m_pModuleName;
	const char* m_pInterfaceName;
};


//-----------------------------------------------------------------------------
// Client systems are singleton objects in the client codebase responsible for
// various tasks
// The order in which the client systems appear in this list are the
// order in which they are initialized and updated. They are shut down in
// reverse order from which they are initialized.
//-----------------------------------------------------------------------------
enum InitReturnVal_t
{
	INIT_FAILED = 0,
	INIT_OK,

	INIT_LAST_VAL,
};

enum AppSystemTier_t
{
	APP_SYSTEM_TIER0 = 0,
	APP_SYSTEM_TIER1,
	APP_SYSTEM_TIER2,
	APP_SYSTEM_TIER3,

	APP_SYSTEM_TIER_OTHER,
};

abstract_class IAppSystem
{
public:
	// Here's where the app systems get to learn about each other 
	virtual bool Connect(CreateInterfaceFn factory) = 0;
	virtual void Disconnect() = 0;

	// Here's where systems can access other interfaces implemented by this object
	// Returns NULL if it doesn't implement the requested interface
	virtual void* QueryInterface(const char* pInterfaceName) = 0;

	// Init, shutdown
	virtual InitReturnVal_t Init() = 0;
	virtual void Shutdown() = 0;

	// Returns all dependent libraries
	virtual const AppSystemInfo_t* GetDependencies() { return NULL; }

	// Returns the tier
	virtual AppSystemTier_t GetTier() { return APP_SYSTEM_TIER_OTHER; }

	// Reconnect to a particular interface
	virtual void Reconnect(CreateInterfaceFn factory, const char* pInterfaceName) {}
	// Is this appsystem a singleton? (returns false if there can be multiple instances of this interface)
	virtual bool IsSingleton() { return true; }
};

abstract_class IInputSystem : public IAppSystem
{
public:
	/// Attach, detach input system from a particular window
	/// This window should be the root window for the application
	/// Only 1 window should be attached at any given time.
	virtual void AttachToWindow(void* hWnd) = 0;
	virtual void DetachFromWindow() = 0;

	/// Enables/disables input. PollInputState will not update current 
	/// button/analog states when it is called if the system is disabled.
	virtual void EnableInput(bool bEnable) = 0;

	/// Enables/disables the windows message pump. PollInputState will not
	/// Peek/Dispatch messages if this is disabled
	virtual void EnableMessagePump(bool bEnable) = 0;

	/// Polls the current input state
	virtual void PollInputState(bool bIsInGame = false) = 0;

	/// Gets the time of the last polling in ms
	virtual int GetPollTick() const = 0;

	/// Is a button down? "Buttons" are binary-state input devices (mouse buttons, keyboard keys)
	virtual bool IsButtonDown(ButtonCode_t code) const = 0;

	/// Returns the tick at which the button was pressed and released
	virtual int GetButtonPressedTick(ButtonCode_t code) const = 0;
	virtual int GetButtonReleasedTick(ButtonCode_t code) const = 0;

	/// Gets the value of an analog input device this frame
	/// Includes joysticks, mousewheel, mouse
	virtual int GetAnalogValue(AnalogCode_t code) const = 0;

	/// Gets the change in a particular analog input device this frame
	/// Includes joysticks, mousewheel, mouse
	virtual int GetAnalogDelta(AnalogCode_t code) const = 0;

	/// Returns the input events since the last poll
	virtual int GetEventCount() const = 0;
	virtual const InputEvent_t* GetEventData() const = 0;

	// Motion Controller status
	virtual bool	   MotionControllerActive() const = 0;
	virtual void* GetMotionControllerOrientation() const = 0; // Pointer direction
	virtual float	   GetMotionControllerPosX() const = 0;
	virtual float	   GetMotionControllerPosY() const = 0;
	virtual int		   GetMotionControllerDeviceStatus() const = 0;
	virtual std::uint64_t GetMotionControllerDeviceStatusFlags() const = 0;
	virtual void	   SetMotionControllerDeviceStatus(int nStatus) = 0;
	virtual void	   SetMotionControllerCalibrationInvalid(void) = 0;
	virtual void	   StepMotionControllerCalibration(void) = 0;
	virtual void	   ResetMotionControllerScreenCalibration(void) = 0;

	/// Posts a user-defined event into the event queue; this is expected
	/// to be called in overridden wndprocs connected to the root panel.
	virtual void PostUserEvent(const InputEvent_t& event) = 0;

	/// Returns the number of joysticks
	virtual int GetJoystickCount() const = 0;

	/// Enable/disable joystick, it has perf costs
	virtual void EnableJoystickInput(int nJoystick, bool bEnable) = 0;

	/// Enable/disable diagonal joystick POV (simultaneous POV buttons down)
	virtual void EnableJoystickDiagonalPOV(int nJoystick, bool bEnable) = 0;

	/// Sample the joystick and append events to the input queue
	virtual void SampleDevices(void) = 0;

	// FIXME: Currently force-feedback is only supported on the Xbox 360
	virtual void SetRumble(float fLeftMotor, float fRightMotor, int userId = INVALID_USER_ID) = 0;
	virtual void StopRumble(int userId = INVALID_USER_ID) = 0;

	/// Resets the input state
	virtual void ResetInputState() = 0;

	/// Convert back + forth between ButtonCode/AnalogCode + strings
	virtual const char* ButtonCodeToString(ButtonCode_t code) const = 0;
	virtual const char* AnalogCodeToString(AnalogCode_t code) const = 0;
	virtual ButtonCode_t StringToButtonCode(const char* pString) const = 0;
	virtual AnalogCode_t StringToAnalogCode(const char* pString) const = 0;

	/// Sleeps until input happens. Pass a negative number to sleep infinitely
	virtual void SleepUntilInput(int nMaxSleepTimeMS = -1) = 0;

	/// Convert back + forth between virtual codes + button codes
	// FIXME: This is a temporary piece of code
	virtual ButtonCode_t VirtualKeyToButtonCode(int nVirtualKey) const = 0;
	virtual int ButtonCodeToVirtualKey(ButtonCode_t code) const = 0;
	virtual ButtonCode_t ScanCodeToButtonCode(int lParam) const = 0;

	/// How many times have we called PollInputState?
	virtual int GetPollCount() const = 0;

	/// Sets the cursor position
	virtual void SetCursorPosition(int x, int y) = 0;

	/// Tells the input system to generate UI-related events, defined
	/// in inputsystem/inputenums.h (see IE_FirstUIEvent)
	/// We could have multiple clients that care about UI-related events
	/// so we refcount the clients with an Add/Remove strategy. If there
	/// are no interested clients, the UI events are not generated
	virtual void AddUIEventListener() = 0;
	virtual void RemoveUIEventListener() = 0;

	/// Returns the currently attached window
	virtual void* GetAttachedWindow() const = 0;

	/// Creates a cursor using one of the well-known cursor icons
	virtual void* GetStandardCursor(void* id) = 0;

	/// Loads a cursor defined in a file
	virtual void* LoadCursorFromFile(const char* pFileName, const char* pPathID = NULL) = 0;

	/// Sets the cursor icon
	virtual void SetCursorIcon(void* hCursor) = 0;

	/// Gets the cursor position
	virtual void GetCursorPosition(int* pX, int* pY) = 0;

	/// Mouse capture
	virtual void EnableMouseCapture(void* hWnd) = 0;
	virtual void DisableMouseCapture() = 0;

	// Mouse visibility, tell inputsystem when we hide stuff rather than querying the OS which is expensive on OSX
	virtual void SetMouseCursorVisible(bool bVisible) = 0;

	/// Reset the current cursor icon.  Used to reset the icon in the case of alt+tabs where the cursor has been forced to a different
	/// icon because it was outside of the client rect during the reload.
	virtual void ResetCursorIcon() = 0;

	// read and clear accumulated raw input values
	virtual void GetRawMouseAccumulators(int& accumX, int& accumY) = 0;

	// ========================================================================
	// Platform Input Device Interface
	//
	// This section provides a way to determine what Input/controller setup(s) are available on a
	//  given platform (PC, MAC, PS3, XBox) and what input device is currently selected/in use on
	//  the local client
	//
	//  Modules outside of the inputsystem need this information for tasks such as statistics, 
	//  achievements, and player rankings which take into account what input controller setup the 
	//  player is using on a per-platform basis.
	//
	//  The platform can be specified because a dedicate server may be running on a different 
	//    platform than the clients connected to it.
	// 
	// The master list of input devices and platforms used here is located in src\common\input_device.h 
	//
	// The methods here allow the user to write platform agnostic code to iterate through and
	//  and process the list of input devices specific to the current (or specified) platform 
	//  without seeing devices not applicable to that platform.
	// 
	// Terminology:
	//   Connected Device    = Input setup is connected and available for use, can be more than one
	//   Current Device		 = Input setup being actively used
	// 

	// ================================================================
	// Input Device Functions specific to the local client and hardware
	// ================================================================

	// Manage the list of input devices that are connected
	virtual void* GetConnectedInputDevices(void) = 0;				// returns the bitfield of all connected devices
	virtual bool IsInputDeviceConnected(void* device) = 0;
	virtual void SetInputDeviceConnected(void* device, bool connected = true) = 0;
	virtual void* IsOnlySingleDeviceConnected(void) = 0;

	// Access the currently selected Input device
	virtual void* GetCurrentInputDevice(void) = 0;				// returns the enum referring to the one currently selected device
	virtual bool IsDeviceReadingInput(void* device) const = 0;			// returns whether the passed in device is the current device.  Returns true if no current device is defined.
	virtual void SetCurrentInputDevice(void* device) = 0;
	virtual void ResetCurrentInputDevice(void) = 0;						// sets the input device to the platform default

	virtual void SampleInputToFindCurrentDevice(bool) = 0;  // looks for the next 'significant' button press to determine and set the current input device
	virtual bool IsSamplingForCurrentDevice(void) = 0;

	virtual bool IsSteamControllerActive() const = 0;
	virtual void SetSteamControllerMode(const char* pMode, const void* obj = NULL) = 0;

};