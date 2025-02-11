#ifndef __VTX_APP_CORE_INPUT_SHORTCUT__
#define __VTX_APP_CORE_INPUT_SHORTCUT__

#include "key_sequence.hpp"
#include <app/core/action/action_system.hpp>
#include <functional>

namespace VTX::App::Core::Input
{
	class Shortcut
	{
	  public:
		using ActionFunction = std::function<void()>;

		template<typename A, typename... Args>
		static ActionFunction callAction( const Args &... p_args )
		{
			return [ p_args... ]() { App::ACTION_SYSTEM().execute<A>( p_args... ); };
		}

	  public:
		Shortcut( const KeySequence & p_sequence, const ActionFunction & p_action ) :
			_sequence( p_sequence ), _action( p_action )
		{
		}

		friend bool operator==( const Shortcut & p_lhs, const Shortcut & p_rhs )
		{
			return p_lhs._sequence == p_rhs._sequence;
		}
		friend std::partial_ordering operator<=>( const Shortcut & p_lhs, const Shortcut & p_rhs )
		{
			return p_lhs._sequence <=> p_rhs._sequence;
		}

		const KeySequence & getSequence() const { return _sequence; }
		void				trigger() const { _action(); }

	  private:
		KeySequence	   _sequence;
		ActionFunction _action;
	};
} // namespace VTX::App::Core::Input
#endif
