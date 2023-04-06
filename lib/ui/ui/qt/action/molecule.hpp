#ifndef __VTX_UI_QT_ACTION_MOLECULE__
#define __VTX_UI_QT_ACTION_MOLECULE__

#include <old/action/base_action.hpp>
#include <old/model/molecule.hpp>

namespace VTX::UI::QT::Action::Molecule
{
	class Orient : public VTX::Action::BaseAction
	{
	  public:
		explicit Orient( const Model::Molecule & p_molecule ) : _molecule( p_molecule ) {}
		virtual void execute() override;

	  private:
		const Model::Molecule & _molecule;
	};
} // namespace VTX::UI::QT::Action::Molecule
#endif
