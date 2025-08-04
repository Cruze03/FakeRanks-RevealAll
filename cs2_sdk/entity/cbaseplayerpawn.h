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

enum PlayerButtons_t : uint64_t
{
	Attack = (1ull << 0),
	Jump = (1ull << 1),
	Duck = (1ull << 2),
	Forward = (1ull << 3),
	Back = (1ull << 4),
	Use = (1ull << 5),
	Cancel = (1ull << 6),
	Left = (1ull << 7),
	Right = (1ull << 8),
	Moveleft = (1ull << 9),
	Moveright = (1ull << 10),
	Attack2 = (1ull << 11),
	Run = (1ull << 12),
	Reload = (1ull << 13),
	Alt1 = (1ull << 14),
	Alt2 = (1ull << 15),
	Speed = (1ull << 16),   /** Player is holding the speed key */
	Walk = (1ull << 17),   /** Player holding walk key */
	Zoom = (1ull << 18),   /** Zoom key for HUD zoom */
	Weapon1 = (1ull << 19),   /** weapon defines these bits */
	Weapon2 = (1ull << 20),   /** weapon defines these bits */
	Bullrush = (1ull << 21),
	Grenade1 = (1ull << 22),   /** grenade 1ull */
	Grenade2 = (1ull << 23),   /** grenade 2 */
	Attack3 = (1ull << 24),
	Scoreboard = (1ull << 33),
	Inspect = (1ull << 35),
};

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