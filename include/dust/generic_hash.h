#ifndef _DUST_GENERIC_HASH_
#define _DUST_GENERIC_HASH_

#include <cryptopp/sha.h>
#include <cryptopp/tiger.h>
#include <cryptopp/whrlpool.h>
#include <cryptopp/ripemd.h>
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <cryptopp/md5.h>

#include <iostream>
#include <cstring>

//using H = CryptoPP::SHA1;
//using H = CryptoPP::Tiger;
//using H = CryptoPP::Whirlpool;
//using H = CryptoPP::RIPEMD128;
//using H = CryptoPP::Weak::MD5;

namespace dust
{
	template <typename H> class generic_hash
	{
		static const unsigned long int size = H::DIGESTSIZE;

		byte value[size];

		static void stream(H& h, const char* v)
		{
			h.Update(reinterpret_cast<const byte*>(v), std::strlen(v));
		}

		static void stream(H&h, const std::string& v)
		{
			h.Update(reinterpret_cast<const byte*>(v.c_str()), v.size());
		}

		template <typename T> static void stream(H& h, const T& v)
		{
			h.Update(reinterpret_cast<const byte*>(&v), sizeof(T));
		}

		public:

		template <typename ... T> generic_hash(const T&... args)
		{
			H h;
			(stream(h, args), ...);
			h.Final(value);
		}

		void rehash()
		{
			H hasher;
			byte buff[size];
			memcpy(buff, value, size);
			hasher.CalculateDigest(value, buff, size);
		}

		bool operator==(const generic_hash<H>& other) const
		{
			return memcmp(value, other.value, size) == 0;
		}

		bool operator==(const generic_hash<H>& other)
		{
			return memcmp(value, other.value, size) == 0;
		}

		bool operator!=(const generic_hash<H>& other) const
		{
			return !(*this == other);
		}

		bool operator!=(const generic_hash<H>& other)
		{
			return !(*this == other); 
		}
		bool operator<(const generic_hash<H>& other)
		{
			return memcmp(value, other.value, size) < 0;
		}

		bool operator<(const generic_hash<H>& other) const
		{
			return memcmp(value, other.value, size) < 0;
		}

		std::string str() const
		{
			const unsigned char* p1 = reinterpret_cast<const unsigned char*>(value);
			std::string s;
			for(unsigned int i = 0; i < size/8; ++i)
			{
				if(i != 0)
					s += " ";
				s += std::to_string(static_cast<int>(p1[i]));
			}
			return s;
		}
	};

	typedef generic_hash<CryptoPP::Tiger> hash;

}

#endif // _DUST_GENERIC_HASH_
