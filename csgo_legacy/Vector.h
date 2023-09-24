#pragma once
#include "Includes.h"
#include <float.h>

#define DECL_ALIGN(x)			__declspec( align( x ) )

#define ALIGN4 DECL_ALIGN(4)
#define ALIGN8 DECL_ALIGN(8)
#define ALIGN16 DECL_ALIGN(16)
#define ALIGN32 DECL_ALIGN(32)
#define ALIGN128 DECL_ALIGN(128)
#define ALIGN_N( _align_ ) DECL_ALIGN( _align_ )

#define ALIGN4_POST
#define ALIGN8_POST
#define ALIGN16_POST
#define ALIGN32_POST
#define ALIGN128_POST
#define ALIGN_N_POST( _align_ )

class Vector2D
{
public:
	// Members
	vec_t x, y;

	// Construction/destruction
	Vector2D(void);
	Vector2D(vec_t X, vec_t Y);
	Vector2D(const float* pFloat);

	// Initialization
	void Init(vec_t ix = 0.0f, vec_t iy = 0.0f);

	// Got any nasty NAN's?
	bool IsValid() const;

	// array access...
	vec_t operator[](int i) const;
	vec_t& operator[](int i);

	// Base address...
	vec_t* Base();
	vec_t const* Base() const;

	// Initialization methods
	void Random(float minVal, float maxVal);

	// equality
	bool operator==(const Vector2D& v) const;
	bool operator!=(const Vector2D& v) const;

	// arithmetic operations
	Vector2D& operator+=(const Vector2D& v);
	Vector2D& operator-=(const Vector2D& v);
	Vector2D& operator*=(const Vector2D& v);
	Vector2D& operator*=(float s);
	Vector2D& operator/=(const Vector2D& v);
	Vector2D& operator/=(float s);

	// negate the Vector2D components
	void	Negate();

	// Get the Vector2D's magnitude.
	vec_t	Length() const;

	// Get the Vector2D's magnitude squared.
	vec_t	LengthSqr(void) const;

	// return true if this vector is (0,0) within tolerance
	bool IsZero(float tolerance = 0.01f) const
	{
		return (x > -tolerance && x < tolerance&&
			y > -tolerance && y < tolerance);
	}

	// Normalize in place and return the old length.
	vec_t	NormalizeInPlace();

	// Compare length.
	bool	IsLengthGreaterThan(float val) const;
	bool	IsLengthLessThan(float val) const;

	// Get the distance from this Vector2D to the other one.
	vec_t	DistTo(const Vector2D& vOther) const;

	// Get the distance from this Vector2D to the other one squared.
	vec_t	DistToSqr(const Vector2D& vOther) const;

	// Copy
	void	CopyToArray(float* rgfl) const;

	// Multiply, add, and assign to this (ie: *this = a + b * scalar). This
	// is about 12% faster than the actual Vector2D equation (because it's done per-component
	// rather than per-Vector2D).
	void	MulAdd(const Vector2D& a, const Vector2D& b, float scalar);

	// Dot product.
	vec_t	Dot(const Vector2D& vOther) const;

	// assignment
	Vector2D& operator=(const Vector2D& vOther);

	Vector2D(const Vector2D& vOther);

	// arithmetic operations
	Vector2D	operator-(void) const;

	Vector2D	operator+(const Vector2D& v) const;
	Vector2D	operator-(const Vector2D& v) const;
	Vector2D	operator*(const Vector2D& v) const;
	Vector2D	operator/(const Vector2D& v) const;
	Vector2D	operator*(float fl) const;
	Vector2D	operator/(float fl) const;

	// Cross product between two vectors.
	Vector2D	Cross(const Vector2D& vOther) const;

	// Returns a Vector2D with the min or max in X, Y, and Z.
	Vector2D	Min(const Vector2D& vOther) const;
	Vector2D	Max(const Vector2D& vOther) const;
};

//-----------------------------------------------------------------------------

const Vector2D vec2_origin(0, 0);
const Vector2D vec2_invalid(FLT_MAX, FLT_MAX);

inline Vector2D::Vector2D(void)
{
	x = y = VEC_T_NAN;
}

inline Vector2D::Vector2D(vec_t X, vec_t Y)
{
	x = X; y = Y;
}

inline Vector2D::Vector2D(const float* pFloat)
{
	x = pFloat[0]; y = pFloat[1];
}

inline void Vector2D::Init(vec_t ix, vec_t iy)
{
	x = ix; y = iy;
}

inline void Vector2DClear(Vector2D& a)
{
	a.x = a.y = 0.0f;
}

//-----------------------------------------------------------------------------
// assignment
//-----------------------------------------------------------------------------

inline Vector2D& Vector2D::operator=(const Vector2D& vOther)
{
	x = vOther.x; y = vOther.y;
	return *this;
}

//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------

inline vec_t& Vector2D::operator[](int i)
{
	return ((vec_t*)this)[i];
}

inline vec_t Vector2D::operator[](int i) const
{
	return ((vec_t*)this)[i];
}

//-----------------------------------------------------------------------------
// Base address...
//-----------------------------------------------------------------------------

inline vec_t* Vector2D::Base()
{
	return (vec_t*)this;
}

inline vec_t const* Vector2D::Base() const
{
	return (vec_t const*)this;
}

//-----------------------------------------------------------------------------
// IsValid?
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// comparison
//-----------------------------------------------------------------------------

inline bool Vector2D::operator==(const Vector2D& src) const
{
	return (src.x == x) && (src.y == y);
}

inline bool Vector2D::operator!=(const Vector2D& src) const
{
	return (src.x != x) || (src.y != y);
}


//-----------------------------------------------------------------------------
// Copy
//-----------------------------------------------------------------------------

inline void Vector2DCopy(const Vector2D& src, Vector2D& dst)
{
	dst.x = src.x;
	dst.y = src.y;
}

inline void	Vector2D::CopyToArray(float* rgfl) const
{
	rgfl[0] = x; rgfl[1] = y;
}

//-----------------------------------------------------------------------------
// standard math operations
//-----------------------------------------------------------------------------

inline void Vector2D::Negate()
{
	x = -x; y = -y;
}

inline Vector2D& Vector2D::operator+=(const Vector2D& v)
{
	x += v.x; y += v.y;
	return *this;
}

inline Vector2D& Vector2D::operator-=(const Vector2D& v)
{
	x -= v.x; y -= v.y;
	return *this;
}

inline Vector2D& Vector2D::operator*=(float fl)
{
	x *= fl;
	y *= fl;
	return *this;
}

inline Vector2D& Vector2D::operator*=(const Vector2D& v)
{
	x *= v.x;
	y *= v.y;
	return *this;
}

inline Vector2D& Vector2D::operator/=(float fl)
{
	float oofl = 1.0f / fl;
	x *= oofl;
	y *= oofl;
	return *this;
}

inline Vector2D& Vector2D::operator/=(const Vector2D& v)
{
	x /= v.x;
	y /= v.y;
	return *this;
}

inline void Vector2DAdd(const Vector2D& a, const Vector2D& b, Vector2D& c)
{
	c.x = a.x + b.x;
	c.y = a.y + b.y;
}

inline void Vector2DSubtract(const Vector2D& a, const Vector2D& b, Vector2D& c)
{
	c.x = a.x - b.x;
	c.y = a.y - b.y;
}

inline void Vector2DMultiply(const Vector2D& a, vec_t b, Vector2D& c)
{
	c.x = a.x * b;
	c.y = a.y * b;
}

inline void Vector2DMultiply(const Vector2D& a, const Vector2D& b, Vector2D& c)
{
	c.x = a.x * b.x;
	c.y = a.y * b.y;
}


inline void Vector2DDivide(const Vector2D& a, vec_t b, Vector2D& c)
{
	vec_t oob = 1.0f / b;
	c.x = a.x * oob;
	c.y = a.y * oob;
}

inline void Vector2DDivide(const Vector2D& a, const Vector2D& b, Vector2D& c)
{
	c.x = a.x / b.x;
	c.y = a.y / b.y;
}

inline void Vector2DMA(const Vector2D& start, float s, const Vector2D& dir, Vector2D& result)
{
	result.x = start.x + s * dir.x;
	result.y = start.y + s * dir.y;
}

// FIXME: Remove
// For backwards compatability
inline void	Vector2D::MulAdd(const Vector2D& a, const Vector2D& b, float scalar)
{
	x = a.x + b.x * scalar;
	y = a.y + b.y * scalar;
}

inline void Vector2DLerp(const Vector2D& src1, const Vector2D& src2, vec_t t, Vector2D& dest)
{
	dest[0] = src1[0] + (src2[0] - src1[0]) * t;
	dest[1] = src1[1] + (src2[1] - src1[1]) * t;
}

//-----------------------------------------------------------------------------
// dot, cross
//-----------------------------------------------------------------------------
inline vec_t DotProduct2D(const Vector2D& a, const Vector2D& b)
{
	return(a.x * b.x + a.y * b.y);
}

// for backwards compatability
inline vec_t Vector2D::Dot(const Vector2D& vOther) const
{
	return DotProduct2D(*this, vOther);
}


//-----------------------------------------------------------------------------
// length
//-----------------------------------------------------------------------------
inline vec_t Vector2DLength(const Vector2D& v)
{
	return (vec_t)FastSqrt(v.x * v.x + v.y * v.y);
}

inline vec_t Vector2D::LengthSqr(void) const
{
	return (x * x + y * y);
}

inline bool Vector2D::IsLengthGreaterThan(float val) const
{
	return LengthSqr() > val * val;
}

inline bool Vector2D::IsLengthLessThan(float val) const
{
	return LengthSqr() < val * val;
}

inline vec_t Vector2D::Length(void) const
{
	return Vector2DLength(*this);
}


inline void Vector2DMin(const Vector2D& a, const Vector2D& b, Vector2D& result)
{
	result.x = (a.x < b.x) ? a.x : b.x;
	result.y = (a.y < b.y) ? a.y : b.y;
}


inline void Vector2DMax(const Vector2D& a, const Vector2D& b, Vector2D& result)
{
	result.x = (a.x > b.x) ? a.x : b.x;
	result.y = (a.y > b.y) ? a.y : b.y;
}


//-----------------------------------------------------------------------------
// Normalization
//-----------------------------------------------------------------------------
inline vec_t Vector2DNormalize(Vector2D& v)
{
	vec_t l = v.Length();
	if (l != 0.0f)
	{
		v /= l;
	}
	else
	{
		v.x = v.y = 0.0f;
	}
	return l;
}


//-----------------------------------------------------------------------------
// Get the distance from this Vector2D to the other one 
//-----------------------------------------------------------------------------
inline vec_t Vector2D::DistTo(const Vector2D& vOther) const
{
	Vector2D delta;
	Vector2DSubtract(*this, vOther, delta);
	return delta.Length();
}

inline vec_t Vector2D::DistToSqr(const Vector2D& vOther) const
{
	Vector2D delta;
	Vector2DSubtract(*this, vOther, delta);
	return delta.LengthSqr();
}


//-----------------------------------------------------------------------------
// Computes the closest point to vecTarget no farther than flMaxDist from vecStart
//-----------------------------------------------------------------------------
inline void ComputeClosestPoint2D(const Vector2D& vecStart, float flMaxDist, const Vector2D& vecTarget, Vector2D* pResult)
{
	Vector2D vecDelta;
	Vector2DSubtract(vecTarget, vecStart, vecDelta);
	float flDistSqr = vecDelta.LengthSqr();
	if (flDistSqr <= flMaxDist * flMaxDist)
	{
		*pResult = vecTarget;
	}
	else
	{
		vecDelta /= FastSqrt(flDistSqr);
		Vector2DMA(vecStart, flMaxDist, vecDelta, *pResult);
	}
}

class VectorByValue;

//=========================================================
// 3D Vector
//=========================================================
class Vector
{
public:
	// Members
	vec_t x, y, z;

	// Construction/destruction:
	Vector(void);
	Vector(vec_t X, vec_t Y, vec_t Z);
	explicit Vector(vec_t XYZ); ///< broadcast initialize

	// Initialization
	void Init(vec_t ix = 0.0f, vec_t iy = 0.0f, vec_t iz = 0.0f);
	// TODO (Ilya): Should there be an init that takes a single float for consistency?

   // Got any nasty NAN's?
	bool IsValid() const;
	void Invalidate();

	// array access...
	vec_t operator[](int i) const;
	vec_t& operator[](int i);

	// Base address...
	vec_t* Base();
	vec_t const* Base() const;

	// Cast to Vector2D...
	Vector2D& AsVector2D();
	const Vector2D& AsVector2D() const;

	// Initialization methods
	void Random(vec_t minVal, vec_t maxVal);
	inline void Zero(); ///< zero out a vector

	// equality
	bool operator==(const Vector& v) const;
	bool operator!=(const Vector& v) const;

	// arithmetic operations
	FORCEINLINE Vector& operator+=(const Vector& v);
	FORCEINLINE Vector& operator-=(const Vector& v);
	FORCEINLINE Vector& operator*=(const Vector& v);
	FORCEINLINE Vector& operator*=(float s);
	FORCEINLINE Vector& operator/=(const Vector& v);
	FORCEINLINE Vector& operator/=(float s);
	FORCEINLINE Vector& operator+=(float fl); ///< broadcast add
	FORCEINLINE Vector& operator-=(float fl); ///< broadcast sub			

	// negate the vector components
	void	Negate();

	// Get the vector's magnitude.
	inline vec_t	Length() const;

	// Get the vector's magnitude squared.
	FORCEINLINE vec_t LengthSqr(void) const
	{
		CHECK_VALID(*this);
		return (x * x + y * y + z * z);
	}

	// return true if this vector is (0,0,0) within tolerance
	bool IsZero(float tolerance = 0.01f) const
	{
		return (x > -tolerance && x < tolerance&&
			y > -tolerance && y < tolerance&&
			z > -tolerance && z < tolerance);
	}

	vec_t	NormalizeInPlace();
	Vector	Normalized() const;
	bool	IsLengthGreaterThan(float val) const;
	bool	IsLengthLessThan(float val) const;

	// check if a vector is within the box defined by two other vectors
	FORCEINLINE bool WithinAABox(Vector const& boxmin, Vector const& boxmax);

	// Get the distance from this vector to the other one.
	vec_t	DistTo(const Vector& vOther) const;

	// Get the distance from this vector to the other one squared.
	// NJS: note, VC wasn't inlining it correctly in several deeply nested inlines due to being an 'out of line' inline.  
	// may be able to tidy this up after switching to VC7
	FORCEINLINE vec_t DistToSqr(const Vector& vOther) const
	{
		Vector delta;

		delta.x = x - vOther.x;
		delta.y = y - vOther.y;
		delta.z = z - vOther.z;

		return delta.LengthSqr();
	}

	// Copy
	void	CopyToArray(float* rgfl) const;

	// Multiply, add, and assign to this (ie: *this = a + b * scalar). This
	// is about 12% faster than the actual vector equation (because it's done per-component
	// rather than per-vector).
	void	MulAdd(const Vector& a, const Vector& b, float scalar);

	// Dot product.
	vec_t	Dot(const Vector& vOther) const;

	// assignment
	Vector& operator=(const Vector& vOther);

	// 2d
	vec_t	Length2D(void) const;
	vec_t	Length2DSqr(void) const;

	operator VectorByValue& () { return *((VectorByValue*)(this)); }
	operator const VectorByValue& () const { return *((const VectorByValue*)(this)); }

#ifndef VECTOR_NO_SLOW_OPERATIONS
	// copy constructors
//	Vector(const Vector &vOther);

	// arithmetic operations
	Vector	operator-(void) const;

	Vector	operator+(const Vector& v) const;
	Vector	operator-(const Vector& v) const;
	Vector	operator*(const Vector& v) const;
	Vector	operator/(const Vector& v) const;
	Vector	operator*(float fl) const;
	Vector	operator/(float fl) const;

	// Cross product between two vectors.
	Vector	Cross(const Vector& vOther) const;

	// Returns a vector with the min or max in X, Y, and Z.
	Vector	Min(const Vector& vOther) const;
	Vector	Max(const Vector& vOther) const;

#else

private:
	// No copy constructors allowed if we're in optimal mode
	Vector(const Vector& vOther);
#endif
};

FORCEINLINE void NetworkVarConstruct(Vector& v) { v.Zero(); }

class VectorByValue : public Vector
{
public:
	// Construction/destruction:
	VectorByValue(void) : Vector() {}
	VectorByValue(vec_t X, vec_t Y, vec_t Z) : Vector(X, Y, Z) {}
	VectorByValue(const VectorByValue& vOther) { *this = vOther; }
};


//-----------------------------------------------------------------------------
// Utility to simplify table construction. No constructor means can use
// traditional C-style initialization
//-----------------------------------------------------------------------------
class TableVector
{
public:
	vec_t x, y, z;

	operator Vector& () { return *((Vector*)(this)); }
	operator const Vector& () const { return *((const Vector*)(this)); }

	// array access...
	inline vec_t& operator[](int i)
	{
		return ((vec_t*)this)[i];
	}

	inline vec_t operator[](int i) const
	{
		return ((vec_t*)this)[i];
	}
};

inline Vector::Vector(void)
{
	x = y = z = VEC_T_NAN;
}

inline Vector::Vector(vec_t X, vec_t Y, vec_t Z)
{
	x = X; y = Y; z = Z;
}

inline Vector::Vector(vec_t XYZ)
{
	x = y = z = XYZ;
}

//inline Vector::Vector(const float *pFloat)					
//{
//	Assert( pFloat );
//	x = pFloat[0]; y = pFloat[1]; z = pFloat[2];	
//	CHECK_VALID(*this);
//} 

#if 0
//-----------------------------------------------------------------------------
// copy constructor
//-----------------------------------------------------------------------------

inline Vector::Vector(const Vector& vOther)
{
	CHECK_VALID(vOther);
	x = vOther.x; y = vOther.y; z = vOther.z;
}
#endif

//-----------------------------------------------------------------------------
// initialization
//-----------------------------------------------------------------------------

inline void Vector::Init(vec_t ix, vec_t iy, vec_t iz)
{
	x = ix; y = iy; z = iz;
	CHECK_VALID(*this);
}

// This should really be a single opcode on the PowerPC (move r0 onto the vec reg)
inline void Vector::Zero()
{
	x = y = z = 0.0f;
}

inline void VectorClear(Vector& a)
{
	a.x = a.y = a.z = 0.0f;
}

//-----------------------------------------------------------------------------
// assignment
//-----------------------------------------------------------------------------

inline Vector& Vector::operator=(const Vector& vOther)
{
	CHECK_VALID(vOther);
	x = vOther.x; y = vOther.y; z = vOther.z;
	return *this;
}


//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------
inline vec_t& Vector::operator[](int i)
{
	return ((vec_t*)this)[i];
}

inline vec_t Vector::operator[](int i) const
{
	return ((vec_t*)this)[i];
}


//-----------------------------------------------------------------------------
// Base address...
//-----------------------------------------------------------------------------
inline vec_t* Vector::Base()
{
	return (vec_t*)this;
}

inline vec_t const* Vector::Base() const
{
	return (vec_t const*)this;
}

//-----------------------------------------------------------------------------
// Cast to Vector2D...
//-----------------------------------------------------------------------------

inline Vector2D& Vector::AsVector2D()
{
	return *(Vector2D*)this;
}

inline const Vector2D& Vector::AsVector2D() const
{
	return *(const Vector2D*)this;
}

//-----------------------------------------------------------------------------
// IsValid?
//-----------------------------------------------------------------------------

//inline bool Vector::IsValid() const
//{
//	return IsFinite(x) && IsFinite(y) && IsFinite(z);
//}

//-----------------------------------------------------------------------------
// Invalidate
//-----------------------------------------------------------------------------

inline void Vector::Invalidate()
{
	//#ifdef _DEBUG
	//#ifdef VECTOR_PARANOIA
	x = y = z = VEC_T_NAN;
	//#endif
	//#endif
}

//-----------------------------------------------------------------------------
// comparison
//-----------------------------------------------------------------------------

inline bool Vector::operator==(const Vector& src) const
{
	CHECK_VALID(src);
	CHECK_VALID(*this);
	return (src.x == x) && (src.y == y) && (src.z == z);
}

inline bool Vector::operator!=(const Vector& src) const
{
	CHECK_VALID(src);
	CHECK_VALID(*this);
	return (src.x != x) || (src.y != y) || (src.z != z);
}


//-----------------------------------------------------------------------------
// Copy
//-----------------------------------------------------------------------------

FORCEINLINE void VectorCopy(const Vector& src, Vector& dst)
{
	CHECK_VALID(src);
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}

inline void	Vector::CopyToArray(float* rgfl) const
{
	CHECK_VALID(*this);
	rgfl[0] = x, rgfl[1] = y, rgfl[2] = z;
}

//-----------------------------------------------------------------------------
// standard math operations
//-----------------------------------------------------------------------------
// #pragma message("TODO: these should be SSE")

FORCEINLINE float DotProduct(const Vector& a, const Vector& b)
{
	return(a.x * b.x + a.y * b.y + a.z * b.z);
}

inline void Vector::Negate()
{
	CHECK_VALID(*this);
	x = -x; y = -y; z = -z;
}

FORCEINLINE  Vector& Vector::operator+=(const Vector& v)
{
	CHECK_VALID(*this);
	CHECK_VALID(v);
	x += v.x; y += v.y; z += v.z;
	return *this;
}

FORCEINLINE  Vector& Vector::operator-=(const Vector& v)
{
	CHECK_VALID(*this);
	CHECK_VALID(v);
	x -= v.x; y -= v.y; z -= v.z;
	return *this;
}

FORCEINLINE  Vector& Vector::operator*=(float fl)
{
	x *= fl;
	y *= fl;
	z *= fl;
	CHECK_VALID(*this);
	return *this;
}

FORCEINLINE  Vector& Vector::operator*=(const Vector& v)
{
	CHECK_VALID(v);
	x *= v.x;
	y *= v.y;
	z *= v.z;
	CHECK_VALID(*this);
	return *this;
}

// this ought to be an opcode.
FORCEINLINE Vector& Vector::operator+=(float fl)
{
	x += fl;
	y += fl;
	z += fl;
	CHECK_VALID(*this);
	return *this;
}

FORCEINLINE Vector& Vector::operator-=(float fl)
{
	x -= fl;
	y -= fl;
	z -= fl;
	CHECK_VALID(*this);
	return *this;
}



FORCEINLINE  Vector& Vector::operator/=(float fl)
{
	float oofl = 1.0f / fl;
	x *= oofl;
	y *= oofl;
	z *= oofl;
	CHECK_VALID(*this);
	return *this;
}

FORCEINLINE  Vector& Vector::operator/=(const Vector& v)
{
	CHECK_VALID(v);
	x /= v.x;
	y /= v.y;
	z /= v.z;
	CHECK_VALID(*this);
	return *this;
}

class RadianEuler;
class DegreeEuler;
class QAngle;

class Quaternion				// same data-layout as engine's vec4_t,
{								//		which is a vec_t[4]
public:
	inline Quaternion(void) {
	}
	inline Quaternion(vec_t ix, vec_t iy, vec_t iz, vec_t iw) : x(ix), y(iy), z(iz), w(iw) { }
	inline explicit Quaternion(RadianEuler const& angle);
	inline explicit Quaternion(DegreeEuler const& angle);

	inline void Init(vec_t ix = 0.0f, vec_t iy = 0.0f, vec_t iz = 0.0f, vec_t iw = 0.0f) { x = ix; y = iy; z = iz; w = iw; }
	inline void Init(const Vector& vImaginaryPart, float flRealPart) { x = vImaginaryPart.x; y = vImaginaryPart.y; z = vImaginaryPart.z; w = flRealPart; }

	bool IsValid() const;
	void Invalidate();

	bool operator==(const Quaternion& src) const;
	bool operator!=(const Quaternion& src) const;

	inline Quaternion Conjugate() const { return Quaternion(-x, -y, -z, w); }

	// 
	const Vector GetForward()const;
	const Vector GetLeft()const;
	const Vector GetUp()const;

	vec_t* Base() { return (vec_t*)this; }
	const vec_t* Base() const { return (vec_t*)this; }

	// convenience for debugging
	inline void Print() const;

	// Imaginary part
	Vector& ImaginaryPart() { return *(Vector*)this; }
	const Vector& ImaginaryPart() const { return *(Vector*)this; }
	float& RealPart() { return w; }
	float RealPart() const { return w; }
	inline QAngle ToQAngle() const;
	inline struct matrix3x4_t ToMatrix() const;

	// array access...
	vec_t operator[](int i) const;
	vec_t& operator[](int i);

	inline Quaternion operator+(void) const { return *this; }
	inline Quaternion operator-(void) const { return Quaternion(-x, -y, -z, -w); }

	vec_t x, y, z, w;
};

// Random Quaternion that is UNIFORMLY distributed over the S^3
// should be good for random generation of orientation for unit tests and for game
// NOTE: Nothing trivial like Quaternion(RandomAngle(0,180)) will do the trick , 
//       one needs to take special care to generate a uniformly distributed quaternion.
const Quaternion RandomQuaternion();
const Quaternion RandomQuaternion();
inline const Quaternion Conjugate(const Quaternion& q)
{
	return Quaternion(-q.x, -q.y, -q.z, q.w);
}



//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------
inline vec_t& Quaternion::operator[](int i)
{
	return ((vec_t*)this)[i];
}

inline vec_t Quaternion::operator[](int i) const
{
	return ((vec_t*)this)[i];
}


//-----------------------------------------------------------------------------
// Equality test
//-----------------------------------------------------------------------------
inline bool Quaternion::operator==(const Quaternion& src) const
{
	return (x == src.x) && (y == src.y) && (z == src.z) && (w == src.w);
}

inline bool Quaternion::operator!=(const Quaternion& src) const
{
	return !operator==(src);
}



//-----------------------------------------------------------------------------
// Binaray operators
//-----------------------------------------------------------------------------
inline Quaternion operator+(const Quaternion& q1, const Quaternion& q2)
{
	return Quaternion(q1.x + q2.x, q1.y + q2.y, q1.z + q2.z, q1.w + q2.w);
}

inline Quaternion operator-(const Quaternion& q1, const Quaternion& q2)
{
	return Quaternion(q1.x - q2.x, q1.y - q2.y, q1.z - q2.z, q1.w - q2.w);
}

inline Quaternion operator*(float s, const Quaternion& q)
{
	return Quaternion(s * q.x, s * q.y, s * q.z, s * q.w);
}

inline Quaternion operator*(const Quaternion& q, float s)
{
	return Quaternion(q.x * s, q.y * s, q.z * s, q.w * s);
}

inline Quaternion operator/(const Quaternion& q, float s)
{
	return Quaternion(q.x / s, q.y / s, q.z / s, q.w / s);
}

class DegreeEuler
{
public:
	///\name Initialization 
	//@{
	inline DegreeEuler(void) ///< Create with un-initialized components. If VECTOR_PARANOIA is set, will init with NANS.
	{
	}
	inline DegreeEuler(vec_t X, vec_t Y, vec_t Z) { x = X; y = Y; z = Z; }
	inline explicit DegreeEuler(Quaternion const& q);
	inline explicit DegreeEuler(QAngle const& angles);
	inline explicit DegreeEuler(RadianEuler const& angles);

	// Initialization
	inline void Init(vec_t ix = 0.0f, vec_t iy = 0.0f, vec_t iz = 0.0f) { x = ix; y = iy; z = iz; }

	inline QAngle ToQAngle() const;

	//	conversion to qangle
	bool IsValid() const;
	void Invalidate();

	inline vec_t* Base() { return &x; }
	inline const vec_t* Base() const { return &x; }

	// array access...
	vec_t operator[](int i) const;
	vec_t& operator[](int i);

	vec_t x, y, z;
};

class RadianEuler
{
public:
	inline RadianEuler(void) { }
	inline RadianEuler(vec_t X, vec_t Y, vec_t Z) { x = X; y = Y; z = Z; }
	inline explicit RadianEuler(Quaternion const& q);
	inline explicit RadianEuler(QAngle const& angles);
	inline explicit RadianEuler(DegreeEuler const& angles);

	// Initialization
	inline void Init(vec_t ix = 0.0f, vec_t iy = 0.0f, vec_t iz = 0.0f) { x = ix; y = iy; z = iz; }

	//	conversion to qangle
	QAngle ToQAngle(void) const;
	bool IsValid() const;
	void Invalidate();

	inline vec_t* Base() { return &x; }
	inline const vec_t* Base() const { return &x; }

	// array access...
	vec_t operator[](int i) const;
	vec_t& operator[](int i);

	vec_t x, y, z;
};

struct cplane_t //gonna put this crapass shit here
{
	Vector	normal;
	float	dist;
	byte	type;			// for fast side tests
	byte	signbits;		// signx + (signy<<1) + (signz<<1)
	byte	pad[2];
};