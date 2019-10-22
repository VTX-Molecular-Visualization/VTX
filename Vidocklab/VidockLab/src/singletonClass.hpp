/*
 * File: nonCopyableClass.hpp
 * Author: Maxime MARIA
 */

#ifndef __SINGLETON_CLASS_HPP__
#define __SINGLETON_CLASS_HPP__

#include "exceptions.hpp"
#include "nonCopyableClass.hpp"

namespace vidocklab
{
	template<typename T>
	class Singleton : public NonCopyable
	{
	public:
		static T *getPtr()
		{
			return m_instance;
		}
		static T &get()
		{
			return *m_instance;
		}

	protected:
		Singleton()
		{
			if (m_instance != nullptr)
				throw SingletonException("Singleton already instanced");
			m_instance = (T *)(this);
		}

		~Singleton()
		{
			m_instance = nullptr;
		}

		static T *m_instance;
	};
} // namespace vidocklab

#endif // __SINGLETON_CLASS_HPP__
