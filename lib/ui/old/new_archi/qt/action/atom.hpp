#ifndef __VTX_UI_QT_ACTION_ATOM__
#define __VTX_UI_QT_ACTION_ATOM__

#include <app/old/core/action/base_action.hpp>
#include <app/old/component/chemistry/atom.hpp>

namespace VTX::UI::QT::Action::Atom
{
	class Orient : public VTX::App::Old::Core::Action::BaseAction
	{
	  public:
		explicit Orient( const App::Old::Component::Chemistry::Atom & p_atom ) : _atom( p_atom ) {}
		virtual void execute() override;

	  private:
		const App::Old::Component::Chemistry::Atom & _atom;
	};
} // namespace VTX::UI::QT::Action::Atom
#endif
