#pragma once
#include "Includes.h"

class QAngleByValue;

class QAngle
{
public:
	// Members
	vec_t x, y, z;

	// Construction/destruction
	QAngle(void);
	QAngle(vec_t X, vec_t Y, vec_t Z);

	//	QAngle(RadianEuler const &angles);	// evil auto type promotion!!!

		// Allow pass-by-value
	operator QAngleByValue& () { return *((QAngleByValue*)(this)); }
	operator const QAngleByValue& () const { return *((const QAngleByValue*)(this)); }

	QAngle	operator-(void) const;

	QAngle	operator+(const QAngle& v) const;
	QAngle	operator-(const QAngle& v) const;
	QAngle	operator*(float fl) const;
	QAngle	operator/(float fl) const;
};

inline QAngle::QAngle(void)
{
	x = y = z = VEC_T_NAN;
}

inline QAngle::QAngle(vec_t X, vec_t Y, vec_t Z)
{
	x = X; y = Y; z = Z;
}

class QAngleByValue : public QAngle
{
public:
	// Construction/destruction:
	QAngleByValue(void) : QAngle() {}
	QAngleByValue(vec_t X, vec_t Y, vec_t Z) : QAngle(X, Y, Z) {}
	QAngleByValue(const QAngleByValue& vOther) { *this = vOther; }
};

inline QAngle QAngle::operator-(void) const
{
	QAngle ret(-x, -y, -z);
	return ret;
}

inline QAngle QAngle::operator+(const QAngle& v) const
{
	QAngle res;
	res.x = x + v.x;
	res.y = y + v.y;
	res.z = z + v.z;
	return res;
}

inline QAngle QAngle::operator-(const QAngle& v) const
{
	QAngle res;
	res.x = x - v.x;
	res.y = y - v.y;
	res.z = z - v.z;
	return res;
}

inline QAngle QAngle::operator*(float fl) const
{
	QAngle res;
	res.x = x * fl;
	res.y = y * fl;
	res.z = z * fl;
	return res;
}

inline QAngle QAngle::operator/(float fl) const
{
	QAngle res;
	res.x = x / fl;
	res.y = y / fl;
	res.z = z / fl;
	return res;
}

inline QAngle operator*(float fl, const QAngle& v)
{
	QAngle ret(v * fl);
	return ret;
}