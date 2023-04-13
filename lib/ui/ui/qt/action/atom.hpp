#ifndef __VTX_UI_QT_ACTION_ATOM__
#define __VTX_UI_QT_ACTION_ATOM__

#include <app/old_app/action/base_action.hpp>
#include <app/old_app/model/atom.hpp>

namespace VTX::UI::QT::Action::Atom
{
	class Orient : public VTX::Action::BaseAction
	{
	  public:
		explicit Orient( const Model::Atom & p_atom ) : _atom( p_atom ) {}
		virtual void execute() override;

	  private:
		const Model::Atom & _atom;
	};
} // namespace VTX::UI::QT::Action::Atom
#endif
