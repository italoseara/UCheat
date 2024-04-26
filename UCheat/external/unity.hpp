#ifndef UNITY_HPP
#define UNITY_HPP

#include <vector>
#include "memory.hpp"
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

		string toString()
		{
			uintptr_t address = THISPTR + 0x14;
			uint32_t size = length() * sizeof(wchar_t);

			const auto buffer = make_unique<wchar_t[]>(size);
			Memory::readBuffer(address, buffer.get(), size);

			wstring wstr(buffer.get());
			using convert_typeX = codecvt_utf8<wchar_t>;
			wstring_convert<convert_typeX, wchar_t> converterX;

			return converterX.to_bytes(wstr);
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

		vector<T> toVector()
		{
			uintptr_t address = Memory::read<uintptr_t>(THISPTR + 0x10) + 0x20;
			uint32_t len = count();

			auto buffer = make_unique<T[]>(len);
			Memory::readBuffer(address, buffer.get(), len * sizeof(T));

			vector<T> vec(len);
			memcpy(vec.data(), buffer.get(), len * sizeof(T));
			return vec;
		}
	};

	template <typename T>
	class Array
	{
	public:
		uint32_t length()
		{
			return Memory::read<uint32_t>(THISPTR + 0x18);
		}

		T get(uint32_t index)
		{
			return Memory::read<T>(THISPTR + 0x20 + (index * sizeof(T)));
		}

		vector<T> toVector()
		{
			uint32_t address = THISPTR + 0x20;
			uint32_t len = length();
			
			auto buffer = make_unique<T[]>(len);
			Memory::readBuffer(address, buffer.get(), len * sizeof(T));

			vector<T> vec(len);
			memcpy(vec.data(), buffer.get(), len * sizeof(T));
			return vec;
		}
	};

	class Vector3
	{
	public:
		float x, y, z;

		Vector3() : x(0), y(0), z(0) {}
		Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

		inline bool operator==(const Vector3& other) const { return x == other.x && y == other.y && z == other.z; }
		inline bool operator!=(const Vector3& other) const { return x != other.x || y != other.y || z != other.z; }
		inline Vector3 operator+(const Vector3& other) const { return Vector3(x + other.x, y + other.y, z + other.z); }
		inline Vector3 operator-(const Vector3& other) const { return Vector3(x - other.x, y - other.y, z - other.z); }
		inline Vector3 operator*(const Vector3& other) const { return Vector3(x * other.x, y * other.y, z * other.z); }
		inline Vector3 operator/(const Vector3& other) const { return Vector3(x / other.x, y / other.y, z / other.z); }
		inline Vector3 operator*(float scalar) const { return Vector3(x * scalar, y * scalar, z * scalar); }
		inline Vector3 operator/(float scalar) const { return Vector3(x / scalar, y / scalar, z / scalar); }
		inline Vector3 operator-() const { return Vector3(-x, -y, -z); }

		inline Vector3& operator+=(const Vector3& other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
			return *this;
		}

		inline Vector3& operator-=(const Vector3& other)
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
			return *this;
		}

		inline Vector3& operator*=(float scalar)
		{
			x *= scalar;
			y *= scalar;
			z *= scalar;
			return *this;
		}

		inline Vector3& operator/=(float scalar)
		{
			x /= scalar;
			y /= scalar;
			z /= scalar;
			return *this;
		}

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

		string toString() const
		{
			return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
		}
	};

	class Quaternion
	{
	public:
		float x, y, z, w;

		Quaternion() : x(0), y(0), z(0), w(1) {}
		Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

		inline Quaternion operator*(float rhs) const { return Quaternion(x * rhs, y * rhs, z * rhs, w * rhs); }
		inline Quaternion operator+(const Quaternion& rhs) const { return Quaternion(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w); }
		inline Quaternion operator-(const Quaternion& rhs) const { return Quaternion(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w); }
		inline Quaternion operator/(const Quaternion& rhs) const { return Quaternion(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w); }
		inline Quaternion operator-() const { return Quaternion(-x, -y, -z, -w); }

		inline Quaternion operator*(const Quaternion& rhs) const
		{
			return Quaternion(
				w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
				w * rhs.y + y * rhs.w + z * rhs.x - x * rhs.z,
				w * rhs.z + z * rhs.w + x * rhs.y - y * rhs.x,
				w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z
			);
		}

		inline Vector3 operator*(const Vector3& rhs) const
		{
			float x = this->x * 2.0F;
			float y = this->y * 2.0F;
			float z = this->z * 2.0F;
			float xx = this->x * x;
			float yy = this->y * y;
			float zz = this->z * z;
			float xy = this->x * y;
			float xz = this->x * z;
			float yz = this->y * z;
			float wx = this->w * x;
			float wy = this->w * y;
			float wz = this->w * z;

			Vector3 res;
			res.x = (1.0f - (yy + zz)) * rhs.x + (xy - wz) * rhs.y + (xz + wy) * rhs.z;
			res.y = (xy + wz) * rhs.x + (1.0f - (xx + zz)) * rhs.y + (yz - wx) * rhs.z;
			res.z = (xz - wy) * rhs.x + (yz + wx) * rhs.y + (1.0f - (xx + yy)) * rhs.z;

			return res;
		}

		inline Quaternion conjugate() const { return Quaternion(-x, -y, -z, w); }
	};

	class Transform
	{
	private:
		// make sure these are correct, these are read from memory
		struct TransformAccess
		{
			uintptr_t hierarchyAddr;
			int index;
		};

		class trsX
		{
		public:
			Vector3 t;
		private:
			char pad_000C[4];
		public:
			Quaternion q;
			Vector3 s;
		private:
			char pad_0014[4];
		};
		template <typename T>
		class ResizableBuffer
		{
		public:
			T* buffer;
			int capacity;

			inline ResizableBuffer() : buffer(nullptr), capacity(100)
			{
				buffer = new T[capacity];
			}

			inline void updateBuffer(uintptr_t address, int capacity)
			{
				if (capacity > this->capacity)
				{
					if (buffer)
						delete[] buffer;

					buffer = new T[capacity];
					this->capacity = capacity;
				}

				Memory::readBuffer(address, (void*)buffer, sizeof(T) * capacity);
			}

			inline T& operator[](int index)
			{
				if (index >= capacity)
					printf("index out of bounds: forgot to call updateBuffer?\n");
				return buffer[index];
			}
		};

		// use the same buffer for all transforms. NOTE: if you're multithreaded, probably mutex these or store buffer another way
		static inline ResizableBuffer<trsX> trsBuffer;
		static inline ResizableBuffer<int> parentIndicesBuffer;

		uintptr_t address;

		TransformAccess transformAccess;
		uintptr_t localTransforms;
		uintptr_t parentIndices;

		// might be beneficial to call these yourself instead of in every function, if you happen to use multiple functions that call this?
		inline void updateTrsXBuffer()
		{
			// feel free to change how new max cap is calc'd. could be index * 2 or something to avoid reallocs
			trsBuffer.updateBuffer(localTransforms, transformAccess.index + 1);
		}

		inline void updateParentIndicesBuffer()
		{
			parentIndicesBuffer.updateBuffer(parentIndices, transformAccess.index + 1);
		}

	public:
		inline Transform(uintptr_t address)
		{
			this->address = address;
			this->transformAccess = Memory::read<TransformAccess>(address + 0x38);
			this->localTransforms = Memory::read<uintptr_t>(transformAccess.hierarchyAddr + 0x18);
			this->parentIndices = Memory::read<uintptr_t>(transformAccess.hierarchyAddr + 0x20);
		}

		// ------------------------------
		// actual methods
		// ------------------------------

		inline Vector3 localPosition()
		{
			return Memory::read<trsX>(localTransforms + transformAccess.index * sizeof(trsX)).t;
		}

		inline Vector3 localScale()
		{
			return Memory::read<trsX>(localTransforms + transformAccess.index * sizeof(trsX)).s;
		}

		inline Quaternion localRotation()
		{
			return Memory::read<trsX>(localTransforms + transformAccess.index * sizeof(trsX)).q;
		}

		inline Vector3 position()
		{
			updateTrsXBuffer();
			updateParentIndicesBuffer();

			Vector3 worldPos = trsBuffer[transformAccess.index].t;
			int index = parentIndicesBuffer[transformAccess.index];
			while (index >= 0)
			{
				auto parent = trsBuffer[index];

				worldPos = parent.q * worldPos;
				worldPos = worldPos * parent.s;
				worldPos = worldPos + parent.t;

				index = parentIndicesBuffer[index];
			}

			return worldPos;
		}

		inline Quaternion rotation()
		{
			updateTrsXBuffer();
			updateParentIndicesBuffer();

			Quaternion worldRot = trsBuffer[transformAccess.index].q;
			int index = parentIndicesBuffer[transformAccess.index];
			while (index >= 0)
			{
				auto parent = trsBuffer[index];

				worldRot = parent.q * worldRot;

				index = parentIndicesBuffer[index];
			}

			return worldRot;
		}

		inline Vector3 right()
		{
			static Vector3 right(1, 0, 0);
			return rotation() * right;
		}

		inline Vector3 up()
		{
			static Vector3 up(0, 1, 0);
			return rotation() * up;
		}

		inline Vector3 forward()
		{
			static Vector3 forward(0, 0, 1);
			return rotation() * forward;
		}

		// local to world
		inline Vector3 transformDirection(Vector3 localDirection)
		{
			return rotation() * localDirection;
		}

		// world to local
		inline Vector3 inverseTransformDirection(Vector3 worldDirection)
		{
			return rotation().conjugate() * worldDirection;
		}

		// local to world
		inline Vector3 transformPoint(Vector3 localPoint)
		{
			updateTrsXBuffer();
			updateParentIndicesBuffer();

			Vector3 worldPos = localPoint;
			int index = transformAccess.index;
			while (index >= 0)
			{
				auto parent = trsBuffer[index];

				worldPos = worldPos * parent.s;
				worldPos = parent.q * worldPos;
				worldPos = worldPos + parent.t;

				index = parentIndicesBuffer[index];
			}

			return worldPos;
		}

		// world to local
		inline Vector3 inverseTransformPoint(Vector3 worldPoint)
		{
			updateTrsXBuffer();
			updateParentIndicesBuffer();

			Vector3 worldPos = trsBuffer[transformAccess.index].t;
			Quaternion worldRot = trsBuffer[transformAccess.index].q;

			Vector3 localScale = trsBuffer[transformAccess.index].s;

			int index = parentIndicesBuffer[transformAccess.index];
			while (index >= 0)
			{
				auto parent = trsBuffer[index];

				worldPos = parent.q * worldPos;
				worldPos = worldPos * parent.s;
				worldPos = worldPos + parent.t;

				worldRot = parent.q * worldRot;

				index = parentIndicesBuffer[index];
			}

			Vector3 local = worldRot.conjugate() * (worldPoint - worldPos);
			return local / localScale;

			return worldPos;
		}
	};

	class GameObject
	{
	public:
		Transform transform() // credit: frankie-11
		{
			auto components = Memory::read<uintptr_t>(THISPTR + 0x30);
			return Transform(Memory::read<uintptr_t>(components + 0x8));
		}
	};
}

#endif // UNITY_HPP
