#ifndef __VTX_UI_QT_ACTION_MOLECULE__
#define __VTX_UI_QT_ACTION_MOLECULE__

#include <app/core/action/base_action.hpp>
#include <app/component/chemistry/molecule.hpp>

namespace VTX::UI::QT::Action::Molecule
{
	class Orient : public VTX::App::Core::Action::BaseAction
	{
	  public:
		explicit Orient( const App::Component::Chemistry::Molecule & p_molecule ) : _molecule( p_molecule ) {}
		virtual void execute() override;

	  private:
		const App::Component::Chemistry::Molecule & _molecule;
	};
} // namespace VTX::UI::QT::Action::Molecule
#endif
