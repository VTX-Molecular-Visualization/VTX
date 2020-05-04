#ifndef __VTX_ACTION_CHAIN_CHANGE_VISIBILITY__
#define __VTX_ACTION_CHAIN_CHANGE_VISIBILITY__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/molecule.hpp"
#include "visible_change_visibility.hpp"

namespace VTX
{
	namespace Action
	{
		class ChainChangeVisibility : public VisibleChangeVisibility
		{
		  public:
			explicit ChainChangeVisibility( Model::Chain & p_chain, const VISIBILITY_MODE p_mode ) :
				VisibleChangeVisibility( p_chain, p_mode )
			{
			}

			virtual void execute() override
			{
				const Model::Chain & chain = ( (Model::Chain &)_visible );
				VisibleChangeVisibility::execute();

				if ( _mode == VISIBILITY_MODE::ALL || _mode == VISIBILITY_MODE::SOLO )
				{
					for ( Model::Chain * const c : chain.getMoleculePtr()->getChains() )
					{
						c->setVisible( _mode == VISIBILITY_MODE::ALL
									   || ( _mode == VISIBILITY_MODE::SOLO && c == &chain ) );
					}
				}

				chain.getMoleculePtr()->refreshVisibility();
			}
		};
	} // namespace Action
} // namespace VTX
#endif
