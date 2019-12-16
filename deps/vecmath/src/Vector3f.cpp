#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <algorithm>

#include "Vector3f.h"
#include "Vector2f.h"

//////////////////////////////////////////////////////////////////////////
// Public
//////////////////////////////////////////////////////////////////////////

// static
const Vector3f Vector3f::ZERO = Vector3f( 0, 0, 0 );

// static
const Vector3f Vector3f::UP = Vector3f( 0, 1, 0 );

// static
const Vector3f Vector3f::RIGHT = Vector3f( 1, 0, 0 );

// static
const Vector3f Vector3f::FORWARD = Vector3f( 0, 0, -1 );

// static
const Vector3f Vector3f::INF_LARGE = Vector3f( std::numeric_limits<double>::max(),
                                               std::numeric_limits<double>::max(),
                                               std::numeric_limits<double>::max());


Vector3f::Vector3f( double f )
{
    m_elements[0] = f;
    m_elements[1] = f;
    m_elements[2] = f;
}

Vector3f::Vector3f( double x, double y, double z )
{
    m_elements[0] = x;
    m_elements[1] = y;
    m_elements[2] = z;
}

Vector3f::Vector3f( const Vector2f& xy, double z )
{
	m_elements[0] = xy.x();
	m_elements[1] = xy.y();
	m_elements[2] = z;
}

Vector3f::Vector3f( double x, const Vector2f& yz )
{
	m_elements[0] = x;
	m_elements[1] = yz.x();
	m_elements[2] = yz.y();
}

Vector3f::Vector3f( const Vector3f& rv )
{
    m_elements[0] = rv[0];
    m_elements[1] = rv[1];
    m_elements[2] = rv[2];
}

Vector3f& Vector3f::operator = ( const Vector3f& rv )
{
    if( this != &rv )
    {
        m_elements[0] = rv[0];
        m_elements[1] = rv[1];
        m_elements[2] = rv[2];
    }
    return *this;
}

const double& Vector3f::operator [] ( int i ) const
{
    return m_elements[i];
}

double& Vector3f::operator [] ( int i )
{
    return m_elements[i];
}

double& Vector3f::x()
{
    return m_elements[0];
}

double& Vector3f::y()
{
    return m_elements[1];
}

double& Vector3f::z()
{
    return m_elements[2];
}

double Vector3f::x() const
{
    return m_elements[0];
}

double Vector3f::y() const
{
    return m_elements[1];
}

double Vector3f::z() const
{
    return m_elements[2];
}

Vector2f Vector3f::xy() const
{
	return Vector2f( m_elements[0], m_elements[1] );
}

Vector2f Vector3f::xz() const
{
	return Vector2f( m_elements[0], m_elements[2] );
}

Vector2f Vector3f::yz() const
{
	return Vector2f( m_elements[1], m_elements[2] );
}

Vector3f Vector3f::xyz() const
{
	return Vector3f( m_elements[0], m_elements[1], m_elements[2] );
}

Vector3f Vector3f::yzx() const
{
	return Vector3f( m_elements[1], m_elements[2], m_elements[0] );
}

Vector3f Vector3f::zxy() const
{
	return Vector3f( m_elements[2], m_elements[0], m_elements[1] );
}

double Vector3f::length() const
{
	return sqrt( m_elements[0] * m_elements[0] + m_elements[1] * m_elements[1] + m_elements[2] * m_elements[2] );
}

double Vector3f::squaredLength() const
{
    return
        (
            m_elements[0] * m_elements[0] +
            m_elements[1] * m_elements[1] +
            m_elements[2] * m_elements[2]
        );
}

void Vector3f::normalize()
{
	double norm = length();
	m_elements[0] /= norm;
	m_elements[1] /= norm;
	m_elements[2] /= norm;
}

Vector3f Vector3f::normalized() const
{
	double norm = length();
	return Vector3f
		(
			m_elements[0] / norm,
			m_elements[1] / norm,
			m_elements[2] / norm
		);
}

Vector2f Vector3f::homogenized() const
{
	return Vector2f
		(
			m_elements[ 0 ] / m_elements[ 2 ],
			m_elements[ 1 ] / m_elements[ 2 ]
		);
}

void Vector3f::negate()
{
	m_elements[0] = -m_elements[0];
	m_elements[1] = -m_elements[1];
	m_elements[2] = -m_elements[2];
}

Vector3f::operator const double* () const
{
    return m_elements;
}

Vector3f::operator double* ()
{
    return m_elements;
}

void Vector3f::print() const
{
	printf( "< %.4f, %.4f, %.4f >\n",
		m_elements[0], m_elements[1], m_elements[2] );
}

Vector3f& Vector3f::operator += ( const Vector3f& v )
{
	m_elements[ 0 ] += v.m_elements[ 0 ];
	m_elements[ 1 ] += v.m_elements[ 1 ];
	m_elements[ 2 ] += v.m_elements[ 2 ];
	return *this;
}

Vector3f& Vector3f::operator -= ( const Vector3f& v )
{
	m_elements[ 0 ] -= v.m_elements[ 0 ];
	m_elements[ 1 ] -= v.m_elements[ 1 ];
	m_elements[ 2 ] -= v.m_elements[ 2 ];
	return *this;
}

Vector3f& Vector3f::operator *= ( double f )
{
	m_elements[ 0 ] *= f;
	m_elements[ 1 ] *= f;
	m_elements[ 2 ] *= f;
	return *this;
}

// static
double Vector3f::dot( const Vector3f& v0, const Vector3f& v1 )
{
    return v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2];
}

// static
Vector3f Vector3f::cross( const Vector3f& v0, const Vector3f& v1 )
{
    return Vector3f
        (
            v0.y() * v1.z() - v0.z() * v1.y(),
            v0.z() * v1.x() - v0.x() * v1.z(),
            v0.x() * v1.y() - v0.y() * v1.x()
        );
}

// static
Vector3f Vector3f::lerp( const Vector3f& v0, const Vector3f& v1, double alpha )
{
	return alpha * ( v1 - v0 ) + v0;
}

// static
Vector3f Vector3f::cubicInterpolate( const Vector3f& p0, const Vector3f& p1, const Vector3f& p2, const Vector3f& p3, double t )
{
	// geometric construction:
	//            t
	//   (t+1)/2     t/2
	// t+1        t	        t-1

	// bottom level
	Vector3f p0p1 = Vector3f::lerp( p0, p1, t + 1 );
	Vector3f p1p2 = Vector3f::lerp( p1, p2, t );
	Vector3f p2p3 = Vector3f::lerp( p2, p3, t - 1 );

	// middle level
	Vector3f p0p1_p1p2 = Vector3f::lerp( p0p1, p1p2, 0.5f * ( t + 1 ) );
	Vector3f p1p2_p2p3 = Vector3f::lerp( p1p2, p2p3, 0.5f * t );

	// top level
	return Vector3f::lerp( p0p1_p1p2, p1p2_p2p3, t );
}

void Vector3f::extendMin(const Vector3f& other) {
    m_elements[0] = std::fmin(m_elements[0], other.m_elements[0]);
    m_elements[1] = std::fmin(m_elements[1], other.m_elements[1]);
    m_elements[2] = std::fmin(m_elements[2], other.m_elements[2]);
}

void Vector3f::extendMax(const Vector3f& other) {
    m_elements[0] = std::fmax(m_elements[0], other.m_elements[0]);
    m_elements[1] = std::fmax(m_elements[1], other.m_elements[1]);
    m_elements[2] = std::fmax(m_elements[2], other.m_elements[2]);
}

Vector3f operator + ( const Vector3f& v0, const Vector3f& v1 )
{
    return Vector3f( v0[0] + v1[0], v0[1] + v1[1], v0[2] + v1[2] );
}

Vector3f operator - ( const Vector3f& v0, const Vector3f& v1 )
{
    return Vector3f( v0[0] - v1[0], v0[1] - v1[1], v0[2] - v1[2] );
}

Vector3f operator * ( const Vector3f& v0, const Vector3f& v1 )
{
    return Vector3f( v0[0] * v1[0], v0[1] * v1[1], v0[2] * v1[2] );
}

Vector3f operator / ( const Vector3f& v0, const Vector3f& v1 )
{
    return Vector3f( v0[0] / v1[0], v0[1] / v1[1], v0[2] / v1[2] );
}

Vector3f operator - ( const Vector3f& v )
{
    return Vector3f( -v[0], -v[1], -v[2] );
}

Vector3f operator * ( double f, const Vector3f& v )
{
    return Vector3f( v[0] * f, v[1] * f, v[2] * f );
}

Vector3f operator * ( const Vector3f& v, double f )
{
    return Vector3f( v[0] * f, v[1] * f, v[2] * f );
}

Vector3f operator / ( const Vector3f& v, double f )
{
    return Vector3f( v[0] / f, v[1] / f, v[2] / f );
}

bool operator == ( const Vector3f& v0, const Vector3f& v1 )
{
    return( v0.x() == v1.x() && v0.y() == v1.y() && v0.z() == v1.z() );
}

bool operator != ( const Vector3f& v0, const Vector3f& v1 )
{
    return !( v0 == v1 );
}

// Color processing

Vector3f Vector3f::confine(){
	return Vector3f(std::min(x(), 1.0), std::min(y(), 1.0), std::min(z(), 1.0));
}

Vector3f Vector3f::exp(){
	return Vector3f(std::exp(x()), std::exp(y()), std::exp(z()));
}

double Vector3f::power(){
	return ( x() + y() + z() ) / 3.0f;
}

bool Vector3f::isBlack() const{
	return std::fabs(x()) < eps && std::fabs(y()) < eps && std::fabs(z()) < eps;
}


Vector3f Vector3f::randomRay(const Vector3f &normal){
	// #define randFloat (rand() % 10001/10000.0)
	#define randFloat (erand48(X))
	// double r1 = 2 * M_PI * randFloat, r2 = randFloat, r2s = sqrt(r2);
	// Vector3f w = normal;
	// Vector3f u = (fabs(w.x()) > 0.2 ? Vector3f(0, 1, 0) : Vector3f(1, 0, 0)).normalized();
	// Vector3f v = Vector3f::cross(w, u);
	// return (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2)).normalized();
	Vector3f ret;
	do
		ret = Vector3f(double(randFloat - 0.5), double(randFloat - 0.5), double(randFloat - 0.5)).normalized();
	while (Vector3f::dot(normal, ret) <= 0);
	return ret;
}

Vector3f Vector3f::minimum(const Vector3f &a) const{
	return Vector3f(std::min(x(), a.x()), std::min(y(), a.y()), std::min(z(), a.z()));
}
Vector3f Vector3f::maximum(const Vector3f &a) const{
	return Vector3f(std::max(x(), a.x()), std::max(y(), a.y()), std::max(z(), a.z()));
}

Vector3f operator - ( const Vector3f& v, const double& r ){
	return Vector3f(v.x() - r, v.y() - r, v.z() - r);
}
Vector3f operator + ( const Vector3f& v, const double& r ){
	return Vector3f(v.x() + r, v.y() + r, v.z() + r);
}