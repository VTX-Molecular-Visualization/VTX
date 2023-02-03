#ifndef __VTX_UI_QT_ACTION_ATOM__
#define __VTX_UI_QT_ACTION_ATOM__

#include "src/action/base_action.hpp"
#include "src/model/atom.hpp"

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
