// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "BattleScript.h"
#include "BattleModel.h"

#include "lauxlib.h"
#include "lualib.h"


static BattleScript* _battlescript = nullptr;




BattleScript::BattleScript(BattleModel* battleModel, SimulationState* simulationState) :
_battleModel(battleModel),
_simulationState(simulationState),
_L(nullptr)
{
	_battlescript = this;

	_L = luaL_newstate();
	luaL_openlibs(_L);

	lua_pushcfunction(_L, battle_message);
	lua_setglobal(_L, "battle_message");

	lua_pushcfunction(_L, battle_get_time);
	lua_setglobal(_L, "battle_get_time");

	lua_pushcfunction(_L, battle_new_unit);
	lua_setglobal(_L, "battle_new_unit");

	lua_pushcfunction(_L, battle_set_unit_movement);
	lua_setglobal(_L, "battle_set_unit_movement");

	lua_pushcfunction(_L, battle_get_unit_status);
	lua_setglobal(_L, "battle_get_unit_status");

	NSString* path = [[NSBundle mainBundle] pathForResource:@"BattleScript" ofType:@"lua" inDirectory:@"BattleScripts"];
	NSData* data = [NSData dataWithContentsOfFile:path];

	int error = luaL_loadbuffer(_L, (const char*) data.bytes, data.length, "line");
	if (!error) lua_pcall(_L, 0, 0, 0);

	if (error)
	{
		NSLog(@"BattleScript ERROR: %s", lua_tostring(_L, -1));
		lua_pop(_L, 1);  /* pop error message from the stack */
	}
}



BattleScript::~BattleScript()
{
	lua_close(_L);
}



void BattleScript::Tick()
{

}



int BattleScript::NewUnit(Player player, UnitPlatform platform, UnitWeapon weapon, int strength, glm::vec2 position, float heading)
{
	UnitStats unitStats = SimulationState::GetDefaultUnitStats(platform, weapon);

	Unit* unit = _simulationState->AddUnit(player, strength, unitStats, position);
	unit->movement.direction = heading;

	_battleModel->AddUnitMarker(unit);
	if (unit->stats.maximumRange > 0)
		_battleModel->AddRangeMarker(unit);

	return unit->unitId;
}



void BattleScript::SetUnitMovement(int unitId, bool running, std::vector<glm::vec2> path, int chargeId, float heading)
{
	Unit* unit = _simulationState->GetUnit(unitId);
	if (unit != nullptr)
	{
		unit->movement.path = path;
		//unit->movement.path_t0 = 0;
		unit->movement.destination = path.empty() ? unit->state.center : path.front();
		unit->movement.direction = heading;
		unit->movement.target = _simulationState->GetUnit(chargeId);
		unit->movement.running = running;

		if (_battleModel->GetMovementMarker(unit) == nullptr)
			_battleModel->AddMovementMarker(unit);
	}
}



/***/


int BattleScript::battle_message(lua_State* L)
{
	int n = lua_gettop(L);

	const char* s = n < 1 ? nullptr : lua_tostring(L, 1);
	if (s != nullptr)
		NSLog(@"BattleScript: %s", s);

	return 0;
}



int BattleScript::battle_get_time(lua_State* L)
{
	lua_pushnumber(L, 47.62);
	return 1;
}



int BattleScript::battle_new_unit(lua_State* L)
{
	int n = lua_gettop(L);
	Player player = n < 1 ? Player1 : ToPlayer(L, 1);
	UnitPlatform platform = n < 2 ? UnitPlatformCav : ToUnitPlatform(L, 2);
	UnitWeapon weapon = n < 3 ? UnitWeaponYari : ToUnitUnitWeapon(L, 3);
	int strength = n < 4 ? 40 : (int) lua_tonumber(L, 4);
	float x = n < 5 ? 512 : (float) lua_tonumber(L, 5);
	float y = n < 6 ? 512 : (float) lua_tonumber(L, 6);

	int unitId = _battlescript->NewUnit(player, platform, weapon, strength, glm::vec2(x, y), 0);

	lua_pushnumber(L, unitId);

	return 1;
}



int BattleScript::battle_set_unit_movement(lua_State* L)
{
	int n = lua_gettop(L);
	int unitId = n < 1 ? 0 : (int) lua_tonumber(L, 1);
	bool running = n < 2 ? false : lua_toboolean(L, 2);
	std::vector<glm::vec2> path;
	if (n >= 3) ToPath(path, L, 3);
	int chargeId = n < 4 ? 0 : (int) lua_tonumber(L, 4);
	float heading = n < 5 ? 0 : (float) lua_tonumber(L, 5);

	_battlescript->SetUnitMovement(unitId, running, path, chargeId, heading);

	return 0;
}



int BattleScript::battle_get_unit_status(lua_State* L)
{
	int n = lua_gettop(L);
	int unitId = n < 1 ? 0 : (int) lua_tonumber(L, 1);

	Unit* unit = _battlescript->_simulationState->GetUnit(unitId);
	if (unit != nullptr)
	{
		UnitStatus status(unit);
		lua_pushnumber(L, status.position.x);
		lua_pushnumber(L, status.position.y);
		lua_pushnumber(L, status.heading);
		lua_pushnumber(L, status.state);
		lua_pushnumber(L, status.strength);
		lua_pushnumber(L, status.morale);
		return 6;
	}

	return 0;
}



/***/




Player BattleScript::ToPlayer(lua_State* L, int index)
{
	return lua_tonumber(L, index) == 2 ? Player2 : Player1;
}



UnitPlatform BattleScript::ToUnitPlatform(lua_State* L, int index)
{
	const char* s = lua_tostring(L, index);
	if (s != nullptr)
	{
		if (std::strcmp(s, "CAV") == 0) return UnitPlatformCav;
		if (std::strcmp(s, "GEN") == 0) return UnitPlatformGen;
		if (std::strcmp(s, "ASH") == 0) return UnitPlatformAsh;
		if (std::strcmp(s, "SAM") == 0) return UnitPlatformSam;
	}
	return UnitPlatformCav;
}



UnitWeapon BattleScript::ToUnitUnitWeapon(lua_State* L, int index)
{
	const char* s = lua_tostring(L, index);
	if (s != nullptr)
	{
		if (std::strcmp(s, "YARI") == 0) return UnitWeaponYari;
		if (std::strcmp(s, "KATA") == 0) return UnitWeaponKata;
		if (std::strcmp(s, "NAGI") == 0) return UnitWeaponNagi;
		if (std::strcmp(s, "BOW") == 0) return UnitWeaponBow;
		if (std::strcmp(s, "ARQ") == 0) return UnitWeaponArq;
	}
	return UnitWeaponYari;
}



void BattleScript::ToPath(std::vector<glm::vec2>& result, lua_State* L, int index)
{
	int key = 1;
	while (true)
	{
		lua_rawgeti(L, index, key);
		if (!lua_istable(L, -1))
			break;

		lua_pushstring(L, "x");
		lua_gettable(L, -2);
		float x = (float) lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_pushstring(L, "y");
		lua_gettable(L, -2);
		float y = (float) lua_tonumber(L, -1);
		lua_pop(L, 1);

		result.push_back(glm::vec2(x, y));

		lua_pop(L, 1);

		++key;
	}
}



/***/



BattleScript::UnitStatus::UnitStatus(Unit* unit)
{
	position = unit->state.center;
	heading = 0;
	state = 0;
	strength = unit->fightersCount;
	morale = unit->state.morale;

	if (unit->state.IsRouting())
	{
		state = 3;
	}
	else if (unit->movement.target != nullptr)
	{
		state = 4;
	}
	else if (!unit->movement.path.empty())
	{
		if (unit->movement.running)
			state = 2;
		else
			state = 1;
	}

	// 0 = STANDING
	// 1 = WALKING
	// 2 = RUNNING
	// 3 = ROUTING
	// 4 = CHARGING
	// 5 = FIGHTING
	// 6 = SHOOTING
}