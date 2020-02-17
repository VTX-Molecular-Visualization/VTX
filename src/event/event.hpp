#ifndef __VTX_EVENT__
#define __VTX_EVENT__

#ifdef _MSC_VER
#pragma once
#endif

#include <string>

namespace VTX
{
	namespace Event
	{
		const std::string LOG_CONSOLE = "LOG_CONSOLE";

		enum class EVENT_MODEL
		{
			RENDER,
			CHANGE_REPRESENTATION
		};
		/*
		template<typename T>
		class Event
		{
		  public:
			Event() : _data( NULL ) {};
			Event( T & p_arg ) : _data( p_arg ) {};
			T data() { return _data; }

		  private:
			T _data;
		};
		*/
	} // namespace Event
} // namespace VTX
#endif
