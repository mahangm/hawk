#ifndef HAWK_HELPERS_MEMORY_H
#define HAWK_HELPERS_MEMORY_H

// stl
#include <cstring>

namespace hawk
{
	namespace Helpers
	{
		namespace Memory
		{
			/**
			 * Fill destination memory with zeroes.
			 * @param dst Memory address.
			 * @param size Memory size.
			 */
			static void zeroize(void* dst, size_t size)
			{
				std::memset(dst, 0, size);
			}

			/**
			 * Copy source memory to destination memory.
			 * @param src Source memory address.
			 * @param dst Destination memory address.
			 * @param size Destination memory size.
			 */
			static void copy(void* src, void* dst, size_t size)
			{
				std::memcpy(dst, src, size);
			}
		}
	}
}
#endif