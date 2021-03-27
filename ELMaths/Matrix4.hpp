#pragma once
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "Maths.hpp"
#include "Quaternion.hpp"
#include <ELCore/ByteReader.hpp>
#include <ELCore/ByteWriter.hpp>
#include <ELCore/String.hpp>

template <typename T>
union Matrix4T
{
private:
	T _data[4][4];

public:
	struct
	{
		T m00; T m01; T m02; T m03;
		T m10; T m11; T m12; T m13;
		T m20; T m21; T m22; T m23;
		T m30; T m31; T m32; T m33;
	};

	Matrix4T(
		const T& m00, const T& m01, const T& m02, const T& m03,
		const T& m10, const T& m11, const T& m12, const T& m13,
		const T& m20, const T& m21, const T& m22, const T& m23,
		const T& m30, const T& m31, const T& m32, const T& m33
	)
		: m00(m00), m01(m01), m02(m02), m03(m03),
		m10(m10), m11(m11), m12(m12), m13(m13),
		m20(m20), m21(m21), m22(m22), m23(m23),
		m30(m30), m31(m31), m32(m32), m33(m33) {}

	Matrix4T() : Matrix4T(
		(T)1, (T)0, (T)0, (T)0, 
		(T)0, (T)1, (T)0, (T)0, 
		(T)0, (T)0, (T)1, (T)0, 
		(T)0, (T)0, (T)0, (T)1) {}

	Matrix4T(const Matrix4T& other) :
		m00(other.m00), m01(other.m01), m02(other.m02), m03(other.m03),
		m10(other.m10), m11(other.m11), m12(other.m12), m13(other.m13),
		m20(other.m20), m21(other.m21), m22(other.m22), m23(other.m23),
		m30(other.m30), m31(other.m31), m32(other.m32), m33(other.m33)
	{}

	T* operator[](int row) { return _data[row]; }
	const T* operator[](int row) const { return _data[row]; }

	Matrix4T& operator=(const Matrix4T& other)
	{
		m00 = other.m00;	m01 = other.m01; m02 = other.m02; m03 = other.m03;
		m10 = other.m10;	m11 = other.m11; m12 = other.m12; m13 = other.m13;
		m20 = other.m20;	m21 = other.m21; m22 = other.m22; m23 = other.m23;
		m30 = other.m30;	m31 = other.m31; m32 = other.m32; m33 = other.m33;
		return *this;
	}

	Matrix4T& operator+=(const Matrix4T& other)
	{
		m00 += other.m00;	m01 += other.m01; m02 += other.m02; m03 += other.m03;
		m10 += other.m10;	m11 += other.m11; m12 += other.m12; m13 += other.m13;
		m20 += other.m20;	m21 += other.m21; m22 += other.m22; m23 += other.m23;
		m30 += other.m30;	m31 += other.m31; m32 += other.m32; m33 += other.m33;
		return *this;
	}

	Matrix4T& operator-=(const Matrix4T& other)
	{
		m00 -= other.m00;	m01 -= other.m01; m02 -= other.m02; m03 -= other.m03;
		m10 -= other.m10;	m11 -= other.m11; m12 -= other.m12; m13 -= other.m13;
		m20 -= other.m20;	m21 -= other.m21; m22 -= other.m22; m23 -= other.m23;
		m30 -= other.m30;	m31 -= other.m31; m32 -= other.m32; m33 -= other.m33;
		return *this;
	}

	Matrix4T& operator*=(const T& scalar)
	{
		m00 *= scalar; m01 *= scalar; m02 *= scalar; m03 *= scalar;
		m10 *= scalar; m11 *= scalar; m12 *= scalar; m13 *= scalar;
		m20 *= scalar; m21 *= scalar; m22 *= scalar; m23 *= scalar;
		m30 *= scalar; m31 *= scalar; m32 *= scalar; m33 *= scalar;
		return *this;
	}

	Matrix4T operator*(const T& scalar) const
	{
		return Matrix4T(
			m00 * scalar, m01 * scalar, m02 * scalar, m03 * scalar,
			m10 * scalar, m11 * scalar, m12 * scalar, m13 * scalar,
			m20 * scalar, m21 * scalar, m22 * scalar, m23 * scalar,
			m30 * scalar, m31 * scalar, m32 * scalar, m33 * scalar
		);
	}

	Matrix4T operator*(const Matrix4T& other) const
	{
		return Matrix4T(
			m00 * other.m00 + m01 * other.m10 + m02 * other.m20 + m03 * other.m30, m00 * other.m01 + m01 * other.m11 + m02 * other.m21 + m03 * other.m31, m00 * other.m02 + m01 * other.m12 + m02 * other.m22 + m03 * other.m32, m00 * other.m03 + m01 * other.m13 + m02 * other.m23 + m03 * other.m33,
			m10 * other.m00 + m11 * other.m10 + m12 * other.m20 + m13 * other.m30, m10 * other.m01 + m11 * other.m11 + m12 * other.m21 + m13 * other.m31, m10 * other.m02 + m11 * other.m12 + m12 * other.m22 + m13 * other.m32, m10 * other.m03 + m11 * other.m13 + m12 * other.m23 + m13 * other.m33,
			m20 * other.m00 + m21 * other.m10 + m22 * other.m20 + m23 * other.m30, m20 * other.m01 + m21 * other.m11 + m22 * other.m21 + m23 * other.m31, m20 * other.m02 + m21 * other.m12 + m22 * other.m22 + m23 * other.m32, m20 * other.m03 + m21 * other.m13 + m22 * other.m23 + m23 * other.m33,
			m30 * other.m00 + m31 * other.m10 + m32 * other.m20 + m33 * other.m30, m30 * other.m01 + m31 * other.m11 + m32 * other.m21 + m33 * other.m31, m30 * other.m02 + m31 * other.m12 + m32 * other.m22 + m33 * other.m32, m30 * other.m03 + m31 * other.m13 + m32 * other.m23 + m33 * other.m33
		);
	}

	void MultiplyRow(int row, float x)
	{
		_data[row][0] *= x;
		_data[row][1] *= x;
		_data[row][2] *= x;
		_data[row][3] *= x;
	}

	void MultiplyColumn(int column, float x)
	{
		_data[0][column] *= x;
		_data[1][column] *= x;
		_data[2][column] *= x;
		_data[3][column] *= x;
	}

	//returns the inverse of the 3x3 part of this matrix (usually rotation/scale)
	//the result is indWorldObject outside of the 3x3 part
	Matrix4T Inverse3x3() const
	{
		//- 0	1	2	3
		//0	0a	1b	2c	3-
		//1	4d	5e	6f	7-
		//2	8g	9h	10i 11
		//3	12	13	14	15

		T determinant =		m00 * m11 * m22 + //aei
							m01 * m12 * m20 + //bfg
							m02 * m10 * m21 - //cdh
							m02 * m11 * m20 - //ceg
							m01 * m10 * m22 - //bdi
							m00 * m12 * m21;  //afh

		if (determinant)
		{
			Matrix4T result;

			//adjugate / det
			result[0][0] = (m11 * m22 - m12 * m21) / determinant; //ei-fh
			result[0][1] = (m02 * m21 - m01 * m22) / determinant; //ch-bi
			result[0][2] = (m01 * m12 - m02 * m11) / determinant; //bf-ce
			result[1][0] = (m12 * m20 - m10 * m22) / determinant; //fg-di
			result[1][1] = (m00 * m22 - m02 * m20) / determinant; //ai-cg
			result[1][2] = (m02 * m10 - m00 * m12) / determinant; //cd-af
			result[2][0] = (m10 * m21 - m11 * m20) / determinant; //dh-eg
			result[2][1] = (m01 * m20 - m00 * m21) / determinant; //bg-ah
			result[2][2] = (m00 * m11 - m01 * m10) / determinant; //ae-bd
			result[3][3] = (T)1;

			return result;
		}

		return Matrix4T();
	}

	Matrix4T Inverse() const
	{
		Matrix4T lu;

		//index: result row, value: source row
		int permutation[4];
		DecomposeLU(lu, permutation);

		//L * U * I == 1
		//U * I == X
		//L * X == 1
		//Solve for X using forward substitution, then solve for I using backward substitution

		Matrix4T x;

		//fwd sub for each column (diagonal of L is 1!)
		for (int c = 0; c < 4; ++c)
		{
			//(where V and R are 4d column vectors)
			x[0][c] = Identity()[0][c];																//1 * V1 = R1...
			x[1][c] = Identity()[1][c] - lu.m10 * x[0][c];											//V2 = R2 - L21 * V1
			x[2][c] = Identity()[2][c] - lu.m20 * x[0][c] - lu.m21 * x[1][c];						//V3 = R3 - L31 * V1 - L32 * V2
			x[3][c] = Identity()[3][c] - lu.m30 * x[0][c] - lu.m31 * x[1][c] - lu.m32 * x[2][c];	//V4 = R4 - L41 * V1 - L42 * V2 - L43 * V3
		}

		Matrix4T i;

		//back sub for each column
		for (int c = 0; c < 4; ++c)
		{
			//(where V is a 4d column vector and X is the right hand side)
			i[3][c] = x[3][c] / lu.m33;																//V4 = X4 / U44
			i[2][c] = (x[2][c] - lu.m23 * i[3][c]) / lu.m22;										//V3 = (X3 - U34 * V4) / U33
			i[1][c] = (x[1][c] - lu.m12 * i[2][c] - lu.m13 * i[3][c]) / lu.m11;						//V2 = (X2 - U23 * V3 - U24 * V4) / U22
			i[0][c] = (x[0][c] - lu.m01 * i[1][c] - lu.m02 * i[2][c] - lu.m03 * i[3][c]) / lu.m00;	//V1 = (X1 - U12 * V2 - U13 * V3 - U14 * V4) / U11
		}
		
		//i is now the inverse matrix of LU, which is a premutated factorisation of our matrix.
		return i.UndoColumnPermutation(permutation);
	}

	Matrix4T UndoRowPermutation(const int permutation[4]) const
	{
		Matrix4T result;
		for (int r = 0; r < 4; ++r)
			for (int c = 0; c < 4; ++c)
				result[permutation[r]][c] = _data[r][c];

		return result;
	}

	Matrix4T UndoColumnPermutation(const int permutation[4]) const
	{
		Matrix4T result;
		for (int r = 0; r < 4; ++r)
			for (int c = 0; c < 4; ++c)
				result[r][permutation[c]] = _data[r][c];

		return result;
	}

	//Decomposes a row-wise permutation of this matrix into LU triangles, where L00 = L11 = L22 = L33 = 1
	//lu is combined into a single matrix where the diagonal belongs to the upper triangle as the lower is implicitly 1
	//permutation maps result rows (index) to source rows (value)
	//fails if a pivot value in the 3x3 diagonal's absolute value is less than tolerance
	bool DecomposeLU(Matrix4T& lu, int permutation[4], T pivotTolerance = (T)0.000000001f) const
	{
		for (int i = 0; i < 4; ++i)
			permutation[i] = i;

		//Partial pivoting
		T absPivot;
		for (int i = 0; i < 3; ++i)
		{
			T maxPivot = Maths::Abs(_data[i][i]);
			int p = i;

			for (int r = i + 1; r < 4; ++r)
				if (absPivot = Maths::Abs(_data[r][i]) > maxPivot)
				{
					maxPivot = absPivot;
					p = r;
				}

			if (maxPivot < pivotTolerance) return false; //Too small! Degenerate!!!

			if (i != p)
				Utilities::Swap(permutation[i], permutation[p]);
		}

		const int& r0 = permutation[0];
		const int& r1 = permutation[1];
		const int& r2 = permutation[2];
		const int& r3 = permutation[3];

		//Upper triangle top row = m top row
		lu.m00 = _data[r0][0];
		lu.m01 = _data[r0][1];
		lu.m02 = _data[r0][2];
		lu.m03 = _data[r0][3];

		//column 1
		lu.m10 = _data[r1][0] / lu.m00; //L21 = M21 / U11
		lu.m20 = _data[r2][0] / lu.m00; //L31 = M31 / U11
		lu.m30 = _data[r3][0] / lu.m00; //L31 = M41 / U11

		//column 2
		lu.m11 = _data[r1][1] - lu.m10 * lu.m01;			//U22 = M22 - L21 * U12
		lu.m21 = (_data[r2][1] - lu.m20 * lu.m01) / lu.m11;	//L32 = (M32 - L31 * U12) / U22
		lu.m31 = (_data[r3][1] - lu.m30 * lu.m01) / lu.m11;	//L42 = (M42 - L41 * U12) / U22

		//column 3
		lu.m12 = _data[r1][2] - lu.m10 * lu.m02;								//U23 = M23 - L21 * U13
		lu.m22 = _data[r2][2] - lu.m20 * lu.m02 - lu.m21 * lu.m12;				//U33 = M33 - L31 * U13 - L32 * U23
		lu.m32 = (_data[r3][2] - lu.m30 * lu.m02 - lu.m31 * lu.m12) / lu.m22;	//(L43 = M43 - L41 * U13 - L42 * U23) / U33

		//column 4
		lu.m13 = _data[r1][3] - lu.m10 * lu.m03;										//U24 = M24 - L21 * 14
		lu.m23 = _data[r2][3] - lu.m20 * lu.m03 - lu.m21 * lu.m13;						//U34 = M34 - L31 * U14 - L32 * U24
		lu.m33 = _data[r3][3] - lu.m30 * lu.m03 - lu.m31 * lu.m13 - lu.m32 * lu.m23;	//U44 = M44 - L41 * U14 - L42 * U24 - L43 * U34
	
		return true;
	}

	void Write(ByteWriter& writer) const
	{
		for (int r = 0; r < 4; ++r)
			for (int c = 0; c < 4; ++c)
				writer.Write_float(_data[r][c]);
	}

	void Read(ByteReader& reader)
	{
		for (int r = 0; r < 4; ++r)
			for (int c = 0; c < 4; ++c)
				_data[r][c] = reader.Read_float();
	}

	String ToString(const char colSeperator = ", ", const char rowSeperator = "\n") const
	{
		return String::Concat(
			m00, colSeperator, m01, colSeperator, m02, colSeperator, m03, rowSeperator,
			m10, colSeperator, m11, colSeperator, m12, colSeperator, m13, rowSeperator,
			m20, colSeperator, m21, colSeperator, m22, colSeperator, m23, rowSeperator,
			m30, colSeperator, m31, colSeperator, m32, colSeperator, m33
		);
	}

	static const Matrix4T& Empty() 
	{ 
		static Matrix4T empty((T)0, (T)0, (T)0, (T)0, (T)0, (T)0, (T)0, (T)0, (T)0, (T)0, (T)0, (T)0, (T)0, (T)0, (T)0, (T)0);
		return empty;
	}

	static const Matrix4T& Identity()
	{
		static Matrix4T idWorldObject;
		return idWorldObject;
	}

	static Matrix4T RotationX(const T& degrees)
	{
		T sin = Maths::Sin(Maths::DegreesToRadians(degrees));
		T cos = Maths::Cos(Maths::DegreesToRadians(degrees));

		return Matrix4T(
			(T)1, (T)0, (T)0, (T)0,
			(T)0, cos, -sin, (T)0,
			(T)0, sin, cos, (T)0,
			(T)0, (T)0, (T)0, (T)1
		);
	}

	static Matrix4T RotationY(const T& degrees)
	{
		T sin = Maths::Sin(Maths::DegreesToRadians(degrees));
		T cos = Maths::Cos(Maths::DegreesToRadians(degrees));

		return Matrix4T(
			cos, (T)0, -sin, (T)0,
			(T)0, (T)1, (T)0, (T)0,
			sin, (T)0, cos, (T)0,
			(T)0, (T)0, (T)0, (T)1
		);
	}

	static Matrix4T RotationZ(const T& degrees)
	{
		T sin = Maths::Sin(Maths::DegreesToRadians(degrees));
		T cos = Maths::Cos(Maths::DegreesToRadians(degrees));

		return Matrix4T(
			cos, -sin, (T)0, (T)0,
			sin, cos, (T)0, (T)0,
			(T)0, (T)0, (T)1, (T)0,
			(T)0, (T)0, (T)0, (T)1
		);
	}

	static Matrix4T Scale(const Vector3T<T>& s)
	{
		return Matrix4T(
			s.x, (T)0, (T)0, (T)0,
			(T)0, s.y, (T)0, (T)0,
			(T)0, (T)0, s.z, (T)0,
			(T)0, (T)0, (T)0, (T)1
		);
	}

	static Matrix4T Translation(const Vector3T<T>& t)
	{
		return Matrix4T(
			(T)1, (T)0, (T)0, (T)0,
			(T)0, (T)1, (T)0, (T)0,
			(T)0, (T)0, (T)1, (T)0,
			t.x, t.y, t.z, (T)1
		);
	}

	static Matrix4T Rotation(const Quaternion& r)
	{
		float x = r.GetData().x;
		float y = r.GetData().y;
		float z = r.GetData().z;
		float w = r.GetData().w;
		return Matrix4T(
			1.f - 2.f * (y * y + z * z), 2.f * (x * y - z * w), 2.f * (x * z + y * w), 0.f,
			2.f * (x * y + z * w), 1.f - 2.f * (x * x + z * z), 2.f * (y * z - x * w), 0.f,
			2.f * (x * z - y * w), 2.f * (y * z + x * w), 1.f - 2.f * (x * x + y * y), 0.f,
			0.f, 0.f, 0.f, 1.f);
	}

	//euler angles are ZXY
	static Matrix4T Transformation(const Vector3T<T>& translation, const Vector3T<T>& euler, const Vector3T<T>& scale)
	{
		//Boiled-down version of Scale(scale) * RotationZ(euler.z) * RotationX(euler.x) * RotationY(euler.y) * Translation(translation)
		float sinX = Maths::Sin(Maths::DegreesToRadians(euler.x));
		float nSinX = -sinX;
		float sinY = Maths::Sin(Maths::DegreesToRadians(euler.y));
		float nSinY = -sinY;
		float sinZ = Maths::Sin(Maths::DegreesToRadians(euler.z));
		float cosX = Maths::Cos(Maths::DegreesToRadians(euler.x));
		float cosY = Maths::Cos(Maths::DegreesToRadians(euler.y));
		float cosZ = Maths::Cos(Maths::DegreesToRadians(euler.z));

		float xCosZ = scale.x * cosZ;
		float nySinZ = scale.x * -sinZ;
		float ySinZ = scale.y * sinZ;
		float yCosZ = scale.y * cosZ;
		float zCosX = scale.z * cosX;

		return Matrix4T(
			xCosZ * cosY + nySinZ * nSinX * sinY, nySinZ * cosX, xCosZ * nSinY + nySinZ * nSinX * cosY, 0.f,
			ySinZ * cosY + yCosZ * nSinX * sinY, yCosZ * cosX, ySinZ * nSinY + yCosZ * nSinX * cosY, 0.f,
			zCosX * sinY, scale.z * sinX, zCosX * cosY, 0.f,
			translation.x, translation.y, translation.z, 1.f
		);
	}

	static Matrix4T Transformation(const Vector3T<T>& translation, const Quaternion& rotation, const Vector3T<T>& scale)
	{
		return Scale(scale) * Rotation(rotation) * Translation(translation);
	}

	static Matrix4T ProjectionOrtho(const T& left, const T& right, const T& bottom, const T& top, const T& n, const T& f, const T& scale) 
	{
		return Matrix4T(
			(T)2 * scale / (right - left), (T)0, (T)0, (T)0,
			(T)0, (T)2 * scale / (top - bottom), (T)0, (T)0,
			(T)0, (T)0, (T)2 / (f - n), (T)0,
			(-right - left) / (right - left), (-top - bottom) / (top - bottom), (-f - n) / (f - n), (T)1
		);
	}

	static Matrix4T ProjectionPerspective(const T& vFovRadians, const T& n, const T& f, const T& aspectRatio)
	{
		T tan = Maths::Tan(vFovRadians / (T)2);

		return Matrix4T(
			(T)1 / aspectRatio * tan, (T)0, (T)0, (T)0,
			(T)0, (T)1 / tan, (T)0, (T)0,
			(T)0, (T)0, f / (f - n), (T)1,
			(T)0, (T)0, -(f * n) / (f - n), (T)0
		);
	}
};

typedef Matrix4T<float> Matrix4;

inline Vector4 operator*(const Vector4& v, const Matrix4& m)
{
	return Vector4(
		v.x * m.m00 + v.y * m.m10 + v.z * m.m20 + v.w * m.m30,
		v.x * m.m01 + v.y * m.m11 + v.z * m.m21 + v.w * m.m31,
		v.x * m.m02 + v.y * m.m12 + v.z * m.m22 + v.w * m.m32,
		v.x * m.m03 + v.y * m.m13 + v.z * m.m23 + v.w * m.m33);
}
