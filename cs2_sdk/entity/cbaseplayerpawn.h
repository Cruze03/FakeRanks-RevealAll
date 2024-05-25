/**
 * =============================================================================
 * CS2Fixes
 * Copyright (C) 2023 Source2ZE
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "ehandle.h"
#include "cbaseentity.h"

class CInButtonState
{
public:
    DECLARE_SCHEMA_CLASS_INLINE(CInButtonState);
	
	SCHEMA_FIELD_POINTER(uint64, m_pButtonStates)
};

class CPlayerPawnComponent
{
public:
	DECLARE_SCHEMA_CLASS_INLINE(CPlayerPawnComponent);
};

class CPlayer_MovementServices : public CPlayerPawnComponent
{
public:
    DECLARE_SCHEMA_CLASS(CPlayer_MovementServices);
	
	SCHEMA_FIELD(CInButtonState, m_nButtons)
};

class CBasePlayerPawn : public CBaseModelEntity
{
public:
	DECLARE_SCHEMA_CLASS(CBasePlayerPawn);

	SCHEMA_FIELD(CPlayer_MovementServices*, m_pMovementServices)
};