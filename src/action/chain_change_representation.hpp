#ifndef __VTX_ACTION_CHAIN_CHANGE_REPRESENTATION__
#define __VTX_ACTION_CHAIN_CHANGE_REPRESENTATION__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/chain.hpp"
#include "util/molecule.hpp"

namespace VTX
{
	namespace Action
	{
		class ChainChangeRepresentation : public BaseAction
		{
		  public:
			explicit ChainChangeRepresentation( Model::Chain &				  p_chain,
												const Generic::REPRESENTATION p_representation ) :
				_chain( p_chain ),
				_representation( p_representation )
			{
			}

			virtual void execute() override { _chain.setRepresentation( _representation ); };

		  private:
			Model::Chain &				  _chain;
			const Generic::REPRESENTATION _representation;
		};
	} // namespace Action
} // namespace VTX
#endif
