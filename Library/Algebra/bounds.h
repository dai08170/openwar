// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BOUNDS_H
#define BOUNDS_H

#include <glm/glm.hpp>


template <class T, glm::precision P>
struct bounds1
{
	T min, max;

	bounds1() : min(0), max(0) {}
	bounds1(const bounds1<T, P>& b) : min(b.min), max(b.max) {}
	explicit bounds1(T v) : min(v), max(v) {}
	bounds1(T min_, T max_) : min(min_), max(max_) {}

	template <class T2> operator bounds1<T2, P>() const { return bounds1<T2, P>((T2)min, (T2)max); }

	T size() const {return max - min;}
	T radius() const {return (max - min) / 2;}

	bool empty() const {return min >= max;}
	bool contains(T v) const {return min <= v && v <= max;}

	bool intersects(const bounds1<T, P>& b) const
	{
		bool disjoint_left = max < b.min;
		bool disjoint_right = b.max < min;
		bool disjoint = disjoint_left || disjoint_right;
		return !disjoint;
	}

	bounds1<T, P> grow(T d) const {return bounds1<T, P>(min - d, max + d);}

	T mid() const {return (min + max) / 2;}
	T mix(T v) const {return min + v * (max - min);}
	T unmix(T v) const {return (v - min) / (max - min);}

	T clamp(T v) const
	{
		if (min > max) return (min + max) / 2;
		if (v < min) return min;
		if (v > max) return max;
		return v;
	}
};


template <class T, glm::precision P>
struct bounds2
{
	glm::tvec2<T, P> min, max;

	bounds2() {}
	bounds2(const bounds2<T, P>& b) : min(b.min), max(b.max) {}
	explicit bounds2(glm::tvec2<T, P> v) : min(v), max(v) {}

	bounds2(T min_x, T min_y, T max_x, T max_y) : min(min_x, min_y), max(max_x, max_y) {}
	bounds2(T min_x, T min_y, glm::tvec2<T, P> max_) : min(min_x, min_y), max(max_) {}
	bounds2(T min_x, T max_x, bounds1<T, P> y) : min(min_x, y.min), max(max_x, y.max) {}
	bounds2(bounds1<T, P> x, T min_y, T max_y) : min(x.min, min_y), max(x.max, max_y) {}
	bounds2(glm::tvec2<T, P> min_, glm::tvec2<T, P> max_) : min(min_), max(max_) {}
	bounds2(bounds1<T, P> x, bounds1<T, P> y) : min(x.min, y.min), max(x.max, y.max) {}

	template <class T2> operator bounds2<T2, P>() const { return bounds2<T2, P>((glm::tvec2<T2, P>)min, (glm::tvec2<T2, P>)max); }

	bounds1<T, P> x() const {return bounds1<T, P>(min.x, max.x);}
	bounds1<T, P> y() const {return bounds1<T, P>(min.y, max.y);}

	glm::tvec2<T, P> mid() const {return glm::tvec2<T, P>((min.x + max.x) / 2, (min.y + max.y) / 2);}
	glm::tvec2<T, P> mix(T kx, T ky) const {return glm::tvec2<T, P>(x().mix(kx), y().mix(ky));}
	glm::tvec2<T, P> mix_00() const {return glm::tvec2<T, P>(min.x, min.y);}
	glm::tvec2<T, P> mix_01() const {return glm::tvec2<T, P>(min.x, max.y);}
	glm::tvec2<T, P> mix_10() const {return glm::tvec2<T, P>(max.x, min.y);}
	glm::tvec2<T, P> mix_11() const {return glm::tvec2<T, P>(max.x, max.y);}

	glm::tvec2<T, P> size() const {return glm::tvec2<T, P>(max.x - min.x, max.y - min.y);}
	glm::tvec2<T, P> radius() const {return glm::tvec2<T, P>((max.x - min.x) / 2, (max.y - min.y) / 2);}

	bool empty() const {return min.x >= max.x || min.y >= max.y;}
	bool contains(T x, T y) const {return min.x <= x && x <= max.x && min.y <= y && y <= max.y;}
	bool contains(glm::tvec2<T, P> p) const {return min.x <= p.x && p.x <= max.x && min.y <= p.y && p.y <= max.y;}
	bool intersects(const bounds2<T, P>& b) const {return x().intersects(b.x()) && y().intersects(b.y());}

	bounds2 grow(T d) const {return bounds2<T, P>(min.x - d, min.y - d, max.x + d, max.y + d);}
	bounds2 grow(T dx, T dy) const {return bounds2<T, P>(min.x - dx, min.y - dy, max.x + dx, max.y + dy);}
	bounds2 grow(glm::tvec2<T, P> d) const {return bounds2<T, P>(min.x - d.x, min.y - d.y, max.x + d.x, max.y + d.y);}

	glm::tvec2<T, P> clamp(glm::tvec2<T, P> p) const {return glm::tvec2<T, P>(x().clamp(p.x), y().clamp(p.y));}

	bounds2& operator += (glm::tvec2<T, P> v)
	{
		min += v;
		max += v;
		return *this;
	}
	bounds2& operator -= (glm::tvec2<T, P> v)
	{
		min -= v;
		max -= v;
		return *this;
	}
	bounds2& operator *= (glm::tvec2<T, P> v)
	{
		min *= v;
		max *= v;
		return *this;
	}
	bounds2& operator /= (glm::tvec2<T, P> v)
	{
		min /= v;
		max /= v;
		return *this;
	}

	bounds2& operator *= (T k)
	{
		min *= k;
		max *= k;
		return *this;
	}
	bounds2& operator /= (T k)
	{
		min /= k;
		max /= k;
		return *this;
	}
};


template <class T, glm::precision P>
struct bounds3
{
	glm::tvec3<T, P> min, max;

	bounds3() {}
	bounds3(const bounds3<T, P>& b) : min(b.min), max(b.max) {}
	explicit bounds3(glm::tvec3<T, P> v) : min(v), max(v) {}

	bounds3(glm::tvec3<T, P> min_, glm::tvec3<T, P> max_)  : min(min_), max(max_) {}
	bounds3(bounds2<T, P> b, bounds1<T, P> z) : min(b.min, z.min), max(b.max, z.max) {}
	bounds3(bounds1<T, P> x, bounds1<T, P> y, bounds1<T, P> z) : min(x.min, y.min, z.min), max(x.max, y.max, z.max) {}

	template <class T2> operator bounds3<T2, P>() const { return bounds3<T2, P>((glm::tvec3<T2, P>)min, (glm::tvec3<T2, P>)max); }

	bool empty() const {return min.x >= max.x || min.y >= max.y || min.z >= max.z;}
	bool contains(glm::tvec3<T, P> p) const {return min.x <= p.x && p.x <= max.x && min.y <= p.y && p.y <= max.y && min.z <= p.z && p.z <= max.z;}

	bounds2<T, P> xy() const {return bounds2<T, P>(min.x, min.y, max.x, max.y);}
};


template <class T, glm::precision P> inline bool operator ==(bounds1<T, P> a, bounds1<T, P> b) {return a.min == b.min && a.max == b.max;}
template <class T, glm::precision P> inline bool operator ==(bounds2<T, P> a, bounds2<T, P> b) {return a.min == b.min && a.max == b.max;}
template <class T, glm::precision P> inline bool operator ==(bounds3<T, P> a, bounds3<T, P> b) {return a.min == b.min && a.max == b.max;}

template <class T, glm::precision P> inline bool operator !=(bounds1<T, P> a, bounds1<T, P> b) {return a.min != b.min || a.max != b.max;}
template <class T, glm::precision P> inline bool operator !=(bounds2<T, P> a, bounds2<T, P> b) {return a.min != b.min || a.max != b.max;}
template <class T, glm::precision P> inline bool operator !=(bounds3<T, P> a, bounds3<T, P> b) {return a.min != b.min || a.max != b.max;}

template <class T, glm::precision P> inline bounds1<T, P> operator +(bounds1<T, P> b, T k) {return bounds1<T, P>(b.min + k, b.max + k);}
template <class T, glm::precision P> inline bounds2<T, P> operator +(bounds2<T, P> b, glm::tvec2<T, P> v) {return bounds2<T, P>(b.min + v, b.max + v);}
template <class T, glm::precision P> inline bounds3<T, P> operator +(bounds3<T, P> b, glm::tvec3<T, P> v) {return bounds3<T, P>(b.min + v, b.max + v);}

template <class T, glm::precision P> inline bounds1<T, P> operator +(bounds1<T, P> b1, bounds1<T, P> b2) {return bounds1<T, P>(b1.min + b2.min, b1.max + b2.max);}
template <class T, glm::precision P> inline bounds2<T, P> operator +(bounds2<T, P> b1, bounds2<T, P> b2) {return bounds2<T, P>(b1.min + b2.min, b1.max + b2.max);}
template <class T, glm::precision P> inline bounds3<T, P> operator +(bounds3<T, P> b1, bounds3<T, P> b2) {return bounds3<T, P>(b1.min + b2.min, b1.max + b2.max);}

template <class T, glm::precision P> inline bounds1<T, P> operator -(bounds1<T, P> b, T k) {return bounds1<T, P>(b.min - k, b.max - k);}
template <class T, glm::precision P> inline bounds2<T, P> operator -(bounds2<T, P> b, glm::tvec2<T, P> v) {return bounds2<T, P>(b.min - v, b.max - v);}
template <class T, glm::precision P> inline bounds3<T, P> operator -(bounds3<T, P> b, glm::tvec3<T, P> v) {return bounds3<T, P>(b.min - v, b.max - v);}

template <class T, glm::precision P> inline bounds1<T, P> operator -(bounds1<T, P> b1, bounds1<T, P> b2) {return bounds1<T, P>(b1.min - b2.min, b1.max - b2.max);}
template <class T, glm::precision P> inline bounds2<T, P> operator -(bounds2<T, P> b1, bounds2<T, P> b2) {return bounds2<T, P>(b1.min - b2.min, b1.max - b2.max);}
template <class T, glm::precision P> inline bounds3<T, P> operator -(bounds3<T, P> b1, bounds3<T, P> b2) {return bounds3<T, P>(b1.min - b2.min, b1.max - b2.max);}

template <class T, glm::precision P> inline bounds1<T, P> operator *(bounds1<T, P> b, T k) {return bounds1<T, P>(b.min * k, b.max * k);}
template <class T, glm::precision P> inline bounds2<T, P> operator *(bounds2<T, P> b, T k) {return bounds2<T, P>(b.min * k, b.max * k);}
template <class T, glm::precision P> inline bounds3<T, P> operator *(bounds3<T, P> b, T k) {return bounds3<T, P>(b.min * k, b.max * k);}
template <class T, glm::precision P> inline bounds2<T, P> operator *(bounds2<T, P> b, glm::tvec2<T, P> v) {return bounds2<T, P>(b.min * v, b.max * v);}
template <class T, glm::precision P> inline bounds3<T, P> operator *(bounds3<T, P> b, glm::tvec3<T, P> v) {return bounds3<T, P>(b.min * v, b.max * v);}

template <class T, glm::precision P> inline bounds1<T, P> operator *(T k, bounds1<T, P> b) {return bounds1<T, P>(k * b.min, k * b.max);}
template <class T, glm::precision P> inline bounds2<T, P> operator *(T k, bounds2<T, P> b) {return bounds2<T, P>(k * b.min, k * b.max);}
template <class T, glm::precision P> inline bounds3<T, P> operator *(T k, bounds3<T, P> b) {return bounds3<T, P>(k * b.min, k * b.max);}
template <class T, glm::precision P> inline bounds2<T, P> operator *(glm::tvec2<T, P> v, bounds2<T, P> b) {return bounds2<T, P>(v * b.min, v * b.max);}
template <class T, glm::precision P> inline bounds3<T, P> operator *(glm::tvec3<T, P> v, bounds3<T, P> b) {return bounds3<T, P>(v * b.min, v * b.max);}

template <class T, glm::precision P> inline bounds1<T, P> operator /(bounds1<T, P> b, T k) {return bounds1<T, P>(b.min / k, b.max / k);}
template <class T, glm::precision P> inline bounds2<T, P> operator /(bounds2<T, P> b, T k) {return bounds2<T, P>(b.min / k, b.max / k);}
template <class T, glm::precision P> inline bounds3<T, P> operator /(bounds3<T, P> b, T k) {return bounds3<T, P>(b.min / k, b.max / k);}
template <class T, glm::precision P> inline bounds2<T, P> operator /(bounds2<T, P> b, glm::tvec2<T, P> v) {return bounds2<T, P>(b.min / v, b.max / v);}
template <class T, glm::precision P> inline bounds3<T, P> operator /(bounds3<T, P> b, glm::tvec3<T, P> v) {return bounds3<T, P>(b.min / v, b.max / v);}


typedef bounds1<double, glm::highp> bounds1d;
typedef bounds2<double, glm::highp> bounds2d;
typedef bounds3<double, glm::highp> bounds3d;

typedef bounds1<float, glm::highp> bounds1f;
typedef bounds2<float, glm::highp> bounds2f;
typedef bounds3<float, glm::highp> bounds3f;

typedef bounds1<int, glm::highp> bounds1i;
typedef bounds2<int, glm::highp> bounds2i;
typedef bounds3<int, glm::highp> bounds3i;


#endif
