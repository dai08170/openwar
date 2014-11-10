// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TOUCH_H
#define TOUCH_H

#include <memory>
#include <vector>
#include "Algorithms/sampler.h"

class Gesture;
class Hotspot;


struct MouseButtons
{
	bool left, right, other;
	MouseButtons(bool l = false, bool r = false, bool o = false) : left(l), right(r), other(o) { }
	bool Any() const { return left || right || other; }
};

inline bool operator==(MouseButtons v1, MouseButtons v2) { return v1.left == v2.left && v1.right == v2.right && v1.other == v2.other; }
inline bool operator!=(MouseButtons v1, MouseButtons v2) { return v1.left != v2.left || v1.right != v2.right || v1.other != v2.other; }


enum class Motion { Unknown, Stationary, Moving };


class Touch
{
	friend class Gesture;
	friend class Hotspot;
	friend class ButtonHotspot; // fulhack

	std::vector<std::shared_ptr<Hotspot>> _subscribedHotspots;
	std::shared_ptr<Hotspot> _capturedByHotspot;

	int _tapCount;
	bool _hasMoved;
	glm::vec2 _position;
	glm::vec2 _previous;
	glm::vec2 _original;
	double _timestart;
	double _timestamp;
	sampler _sampler;
	MouseButtons _currentButtons;
	MouseButtons _previousButtons;

public:
	Touch(int tapCount, glm::vec2 position, double timestamp, MouseButtons buttons);
	~Touch();

	bool IsCaptured() const;

	int GetTapCount() const;

	void TouchBegan();
	void TouchMoved();
	void TouchEnded();

	void Update(glm::vec2 position, glm::vec2 previous, double timestamp);
	void Update(glm::vec2 position, double timestamp, MouseButtons buttons);
	void Update(double timestamp);

	glm::vec2 GetCurrentPosition() const;
	glm::vec2 GetPreviousPosition() const;
	glm::vec2 GetOriginalPosition() const;

	double GetTimeStart() const;
	double GetTimestamp() const;

	MouseButtons GetCurrentButtons() const;
	MouseButtons GetPreviousButtons() const;

	Motion GetMotion() const;

	bool HasMoved() const;
	void ResetHasMoved();

	void ResetVelocity();
	glm::vec2 GetVelocity() const;
	glm::vec2 GetVelocity(double timestamp) const;

};


#endif