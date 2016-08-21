#ifndef _DUST_GENERIC_HASH_
#define _DUST_GENERIC_HASH_

#include <cryptopp/sha.h>
#include <cryptopp/tiger.h>
#include <cryptopp/whrlpool.h>
#include <cryptopp/ripemd.h>
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <cryptopp/md5.h>

//using H = CryptoPP::SHA1;
//using H = CryptoPP::Tiger;
//using H = CryptoPP::Whirlpool;
//using H = CryptoPP::RIPEMD128;
//using H = CryptoPP::Weak::MD5;

namespace dust
{
	template <typename H> struct generic_hash
	{
		static const unsigned long int size = H::DIGESTSIZE;

		byte value[size];

		template <typename T> generic_hash(const T& v)
		{
			H hasher;
			hasher.CalculateDigest(value, (const byte*)&v, sizeof(T));
		}

		void rehash()
		{
			H hasher;
			byte buff[size];
			memcpy(buff, value, size);
			hasher.CalculateDigest(value, buff, size);
		}

		const bool operator==(const generic_hash<H>& other)
		{
			return memcmp(value, other.value, size);
		}

		const bool operator==(const generic_hash<H>& other) const
		{
			return memcmp(value, other.value, size);
		}

		const bool operator<(const generic_hash<H>& other)
		{
			return memcmp(value, other.value, size) < 0;
		}

		const bool operator<(const generic_hash<H>& other) const
		{
			return memcmp(value, other.value, size) < 0;
		}

		std::string str()
		{
			unsigned char* p1 = (unsigned char*) value;
			std::string s;
			for(unsigned int i = 0; i < size/3; ++i)
			{
				s += std::to_string((int)p1[i]);
				s += " ";
			}
			return s;
		}
	};

	typedef generic_hash<CryptoPP::Tiger> hash;

}

#endif // _DUST_GENERIC_HASH_
