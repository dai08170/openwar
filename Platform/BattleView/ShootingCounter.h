// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ShootingMarker_H
#define ShootingMarker_H

#include "BattleModel/BattleSimulator.h"
#include "VertexBuffer.h"


class ShootingCounter
{
public:
	struct Projectile
	{
		glm::vec3 position1;
		glm::vec3 position2;
		float time;
		float duration;

		Projectile() :
		position1(),
		position2(),
		time(0),
		duration(0) { }
	};

	MissileType _missileType;
	std::vector<Projectile> _projectiles;
	int _soundCookie;
	bool _impacted;

public:
	ShootingCounter(MissileType missileType);
	~ShootingCounter();

	bool Animate(float seconds);
	void AddProjectile(glm::vec3 position1, glm::vec3 position2, float delay, float duration);

	void Render(VertexBuffer_3f_4f* renderer);

private:
	void RenderArrow(VertexBuffer_3f_4f* vertices, glm::vec3 p1, glm::vec3 p2, float t);
	void RenderBullet(VertexBuffer_3f_4f* vertices, glm::vec3 p1, glm::vec3 p2, float t);
};


#endif
