#ifndef __VTX_UI_QT_ACTION_RESIDUE__
#define __VTX_UI_QT_ACTION_RESIDUE__

#include <app/old/core/action/base_action.hpp>
#include <app/old/component/chemistry/residue.hpp>

namespace VTX::UI::QT::Action::Residue
{
	class Orient : public VTX::App::Old::Core::Action::BaseAction
	{
	  public:
		explicit Orient( const App::Old::Component::Chemistry::Residue & p_residue ) : _residue( p_residue ) {}
		virtual void execute() override;

	  private:
		const App::Old::Component::Chemistry::Residue & _residue;
	};

} // namespace VTX::UI::QT::Action::Residue
#endif
