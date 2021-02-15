#ifndef __VTX_ACTION_CHAIN__
#define __VTX_ACTION_CHAIN__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "state/state_machine.hpp"
#include "state/visualization.hpp"
#include "util/molecule.hpp"
#include "visible.hpp"

namespace VTX
{
	namespace Action
	{
		namespace Chain
		{
			class ChangeColor : public BaseAction
			{
			  public:
				explicit ChangeColor( Model::Chain & p_chain, const Color::Rgb & p_color ) :
					_chain( p_chain ), _color( p_color )
				{
				}

				virtual void execute() override
				{
					_chain.setColor( _color );
					_chain.getMoleculePtr()->refreshColors();

					if ( _chain.getMoleculePtr()->getSecondaryStructure().getColorMode()
						 == Model::SecondaryStructure::COLOR_MODE::CHAIN )
					{
						_chain.getMoleculePtr()->getSecondaryStructure().refreshColors();
					}

					VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
				}

			  private:
				Model::Chain &	 _chain;
				const Color::Rgb _color;
			};

			class ChangeVisibility : public Visible::ChangeVisibility
			{
			  public:
				explicit ChangeVisibility( Model::Chain & p_chain, const VISIBILITY_MODE p_mode ) :
					Visible::ChangeVisibility( p_chain, p_mode )
				{
				}

				virtual void execute() override
				{
					for ( Generic::BaseVisible * const visible : _visibles )
					{
						bool		   newVisibility = _getVisibilityBool( *visible );
						Model::Chain & chain		 = *( (Model::Chain *)visible );

						chain.setVisible( newVisibility );

						if ( _mode == VISIBILITY_MODE::ALL || _mode == VISIBILITY_MODE::SOLO )
						{
							for ( Model::Chain * const c : chain.getMoleculePtr()->getChains() )
							{
								if ( c == nullptr )
									continue;

								c->setVisible( _mode == VISIBILITY_MODE::ALL
											   || ( _mode == VISIBILITY_MODE::SOLO && c == &chain ) );
							}
						}

						chain.getMoleculePtr()->computeRepresentationTargets();
					}

					VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
				}
			};

			class Orient : public BaseAction
			{
			  public:
				explicit Orient( Model::Chain & p_chain ) : _chain( p_chain ) {}

				virtual void execute() override
				{
					VTXApp::get()
						.getStateMachine()
						.getItem<State::Visualization>( ID::State::VISUALIZATION )
						->orientCameraController( _chain.getWorldAABB() );
				}

			  private:
				Model::Chain & _chain;
			};
		} // namespace Chain
	}	  // namespace Action
} // namespace VTX
#endif
