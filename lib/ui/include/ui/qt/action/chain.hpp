#ifndef __VTX_UI_QT_ACTION_CHAIN__
#define __VTX_UI_QT_ACTION_CHAIN__

#include <app/old_app/action/base_action.hpp>
#include <app/old_app/model/chain.hpp>

namespace VTX::UI::QT::Action::Chain
{

	class Orient : public VTX::Action::BaseAction
	{
	  public:
		explicit Orient( const Model::Chain & p_chain ) : _chain( p_chain ) {}
		virtual void execute() override;

	  private:
		const Model::Chain & _chain;
	};

} // namespace VTX::UI::QT::Action::Chain
#endif