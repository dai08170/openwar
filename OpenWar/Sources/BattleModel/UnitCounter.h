// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef UnitCounter_H
#define UnitCounter_H

class BattleModel;
class BattleRendering;
class BillboardModel;
class PlainLineRenderer;
class TextureBillboardRenderer;
class Unit;


class UnitCounter
{
public:
	BattleModel* _battleModel;
	Unit* _unit;
	float _routingTimer;

public:
	UnitCounter(BattleModel* battleModel, Unit* unit);
	~UnitCounter();

	bool Animate(float seconds);

	void AppendUnitMarker(TextureBillboardRenderer* renderer1, TextureBillboardRenderer* renderer2, bool flip);
	void AppendFighterWeapons(PlainLineRenderer* renderer);
	void AppendFighterBillboards(BillboardModel* billboardModel);
};


#endif
