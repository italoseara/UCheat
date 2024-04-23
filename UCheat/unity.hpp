#ifndef UNITY_HPP
#define UNITY_HPP

#include <vector>
#include "unturned.hpp"

namespace Unity
{
	class Transform;
	class GameObject;
	class Vector3;
	class String;

	class String
	{
	public:
		uint32_t length()
		{
			return Memory::read<uint32_t>(THISPTR + 0x10);
		}

		string to_string()
		{
			uintptr_t address = THISPTR + 0x14;
			uint32_t size = length() * sizeof(wchar_t);

			const auto buffer = make_unique<wchar_t[]>(size);
			ReadProcessMemory(Memory::proc_handle, (LPVOID)address, buffer.get(), size, NULL);

			wstring wstr(buffer.get());
			using convert_typeX = codecvt_utf8<wchar_t>;
			wstring_convert<convert_typeX, wchar_t> converterX;

			return converterX.to_bytes(wstr);
		}
	};

	class Vector3
	{
	public:
		float x, y, z;

		Vector3() : x(0), y(0), z(0) {}
		Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
		
		inline float dot(const Vector3& other) const
		{
			return x * other.x + y * other.y + z * other.z;
		}

		inline float distance(const Vector3& other) const
		{
			return sqrtf(powf(x - other.x, 2) + powf(y - other.y, 2) + powf(z - other.z, 2));
		}

		inline float length() const
		{
			return sqrtf(x * x + y * y + z * z);
		}

		Vector3 normalized() const
		{
			float len = length();
			return Vector3(x / len, y / len, z / len);
		}

		string to_string() const
		{
			return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
		}

		bool operator==(const Vector3& other) const
		{
			return x == other.x && y == other.y && z == other.z;
		}

		bool operator!=(const Vector3& other) const
		{
			return x != other.x || y != other.y || z != other.z;
		}

		Vector3 operator+(const Vector3& other) const
		{
			return Vector3(x + other.x, y + other.y, z + other.z);
		}

		Vector3 operator-(const Vector3& other) const
		{
			return Vector3(x - other.x, y - other.y, z - other.z);
		}

		Vector3 operator*(float scalar) const
		{
			return Vector3(x * scalar, y * scalar, z * scalar);
		}

		Vector3 operator/(float scalar) const
		{
			return Vector3(x / scalar, y / scalar, z / scalar);
		}

		Vector3& operator+=(const Vector3& other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
			return *this;
		}

		Vector3& operator-=(const Vector3& other)
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
			return *this;
		}

		Vector3& operator*=(float scalar)
		{
			x *= scalar;
			y *= scalar;
			z *= scalar;
			return *this;
		}

		Vector3& operator/=(float scalar)
		{
			x /= scalar;
			y /= scalar;
			z /= scalar;
			return *this;
		}
	};

	template <typename T>
	class List
	{
	public:
		uint32_t count()
		{
			return Memory::read<uint32_t>(THISPTR + 0x18);
		}

		T get(uint32_t index)
		{
			return Memory::read<T>(Memory::read<uintptr_t>(THISPTR + 0x10) + 0x20 + (index * sizeof(T)));
		}

		vector<T> to_vector()
		{
			vector<T> vec;
			for (uint32_t i = 0; i < count(); i++)
				vec.push_back(get(i));
			return vec;
		}
	};

	class Transform
	{
	public:
		FIELD_DEF(Vector3, local_position, 0x90);
	};

	class GameObject
	{
	public:
		Transform* transform() // credit: frankie-11
		{
			uintptr_t components = Memory::read<uintptr_t>(THISPTR + 0x30);
			return (Transform*)Memory::read<uintptr_t>(Memory::read<uintptr_t>(components + 0x8) + 0x38);
		}
	};
}

#endif // UNITY_HPP
