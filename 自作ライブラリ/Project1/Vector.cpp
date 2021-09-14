#include "Vector.h"

Vector2::Vector2()
{
	x = 0;
	y = 0;
}

Vector2::Vector2(const Vector2& vector)
{
	x = vector.x;
	y = vector.y;
}

Vector2::Vector2(const XMFLOAT2& vector)
{
	x = vector.x;
	y = vector.y;
}

Vector2::Vector2(const float& x, const float& y)
{
	this->x = x;
	this->y = y;
}

Vector2 Vector2::operator+(Vector2 vector)
{
	Vector2 ans;
	ans.x = x + vector.x;
	ans.y = y + vector.y;
	return ans;
}

Vector2 Vector2::operator-(Vector2 vector)
{
	Vector2 ans;
	ans.x = x - vector.x;
	ans.y = y - vector.y;
	return ans;
}

Vector2 Vector2::operator*(Vector2 vector)
{
	Vector2 ans;
	ans.x = x * vector.x;
	ans.y = y * vector.y;
	return ans;
}

Vector2 Vector2::operator/(Vector2 vector)
{
	Vector2 ans;
	ans.x = x / vector.x;
	ans.y = y / vector.y;
	return ans;
}

void Vector2::operator+=(Vector2 vector)
{
	x = x + vector.x;
	y = y + vector.y;
}

void Vector2::operator-=(Vector2 vector)
{
	x = x - vector.x;
	y = y - vector.y;
}

void Vector2::operator*=(Vector2 vector)
{
	x = x * vector.x;
	y = y * vector.y;
}

void Vector2::operator/= (Vector2 vector)
{
	x = x / vector.x;
	y = y / vector.y;
}

Vector2 Vector2::operator+(float f)
{
	Vector2 ans;
	ans.x = x + f;
	ans.y = y + f;
	return ans;
}

Vector2 Vector2::operator-(float f)
{
	Vector2 ans;
	ans.x = x - f;
	ans.y = y - f;
	return ans;
}

Vector2 Vector2::operator*(float f)
{
	Vector2 ans;
	ans.x = x * f;
	ans.y = y * f;
	return ans;
}

Vector2 Vector2::operator/(float f)
{
	Vector2 ans;
	ans.x = x / f;
	ans.y = y / f;
	return ans;
}

void Vector2::operator=(float num)
{
	x = num;
	y = num;
}

float Vector2::Length(Vector2 vector)
{
	return sqrt(vector.x * vector.x + vector.y*vector.y);
}

Vector2 Vector2::Normalize(Vector2 vector)
{
	float length = Length(vector);
	if (length != 0)
	{
		Vector2 ans;
		ans.x = vector.x / length;
		ans.y = vector.y / length;
		return ans;
	}
	return vector;
}

float Vector2::Cross(Vector2 vector1, Vector2 vector2)
{
	return vector1.x * vector2.y - vector1.y * vector2.x;
}

float Vector2::Dot(Vector2 vector1, Vector2 vector2)
{
	return vector1.x * vector2.x + vector1.y * vector2.y;
}





Vector3::Vector3()
{
	x = 0;
	y = 0;
	z = 0;
}

Vector3::Vector3(const Vector3 & vector)
{
	x = vector.x;
	y = vector.y;
	z = vector.z;
}

Vector3::Vector3(const XMFLOAT3 & vector)
{
	x = vector.x;
	y = vector.y;
	z = vector.z;
}

Vector3::Vector3(const DirectX::XMVECTOR& vector)
{
	x = vector.m128_f32[0];
	y = vector.m128_f32[1];
	z = vector.m128_f32[2];

}

Vector3::Vector3(const float & x, const float & y, const float & z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3& Vector3::operator*(Vector3 vector)
{
	Vector3 ans;
	ans.x = x * vector.x;
	ans.y = y * vector.y;
	ans.z = z * vector.z;
	return ans;
}

Vector3& Vector3::operator/(Vector3 vector)
{
	Vector3 ans;
	ans.x = x / vector.x;
	ans.y = y / vector.y;
	ans.z = z / vector.z;
	return ans;
}

Vector3& Vector3::operator+=(Vector3 vector)
{
	x = x + vector.x;
	y = y + vector.y;
	z = z + vector.z;
	return *this;
}

Vector3& Vector3::operator-=(Vector3 vector)
{
	x = x - vector.x;
	y = y - vector.y;
	z = z - vector.z;
	return *this;
}

Vector3& Vector3::operator*=(Vector3 vector)
{
	x = x * vector.x;
	y = y * vector.y;
	z = z * vector.z;
	return *this;
}

Vector3& Vector3::operator/=(Vector3 vector)
{
	x = x / vector.x;
	y = y / vector.y;
	z = z / vector.z;
	return *this;
}

bool Vector3::operator!=(Vector3 vector)
{
	if (x != vector.x)
		return true;
	if (y != vector.y)
		return true;
	if (z != vector.z)
		return true;
	return false;
}


Vector3& Vector3::operator+=(float f)
{
	x = x + f;
	y = y + f;
	z = z + f;
	return *this;
}

Vector3& Vector3::operator-=(float f)
{
	x = x - f;
	y = y - f;
	z = z - f;
	return *this;
}

Vector3& Vector3::operator*=(float f)
{
	x = x * f;
	y = y * f;
	z = z * f;
	return *this;
}

Vector3& Vector3::operator/=(float f)
{
	x = x / f;
	y = y / f;
	z = z / f;
	return *this;
}

Vector3& Vector3::operator=(float num)
{
	x = num;
	y = num;
	z = num;
	return *this;
}

float Vector3::Length()
{
	return sqrt(x*x + y * y + z * z);
}

Vector3& Vector3::Normalize()
{
	float length = Length();
	if (length != 0)
	{
		return *this /= length;
	}
	return *this;
}

Vector3 Vector3::Cross(Vector3 vector)
{
	Vector3 ans;
	ans.x = y * vector.z - z * vector.y;
	ans.y = z * vector.x - x * vector.z;
	ans.z = x * vector.y - y * vector.x;
	return ans;
}

float Vector3::Dot(Vector3 vector)
{
	return x * vector.x + y * vector.y + z * vector.z;
}

DirectX::XMFLOAT3 Vector3::ConvertFLOAT3()
{
	return DirectX::XMFLOAT3(x, y, z);
}

Vector3 Vector3::straight(Vector3 myPos, Vector3 otherPos)
{
	return Vector3();
}

float Vector3::Length(Vector3 vector)
{
	return sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

Vector3 Vector3::Normalize(Vector3 vector)
{
	float length = Length(vector);
	if (length != 0)
	{
		return vector /= length;
	}
	return vector;
}

Vector3 Vector3::Cross(Vector3 vector1, Vector3 vector2)
{
	Vector3 ans;
	ans.x = vector1.y * vector2.z - vector1.z * vector2.y;
	ans.y = vector1.z * vector2.x - vector1.x * vector2.z;
	ans.z = vector1.x * vector2.y - vector1.y * vector2.x;
	return ans;
}

float Vector3::Dot(Vector3 vector1, Vector3 vector2)
{
	return vector1.x * vector2.x + vector1.y * vector2.y + vector1.z * vector2.z;
}

DirectX::XMFLOAT3 Vector3::ConvertFLOAT3(const Vector3 & vector)
{
	DirectX::XMFLOAT3 convert = { vector.x,vector.y,vector.z };
	return convert;
}



const Vector3 operator+(const Vector3 & v1, const Vector3 & v2)
{
	Vector3 temp(v1);
	return temp += v2;
}

const Vector3 operator-(const Vector3 & v1, const Vector3 & v2)
{
	Vector3 temp(v1);
	return temp -= v2;
}

const Vector3 operator*(const Vector3 & v, float s)
{
	Vector3 temp(v);
	return temp *= s;
}

const Vector3 operator*(float s, const Vector3 & v)
{
	return v * s;
}

const Vector3 operator/(const Vector3 & v, float s)
{
	Vector3 temp(v);
	return temp /= s;
}
