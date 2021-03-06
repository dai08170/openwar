// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "UnitMarker.h"
#include "BattleModel/BattleSimulator_v1_0_0.h"



UnitMarker::UnitMarker(BattleView* battleView, BattleObjects::Unit* unit) :
	_battleView{battleView},
	_unit{unit}
{
}


UnitMarker::~UnitMarker()
{
}
