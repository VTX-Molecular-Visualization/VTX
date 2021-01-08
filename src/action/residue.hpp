#ifndef __VTX_ACTION_RESIDUE__
#define __VTX_ACTION_RESIDUE__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/residue.hpp"
#include "util/molecule.hpp"
#include "visible.hpp"

namespace VTX
{
	namespace Action
	{
		namespace Residue
		{
			class ChangeColor : public BaseAction
			{
			  public:
				explicit ChangeColor( Model::Residue & p_residue, const Color::Rgb & p_color ) : _residue( p_residue ), _color( p_color ) {}

				virtual void execute() override
				{
					_residue.setColor( _color );
					_residue.getMoleculePtr()->refreshColors();

					if ( _residue.getMoleculePtr()->getSecondaryStructure().getColorMode() == Model::SecondaryStructure::COLOR_MODE::RESIDUE )
					{
						_residue.getMoleculePtr()->getSecondaryStructure().refreshColors();
					}
				}

			  private:
				Model::Residue & _residue;
				const Color::Rgb _color;
			};

			class ChangeVisibility : public Visible::ChangeVisibility
			{
			  public:
				explicit ChangeVisibility( Model::Residue & p_residue, const VISIBILITY_MODE p_mode ) : Visible::ChangeVisibility( p_residue, p_mode ) {}

				virtual void execute() override
				{
					for ( Generic::BaseVisible * const visible : _visibles )
					{
						bool			 newVisibility = _getVisibilityBool( *visible );
						Model::Residue & residue	   = *( (Model::Residue *)visible );

						residue.setVisible( newVisibility );

						if ( _mode == VISIBILITY_MODE::ALL || _mode == VISIBILITY_MODE::SOLO )
						{
							for ( uint i = 0; i < residue.getChainPtr()->getResidueCount(); ++i )
							{
								residue.getMoleculePtr()
									->getResidue( residue.getChainPtr()->getIndexFirstResidue() + i )
									.setVisible( _mode == VISIBILITY_MODE::ALL
												 || ( _mode == VISIBILITY_MODE::SOLO && residue.getChainPtr()->getIndexFirstResidue() + i == residue.getIndex() ) );
							}
						}

						residue.getMoleculePtr()->computeRepresentationTargets();
					}
				}
			};
		} // namespace Residue
	}	  // namespace Action
} // namespace VTX
#endif
