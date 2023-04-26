#ifndef __VTX_UI_QT_ACTION_ATOM__
#define __VTX_UI_QT_ACTION_ATOM__

#include <app/core/action/base_action.hpp>
#include <app/component/chemistry/atom.hpp>

namespace VTX::UI::QT::Action::Atom
{
	class Orient : public VTX::App::Core::Action::BaseAction
	{
	  public:
		explicit Orient( const App::Component::Chemistry::Atom & p_atom ) : _atom( p_atom ) {}
		virtual void execute() override;

	  private:
		const App::Component::Chemistry::Atom & _atom;
	};
} // namespace VTX::UI::QT::Action::Atom
#endif
