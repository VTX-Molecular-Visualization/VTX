#ifndef __VTX_UI_QT_INPUT_SHORTCUT__
#define __VTX_UI_QT_INPUT_SHORTCUT__

#include "ui/qt/input/key_sequence.hpp"
#include <app/application/action/action_manager.hpp>
#include <app/vtx_app.hpp>
#include <functional>

namespace VTX::UI::QT::Controller
{
	class Shortcut
	{
	  public:
		using ActionFunction = std::function<void()>;

		template<typename A, typename... Args>
		static ActionFunction callAction( const Args &... p_args )
		{
			return [ p_args... ]() { App::VTX_ACTION().execute<A>( p_args... ); };
		}

	  public:
		Shortcut( const Input::KeySequence & p_sequence, const ActionFunction & p_action ) :
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

		const Input::KeySequence & getSequence() const { return _sequence; }
		void					   trigger() const { _action(); }

	  private:
		Input::KeySequence _sequence;
		ActionFunction	   _action;
	};
} // namespace VTX::UI::QT::Controller
#endif
