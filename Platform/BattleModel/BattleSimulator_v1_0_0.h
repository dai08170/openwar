// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BattleSimulator_H
#define BattleSimulator_H

#include <map>
#include <set>
#include <string>
#include "Algorithms/quadtree.h"
#include "BattleMap/GroundMap.h"
#include "BattleObjects_v1.h"

class BattleObserver;



class BattleSimulator_v1_0_0
{
	std::set<BattleObserver*> _observers{};
	quadtree<BattleObjects_v1::Fighter*> _fighterQuadTree{0, 0, 1024, 1024};
	quadtree<BattleObjects_v1::Fighter*> _weaponQuadTree{0, 0, 1024, 1024};

	BattleMap* _battleMap{};

	std::vector<BattleObjects_v1::Unit*> _units{};
	std::vector<std::pair<float, BattleObjects_v1::Shooting>> _shootings{};
	std::map<int, int> _kills{};

	float _secondsSinceLastTimeStep{};
	float _timeStep{1.0f / 15.0f};
	bool _practice{};
	int _winnerTeam{};
	glm::vec3 _deploymentZones[2]{};

	/***/

	BattleScript* _script{};
	std::vector<BattleObjects::Commander*> _commanders{};
	BattleObjects::Commander* _dummyCommander{};
	int _teamPosition1{};
	int _teamPosition2{};
	float _deploymentTimer{};
	bool _deploymentEnabled{};

public:
	BattleSimulator_v1_0_0(BattleMap* battleMap);
	~BattleSimulator_v1_0_0();

	float GetTimerDelay() const { return 0.25f; }

	void SetPractice(bool value) { _practice = value; }
	int GetKills(int team) { return _kills[team]; }

	void AddObserver(BattleObserver* observer);
	void RemoveObserver(BattleObserver* observer);

	BattleMap* GetBattleMap() const;

	void SetDeploymentZone(int team, glm::vec2 center, float radius);
	glm::vec2 GetDeploymentCenter(int team) const;
	float GetDeploymentRadius(int team) const;
	bool IsDeploymentZone(int team, glm::vec2 position) const;
	glm::vec2 ConstrainDeploymentZone(int team, glm::vec2 position, float inset) const;
	bool HasCompletedDeployment(int team) const;
	void Deploy(BattleObjects_v1::Unit* unit, glm::vec2 position);

	const std::vector<BattleObjects_v1::Unit*>& GetUnits() { return _units; }
	BattleObjects_v1::Unit* AddUnit(BattleObjects::Commander* commander, const char* unitClass, int numberOfFighters, BattleObjects_v1::UnitStats stats, glm::vec2 position);
	void RemoveUnit(BattleObjects_v1::Unit* unit);

	void NewUnit(int commanderId, const char* unitClass, int strength, glm::vec2 position, float bearing);

	void SetUnitCommand(BattleObjects_v1::Unit* unit, const BattleObjects_v1::UnitCommand& command, float timer);

	void AddShooting(const BattleObjects_v1::Shooting& shooting, float timer);

	void AdvanceTime(float secondsSinceLastTime);

	int CountCavalryInMelee() const;
	int CountInfantryInMelee() const;

	int GetWinnerTeam() const { return _winnerTeam; }

private:
	void SimulateOneTimeStep();

	void RebuildQuadTree();

	void ComputeNextState();
	void AssignNextState();
	void UpdateUnitRange(BattleObjects_v1::Unit* unit);

	void ResolveMeleeCombat();
	void ResolveMissileCombat();

	void TriggerShooting(BattleObjects_v1::Unit* unit);
	void ResolveProjectileCasualties();

	void RemoveCasualties();
	void RemoveDeadUnits();
	void RemoveFinishedShootings();

	BattleObjects_v1::UnitState NextUnitState(BattleObjects_v1::Unit* unit);
	BattleObjects_v1::UnitMode NextUnitMode(BattleObjects_v1::Unit* unit);
	float NextUnitDirection(BattleObjects_v1::Unit* unit);

	BattleObjects_v1::FighterState NextFighterState(BattleObjects_v1::Fighter* fighter);
	glm::vec2 NextFighterPosition(BattleObjects_v1::Fighter* fighter);
	glm::vec2 NextFighterVelocity(BattleObjects_v1::Fighter* fighter);

	BattleObjects_v1::Fighter* FindFighterStrikingTarget(BattleObjects_v1::Fighter* fighter);

	bool IsWithinLineOfFire(BattleObjects_v1::Unit* unit, glm::vec2 position);
	BattleObjects_v1::Unit* ClosestEnemyWithinLineOfFire(BattleObjects_v1::Unit* unit);

	bool TeamHasAbandondedBattle(int team) const;

public:
	void SetScript(BattleScript* value);

	void SetTeamPosition(int team, int position);
	int GetTeamPosition(int team) const;

	BattleObjects::Commander* AddCommander(const char* playerId, int team, BattleObjects::CommanderType type);
	BattleObjects::Commander* GetCommander(const char* playerId) const;
	const std::vector<BattleObjects::Commander*>& GetCommanders() const { return _commanders; }
	BattleObjects::Commander* GetDummyCommander() const;

	void Tick(double secondsSinceLastTick);

	void EnableDeploymentZones(float deploymentTimer);
	void UpdateDeploymentZones(double secondsSinceLastTick);

	static void MovementRules_AdvanceTime(BattleObjects_v1::Unit* unit, float timeStep);
	static void MovementRules_SwapFighters(BattleObjects_v1::Unit* unit);
	static glm::vec2 MovementRules_NextFighterDestination(BattleObjects_v1::Fighter* fighter);
	static glm::vec2 MovementRules_NextWaypoint(BattleObjects_v1::Unit* unit);
};


#endif