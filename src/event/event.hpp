#ifndef __VTX_EVENT__
#define __VTX_EVENT__

#ifdef _MSC_VER
#pragma once
#endif

namespace VTX
{
	namespace Event
	{
		enum class EVENT_UI
		{
			LOG_CONSOLE
		};

		enum class EVENT_MODEL
		{
			DRAW,
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
