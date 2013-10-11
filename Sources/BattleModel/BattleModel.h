// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BATTLEMODEL_H
#define BATTLEMODEL_H

#include <map>
#include "MovementRules.h"


class BattleModel;
class HeightMap;
class GroundMap;

struct Fighter;
struct Unit;
struct UnitUpdate;


struct Player
{
	int id;
	int team;

	Player() : id(0), team(0) { }
	Player(int i, int t) : id(i), team(t) { }
	Player(const Player& other) : id(other.id), team(other.team) { }
	Player& operator=(const Player& other) { id = other.id; team = other.team; return *this; }
	bool operator==(const Player& other) const { return id == other.id && team == other.team; }
	bool operator!=(const Player& other) const { return id != other.id || team != other.team; }
};



enum UnitPlatform
{
	UnitPlatformCav = 0, // Cavalry
	UnitPlatformGen = 1, // General
	UnitPlatformAsh = 2, // Ashigaru
	UnitPlatformSam = 3  // Samurai
};


enum UnitWeapon
{
	UnitWeaponYari = 0,
	UnitWeaponKata = 1,
	UnitWeaponNagi = 2,
	UnitWeaponBow = 3,
	UnitWeaponArq = 4
};


enum UnitMode
{
	UnitModeInitializing,
	UnitModeStanding,
	UnitModeMoving,
	UnitModeTurning
};


struct Projectile
{
	glm::vec2 position1;
	glm::vec2 position2;
	float delay;

	Projectile();
};


struct Shooting
{
	UnitWeapon unitWeapon;
	float timeToImpact;
	std::vector<Projectile> projectiles;

	Shooting();
};


struct Casualty
{
	glm::vec2 position;
	Player player;
	UnitPlatform platform;

	Casualty(glm::vec2 position_, Player player_, UnitPlatform platform_) :
	position(position_), player(player_), platform(platform_) { }
};


enum ReadyState
{
	ReadyStateUnready,
	ReadyStateReadying,
	ReadyStatePrepared,
	ReadyStateStriking,
	ReadyStateStunned
};


struct FighterState
{
	// dynamic attributes
	glm::vec2 position;
	float position_z;
	ReadyState readyState;
	float readyingTimer;
	float strikingTimer;
	float stunnedTimer;
	Fighter* opponent;

	// intermediate attributes
	glm::vec2 destination;
	glm::vec2 velocity;
	float direction;
	Fighter* meleeTarget;

	FighterState();
};


struct FighterUpdate
{
	unsigned char positionX;
	unsigned char positionY;
};


struct Fighter
{
	// static attributes
	Unit* unit;

	// dynamic attributes
	FighterState state;

	// optimization attributes
	bool terrainForest;
	bool terrainImpassable;
	glm::vec2 terrainPosition;

	// intermediate attributes
	FighterState nextState;
	bool casualty;


	Fighter();
	FighterUpdate GetFighterUpdate(const UnitUpdate& unitUpdate);
	void SetFighterUpdate(const UnitUpdate& unitUpdate, const FighterUpdate& fighterUpdate);
};


struct UnitStats
{
	UnitPlatform unitPlatform;
	UnitWeapon unitWeapon;
	float weaponReach;
	float trainingLevel;
	float strikingDuration;
	float readyingDuration;
	float fireRate; // shots per minute
	float minimumRange; // minimum fire range in meters
	float maximumRange; // maximum fire range in meters
	float fireAccuracy; // percentage of hit at half range
	float walkingSpeed; // meters per second
	float runningSpeed; // meters per second
	glm::vec2 fighterSize; // x = side-to-side, y = front-to-back
	glm::vec2 spacing;  // x = side-to-side, y = front-to-back

	UnitStats();
};


struct UnitRange
{
	glm::vec2 center;
	float angleStart;
	float angleLength;
	float minimumRange;
	float maximumRange;
	std::vector<float> actualRanges;

	UnitRange();

	bool IsWithinRange(glm::vec2 p) const;
};


struct UnitState
{
	// dynamic attributes
	float loadingTimer;
	float loadingDuration;
	int shootingCounter;
	float morale;

	// optimization attributes
	UnitMode unitMode;
	glm::vec2 center;
	float direction;
	float influence;

	// intermediate attributes
	int recentCasualties;

	glm::vec2 waypoint;


	bool IsRouting() const
	{
		float x = morale + influence;
		return x <= 0;
	}

	float GetRoutingBlinkTime() const
	{
		float x = morale + influence;
		return 0 <= x && x < 0.33f ? 0.1f + x * 3 : 0;
	}

	UnitState();
};


struct UnitUpdate
{
	int unitId;
	int fightersCount;
	float morale;

	float minX, maxX, minY, maxY;

	glm::vec2 movementDestination;
	float movementDirection;
	int movementTargetUnitId;
	bool movementRunning;

	UnitUpdate();
};


struct UnitCommand
{
	std::vector<glm::vec2> path;
	float facing;
	bool running;
	Unit* meleeTarget;
	Unit* missileTarget;
	bool missileTargetLocked; // updated by TouchGesture()
	bool holdFire;

	UnitCommand();


	void UpdatePath(glm::vec2 curr, glm::vec2 dest)
	{
		MovementRules::UpdateMovementPath(path, curr, dest);
	}

	void ClearPathAndSetDestination(glm::vec2 p)
	{
		path.clear();
		path.push_back(p);
	}


	glm::vec2 GetDestination()
	{
		return !path.empty() != 0 ? path.back() : glm::vec2();
	}
};


struct Unit
{
	// static attributes
	int unitId;
	Player player;
	UnitStats stats;
	Fighter* fighters;

	// dynamic attributes
	UnitState state; // updated by AssignNextState()
	int fightersCount; // updated by RemoveCasualties()
	int shootingCounter; // updated by ResolveMissileCombat()
	Formation formation; // updated by UpdateFormation()
	float timeUntilSwapFighters;

	// intermediate attributes
	UnitState nextState; // updated by ComputeNextState()
	UnitRange unitRange;

	// control attributes
	UnitCommand command; // updated by TouchGesture()

	Unit();

	UnitUpdate GetUnitUpdate();
	void SetUnitUpdate(UnitUpdate unitUpdate, BattleModel* battleModel);

	glm::vec2 CalculateUnitCenter();

	float GetSpeed();

	static int GetFighterRank(Fighter* fighter);
	static int GetFighterFile(Fighter* fighter);
	static Fighter* GetFighter(Unit* unit, int rank, int file);
};


class SmoothTerrainRenderer;

class BattleModel
{
public:
	GroundMap* groundMap;
	HeightMap* heightMap;

	int lastUnitId;
	Player bluePlayer;
	int winnerTeam;
	float time;
	float timeStep;

	std::map<int, Unit*> units;
	std::vector<Shooting> shootings;

public:
	BattleModel();
	virtual ~BattleModel();

	Unit* GetUnit(int unitId) const
	{
		std::map<int, Unit*>::const_iterator i = units.find(unitId);
		return i != units.end() ? (*i).second : 0;
	}

	bool IsMelee() const;

	Unit* AddUnit(Player player, int numberOfFighters, UnitStats stats, glm::vec2 position);

	static UnitStats GetDefaultUnitStats(UnitPlatform unitPlatform, UnitWeapon unitWeapon);
};


#endif
