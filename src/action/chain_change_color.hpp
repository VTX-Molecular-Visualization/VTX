#ifndef __VTX_ACTION_CHAIN_CHANGE_COLOR__
#define __VTX_ACTION_CHAIN_CHANGE_COLOR__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/chain.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ChainChangeColor : public BaseAction
		{
		  public:
			explicit ChainChangeColor( Model::Chain & p_chain, const Color::Rgb & p_color ) :
				_chain( p_chain ), _color( p_color )
			{
			}

			virtual void execute() override
			{
				_chain.setColor( _color );
				_chain.getMoleculePtr()->setColorMode();
			}

		  private:
			Model::Chain &	 _chain;
			const Color::Rgb _color;
		};
	} // namespace Action
} // namespace VTX
#endif
