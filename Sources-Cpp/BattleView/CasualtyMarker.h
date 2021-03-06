// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef CasualtyMarker_H
#define CasualtyMarker_H

#include <cstdlib>
#include "BattleModel/BattleSimulator_v1_0_0.h"
#include "Shapes/VertexShape.h"

class BillboardModel;


class CasualtyMarker
{
public:
	struct Casualty
	{
		glm::vec3 position{};
		int team{};
		BattleObjects_v1::SamuraiPlatform platform{};
		float time{};
		int seed{};

		Casualty(glm::vec3 position_, int team_, BattleObjects_v1::SamuraiPlatform platform_) :
			position{position_},
			team{team_},
			platform{platform_},
			seed{std::rand() & 0x7fff}
		{ }
	};

	std::vector<Casualty> casualties;
	BattleSimulator* _battleSimulator;

public:
	CasualtyMarker(BattleSimulator* battleSimulator);
	~CasualtyMarker();

	void AddCasualty(glm::vec3 position, int team, BattleObjects_v1::SamuraiPlatform platform);
	bool Animate(float seconds);

	void RenderCasualtyColorBillboards(VertexShape_3f_4f_1f* vertices);
	void AppendCasualtyBillboards(BillboardModel* billboardModel);
};


#endif
