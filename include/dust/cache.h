#ifndef _DUST_CACHE_
#define _DUST_CACHE_

#include "generic_hash.h"

#include <map>
#include <exception>
#include <iostream>
#include <sstream>

namespace dust
{
	template <typename Key = int>
	class cache
	{
		struct obj_too_big_exception : public std::exception
		{
			std::string whatstr;

			obj_too_big_exception(std::string whatstr) : whatstr(whatstr)
			{
			}

			const char* what() const noexcept
			{
				return whatstr.c_str();
			}
		};

		class ptr_store
		{
			void* obj;
			size_t obj_size;
			void (*destructor)(void*);

			public:

			template <typename T> ptr_store(T* obj) :
				obj(static_cast<void*>(obj)),
				obj_size(sizeof(T))
			{

				//std::cout << "constructor " << obj << std::endl;

				destructor = [](void* o) -> void
					{
						delete static_cast<T*>(o);
					};
			}

			void* ptr()
			{
				return obj;
			}

			size_t size()
			{
				return obj_size;
			}

			~ptr_store()
			{
				//std::cout << "destructor" << obj << std::endl;
				destructor(obj);
			}
		};

		size_t size;
		size_t used;
		std::map<Key, ptr_store> objects;

		public:

		cache(size_t size) :
			size(size),
			used(0)
		{
		}

		template <typename T> void put(Key k, T* v)
		{
			bool deleted = false;
			auto it = objects.find(k);

			if(it != objects.end())
			{
				objects.erase(it);
				deleted = true;
			}
			
			if(!deleted)
			{
				if(sizeof(T) > size)
				{
					std::stringstream ss;
					ss << "Attempted to cache object of size " << sizeof(T) << " in a cache of size " << size;
					throw obj_too_big_exception(ss.str());
				}

				while(used + sizeof(T) > size)
				{
					std::cout << "used " << used << " size " << size << " sizeof " << sizeof(T) << "\n";
					auto it = objects.begin();
					used -= it->second.size();
					objects.erase(it);
				}

				used += sizeof(T);
			}

			objects.emplace(k, v);
		}

		template <typename T> bool get(Key k, T* v)
		{
			auto it = objects.find(k);
			if(it == objects.end())
				return false;

			*v = static_cast<T*>(it->second.ptr());
			return true;
		}
	};
}

#endif // _DUST_CACHE_
