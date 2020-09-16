#ifndef __VTX_ACTION_RESIDUE__
#define __VTX_ACTION_RESIDUE__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/residue.hpp"
#include "util/molecule.hpp"
#include "visible.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		namespace Residue
		{
			class ChangeColor : public BaseAction
			{
			  public:
				explicit ChangeColor( Model::Residue & p_residue, const Color::Rgb & p_color ) :
					_residue( p_residue ), _color( p_color )
				{
				}

				virtual void execute() override
				{
					_residue.setColor( _color );
					_residue.getMoleculePtr()->refreshColors();

					if ( _residue.getMoleculePtr()->getSecondaryStructure().getColorMode()
						 == Model::SecondaryStructure::COLOR_MODE::RESIDUE )
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
				explicit ChangeVisibility( Model::Residue & p_residue, const VISIBILITY_MODE p_mode ) :
					Visible::ChangeVisibility( p_residue, p_mode )
				{
				}

				virtual void execute() override
				{
					const Model::Residue & residue = ( (Model::Residue &)_visible );
					Visible::ChangeVisibility::execute();

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

					Util::Molecule::refreshRepresentationState( *residue.getMoleculePtr() );
				}
			};
		} // namespace Residue
	}	  // namespace Action
} // namespace VTX
#endif
