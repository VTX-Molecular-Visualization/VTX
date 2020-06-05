#ifndef __VTX_ACTION_RESIDUE_CHANGE_VISIBILITY__
#define __VTX_ACTION_RESIDUE_CHANGE_VISIBILITY__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/molecule.hpp"
#include "visible_change_visibility.hpp"

namespace VTX
{
	namespace Action
	{
		class ResidueChangeVisibility : public VisibleChangeVisibility
		{
		  public:
			explicit ResidueChangeVisibility( Model::Residue & p_residue, const VISIBILITY_MODE p_mode ) :
				VisibleChangeVisibility( p_residue, p_mode )
			{
			}

			virtual void execute() override
			{
				const Model::Residue & residue = ( (Model::Residue &)_visible );
				VisibleChangeVisibility::execute();

				if ( _mode == VISIBILITY_MODE::ALL || _mode == VISIBILITY_MODE::SOLO )
				{
					for ( uint i = 0; i < residue.getChainPtr()->getResidueCount(); ++i )
					{
						residue.getMoleculePtr()
							->getResidue( residue.getChainPtr()->getIndexFirstResidue() + i )
							.setVisible(
								_mode == VISIBILITY_MODE::ALL
								|| ( _mode == VISIBILITY_MODE::SOLO
									 && residue.getChainPtr()->getIndexFirstResidue() + i == residue.getIndex() ) );
					}
				}

				residue.getMoleculePtr()->refreshVisibility();
			}
		};
	} // namespace Action
} // namespace VTX
#endif
