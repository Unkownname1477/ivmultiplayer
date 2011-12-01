//============== IV: Multiplayer - http://code.iv-multiplayer.com ==============
//
// File: CIVPad.cpp
// Project: Client.Core
// Author(s): jenksta
// License: See LICENSE in root directory
//
//==============================================================================

#include "CIVPad.h"
#include "COffsets.h"

// Helper macros for the ToControlState function
#define SET_ANALOG_KEY(key, analog) \
	analog = (bCurrent ? m_pPad->m_padData[key].m_byteCurrentValue : m_pPad->m_padData[key].m_byteLastValue)

#define SET_BINARY_KEY(key, binary) \
	binary = (bCurrent ? ((m_pPad->m_padData[key].m_byteCurrentValue == MAX_INPUT_VALUE) ? true : false) : ((m_pPad->m_padData[key].m_byteLastValue == MAX_INPUT_VALUE) ? true : false))

// Helper macros for the FromControlState function
#define GET_ANALOG_KEY(key, analog) \
	if(bCurrent) { m_pPad->m_padData[key].m_byteCurrentValue = analog; } else { m_pPad->m_padData[key].m_byteLastValue = analog; }

#define GET_BINARY_KEY(key, binary) \
	if(bCurrent) { m_pPad->m_padData[key].m_byteCurrentValue = (binary ? MAX_INPUT_VALUE : DEFAULT_BINARY_INPUT_VALUE); } else { m_pPad->m_padData[key].m_byteLastValue = (binary ? MAX_INPUT_VALUE : DEFAULT_BINARY_INPUT_VALUE); }

CIVPad::CIVPad()
{
	// Flag ourself as created by ourself
	m_bCreatedByUs = true;

	// Allocate the new pad
	m_pPad = new IVPad;

	// Call the CPad constructor
	IVPad * pPad = m_pPad;
	_asm
	{
		mov ecx, pPad
		call COffsets::FUNC_CPad__Constructor
	}

	// Call CPad::Initialize
	_asm
	{
		push 0
		mov ecx, pPad
		call COffsets::FUNC_CPad__Initialize
	}

	// HACK: To fix values not being initialized properly
	for(int i = 0; i < INPUT_COUNT; i++)
	{
		m_pPad->m_padData[i].m_byteUnknown4 = MAX_INPUT_VALUE;

		if(IsAnalogInput((eInput)i))
		{
			m_pPad->m_padData[i].m_byteCurrentValue = DEFAULT_ANALOG_INPUT_VALUE;
			m_pPad->m_padData[i].m_byteLastValue = DEFAULT_ANALOG_INPUT_VALUE;
			//CLogFile::Printf("Analog input %d values are %d and %d", i, m_pPad->m_padData[i].m_byteCurrentValue, m_pPad->m_padData[i].m_byteLastValue);
		}
	}
}

CIVPad::CIVPad(IVPad * pPad)
{
	m_bCreatedByUs = false;
	m_pPad = pPad;
}

CIVPad::~CIVPad()
{
	// Has the pad been created by us?
	if(m_bCreatedByUs)
	{
		// Call the CPad destructor
		IVPad * pPad = m_pPad;
		_asm
		{
			mov ecx, pPad
			call COffsets::FUNC_CPad__Destructor
		}

		// Delete the pad
		SAFE_DELETE(m_pPad);
	}
}

void CIVPad::SetPad(IVPad * pPad)
{
	m_pPad = pPad;
}

IVPad * CIVPad::GetPad()
{
	return m_pPad;
}

void CIVPad::ToControlState(CControlState& controlState, bool bCurrent)
{
	// Do we not have a valid pad?
	if(!m_pPad)
		return;

	// Analog keys
	SET_ANALOG_KEY(INPUT_MOVE_LEFT,         controlState.ucOnFootMove[0]);
	SET_ANALOG_KEY(INPUT_MOVE_RIGHT,        controlState.ucOnFootMove[1]);
	SET_ANALOG_KEY(INPUT_MOVE_UP,           controlState.ucOnFootMove[2]);
	SET_ANALOG_KEY(INPUT_MOVE_DOWN,         controlState.ucOnFootMove[3]);
	SET_ANALOG_KEY(INPUT_VEH_MOVE_LEFT,     controlState.ucInVehicleMove[0]);
	SET_ANALOG_KEY(INPUT_VEH_MOVE_RIGHT,    controlState.ucInVehicleMove[1]);
	SET_ANALOG_KEY(INPUT_VEH_MOVE_UP,       controlState.ucInVehicleMove[2]);
	SET_ANALOG_KEY(INPUT_VEH_MOVE_DOWN,     controlState.ucInVehicleMove[3]);
	SET_ANALOG_KEY(INPUT_VEH_BRAKE,         controlState.ucInVehicleTriggers[0]);
	SET_ANALOG_KEY(INPUT_VEH_ACCELERATE,    controlState.ucInVehicleTriggers[1]);

	// Binary keys
	SET_BINARY_KEY(INPUT_ENTER,             controlState.keys.bEnterExitVehicle);
	SET_BINARY_KEY(INPUT_SPRINT,            controlState.keys.bSprint);
	SET_BINARY_KEY(INPUT_JUMP,              controlState.keys.bJump);
	SET_BINARY_KEY(INPUT_ATTACK,            controlState.keys.bAttack);
	SET_BINARY_KEY(INPUT_ATTACK2,           controlState.keys.bAttack2);
	SET_BINARY_KEY(INPUT_AIM,               controlState.keys.bAim);
	SET_BINARY_KEY(INPUT_FREE_AIM,          controlState.keys.bFreeAim);
	SET_BINARY_KEY(INPUT_MELEE_ATTACK1,     controlState.keys.bMeleeAttack1);
	SET_BINARY_KEY(INPUT_MELEE_ATTACK2,     controlState.keys.bMeleeAttack2);
	SET_BINARY_KEY(INPUT_MELEE_KICK,        controlState.keys.bMeleeKick);
	SET_BINARY_KEY(INPUT_MELEE_BLOCK,       controlState.keys.bMeleeBlock);
	SET_BINARY_KEY(INPUT_VEH_HANDBRAKE,     controlState.keys.bHandbrake);
	SET_BINARY_KEY(INPUT_VEH_HANDBRAKE_ALT, controlState.keys.bHandbrake2);
	SET_BINARY_KEY(INPUT_VEH_HORN,          controlState.keys.bHorn);
	SET_BINARY_KEY(INPUT_VEH_ATTACK,        controlState.keys.bDriveBy);
	SET_BINARY_KEY(INPUT_VEH_ATTACK2,       controlState.keys.bHeliPrimaryFire);
}

void CIVPad::FromControlState(CControlState controlState, bool bCurrent)
{
	// Do we not have a valid pad?
	if(!m_pPad)
		return;

	// Analog keys
	GET_ANALOG_KEY(INPUT_MOVE_LEFT,         controlState.ucOnFootMove[0]);
	GET_ANALOG_KEY(INPUT_MOVE_RIGHT,        controlState.ucOnFootMove[1]);
	GET_ANALOG_KEY(INPUT_MOVE_UP,           controlState.ucOnFootMove[2]);
	GET_ANALOG_KEY(INPUT_MOVE_DOWN,         controlState.ucOnFootMove[3]);
	GET_ANALOG_KEY(INPUT_VEH_MOVE_LEFT,     controlState.ucInVehicleMove[0]);
	GET_ANALOG_KEY(INPUT_VEH_MOVE_RIGHT,    controlState.ucInVehicleMove[1]);
	GET_ANALOG_KEY(INPUT_VEH_MOVE_UP,       controlState.ucInVehicleMove[2]);
	GET_ANALOG_KEY(INPUT_VEH_MOVE_DOWN,     controlState.ucInVehicleMove[3]);
	GET_ANALOG_KEY(INPUT_VEH_BRAKE,         controlState.ucInVehicleTriggers[0]);
	GET_ANALOG_KEY(INPUT_VEH_ACCELERATE,    controlState.ucInVehicleTriggers[1]);

	// Binary keys
	GET_BINARY_KEY(INPUT_ENTER,             controlState.keys.bEnterExitVehicle);
	GET_BINARY_KEY(INPUT_SPRINT,            controlState.keys.bSprint);
	GET_BINARY_KEY(INPUT_JUMP,              controlState.keys.bJump);
	GET_BINARY_KEY(INPUT_ATTACK,            controlState.keys.bAttack);
	GET_BINARY_KEY(INPUT_ATTACK2,           controlState.keys.bAttack2);
	GET_BINARY_KEY(INPUT_AIM,               controlState.keys.bAim);
	GET_BINARY_KEY(INPUT_FREE_AIM,          controlState.keys.bFreeAim);
	GET_BINARY_KEY(INPUT_MELEE_ATTACK1,     controlState.keys.bMeleeAttack1);
	GET_BINARY_KEY(INPUT_MELEE_ATTACK2,     controlState.keys.bMeleeAttack2);
	GET_BINARY_KEY(INPUT_MELEE_KICK,        controlState.keys.bMeleeKick);
	GET_BINARY_KEY(INPUT_MELEE_BLOCK,       controlState.keys.bMeleeBlock);
	GET_BINARY_KEY(INPUT_VEH_HANDBRAKE,     controlState.keys.bHandbrake);
	GET_BINARY_KEY(INPUT_VEH_HANDBRAKE_ALT, controlState.keys.bHandbrake2);
	GET_BINARY_KEY(INPUT_VEH_HORN,          controlState.keys.bHorn);
	GET_BINARY_KEY(INPUT_VEH_ATTACK,        controlState.keys.bDriveBy);
	GET_BINARY_KEY(INPUT_VEH_ATTACK2,       controlState.keys.bHeliPrimaryFire);
}

void CIVPad::SetCurrentClientControlState(CControlState controlState)
{
	if(m_pPad)
		FromControlState(controlState, true);
}

void CIVPad::GetCurrentClientControlState(CControlState& controlState)
{
	if(m_pPad)
		ToControlState(controlState, true);
}

void CIVPad::SetLastClientControlState(CControlState controlState)
{
	if(m_pPad)
		FromControlState(controlState, false);
}

void CIVPad::GetLastClientControlState(CControlState& controlState)
{
	if(m_pPad)
		ToControlState(controlState, false);
}

void CIVPad::SetIsUsingController(bool bIsUsingController)
{
	if(m_pPad)
		m_pPad->m_bIsUsingController = bIsUsingController;
}

bool CIVPad::GetIsUsingController()
{
	if(m_pPad)
		return m_pPad->m_bIsUsingController;

	return false;
}

void CIVPad::SetLastUpdateTime(DWORD dwLastUpdateTime)
{
	if(m_pPad)
		m_pPad->m_dwLastUpdateTime = dwLastUpdateTime;
}

DWORD CIVPad::GetLastUpdateTime()
{
	if(m_pPad)
		return m_pPad->m_dwLastUpdateTime;

	return 0;
}
