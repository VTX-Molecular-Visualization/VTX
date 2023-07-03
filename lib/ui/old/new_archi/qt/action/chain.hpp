#ifndef __VTX_UI_QT_ACTION_CHAIN__
#define __VTX_UI_QT_ACTION_CHAIN__

#include <app/old/core/action/base_action.hpp>
#include <app/old/component/chemistry/chain.hpp>

namespace VTX::UI::QT::Action::Chain
{

	class Orient : public VTX::App::Old::Core::Action::BaseAction
	{
	  public:
		explicit Orient( const App::Old::Component::Chemistry::Chain & p_chain ) : _chain( p_chain ) {}
		virtual void execute() override;

	  private:
		const App::Old::Component::Chemistry::Chain & _chain;
	};

} // namespace VTX::UI::QT::Action::Chain
#endif
