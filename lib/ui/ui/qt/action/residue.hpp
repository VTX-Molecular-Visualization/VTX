#ifndef __VTX_UI_QT_ACTION_RESIDUE__
#define __VTX_UI_QT_ACTION_RESIDUE__

#include <app/old_app/action/base_action.hpp>
#include <app/old_app/model/residue.hpp>

namespace VTX::UI::QT::Action::Residue
{
	class Orient : public VTX::Action::BaseAction
	{
	  public:
		explicit Orient( const Model::Residue & p_residue ) : _residue( p_residue ) {}
		virtual void execute() override;

	  private:
		const Model::Residue & _residue;
	};

} // namespace VTX::UI::QT::Action::Residue
#endif
