/*
 * File: nonCopyableClass.hpp
 * Author: Maxime MARIA
 */

#ifndef __NON_COPYABLE_CLASS__
#define __NON_COPYABLE_CLASS__

class NonCopyable
{
public:
	NonCopyable() = default;
	~NonCopyable() = default;

	NonCopyable(const NonCopyable &other) = delete;
	NonCopyable &operator=(const NonCopyable &other) = delete;
};

#endif // __NON_COPYABLE_CLASS__
