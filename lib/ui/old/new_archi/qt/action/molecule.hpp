#ifndef __VTX_UI_QT_ACTION_MOLECULE__
#define __VTX_UI_QT_ACTION_MOLECULE__

#include <app/old/core/action/base_action.hpp>
#include <app/old/component/chemistry/molecule.hpp>

namespace VTX::UI::QT::Action::Molecule
{
	class Orient : public VTX::App::Old::Core::Action::BaseAction
	{
	  public:
		explicit Orient( const App::Old::Component::Chemistry::Molecule & p_molecule ) : _molecule( p_molecule ) {}
		virtual void execute() override;

	  private:
		const App::Old::Component::Chemistry::Molecule & _molecule;
	};
} // namespace VTX::UI::QT::Action::Molecule
#endif
