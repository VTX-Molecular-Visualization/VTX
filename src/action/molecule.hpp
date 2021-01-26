#ifndef __VTX_ACTION_MOLECULE__
#define __VTX_ACTION_MOLECULE__

#ifdef _MSC_VER
#pragma once
#endif

#include "event/event_manager.hpp"
#include "model/generated_molecule.hpp"
#include "model/molecule.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "selection/selection_manager.hpp"
#include "setting.hpp"
#include "state/state_machine.hpp"
#include "state/visualization.hpp"
#include "util/math.hpp"
#include "util/molecule.hpp"
#include "util/secondary_structure.hpp"
#include "visible.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		namespace Molecule
		{
			class ChangeColorMode : public BaseAction
			{
			  public:
				explicit ChangeColorMode( Model::Molecule & p_molecule, const Generic::COLOR_MODE p_colorMode ) :
					_molecule( p_molecule ), _colorMode( p_colorMode )
				{
				}

				virtual void execute() override { _molecule.setColorMode( _colorMode ); }

			  private:
				Model::Molecule &		  _molecule;
				const Generic::COLOR_MODE _colorMode;
			};

			class ChangeColor : public BaseAction
			{
			  public:
				explicit ChangeColor( Model::Molecule & p_molecule, const Color::Rgb & p_color ) :
					_molecule( p_molecule ), _color( p_color )
				{
				}

				virtual void execute() override
				{
					_molecule.setColor( _color );
					_molecule.refreshColors();

					if ( _molecule.getSecondaryStructure().getColorMode()
						 == Model::SecondaryStructure::COLOR_MODE::PROTEIN )
					{
						_molecule.getSecondaryStructure().refreshColors();
					}
				}

			  private:
				Model::Molecule & _molecule;
				const Color::Rgb  _color;
			};

			class ChangeVisibility : public Visible::ChangeVisibility
			{
			  public:
				explicit ChangeVisibility( Model::Molecule & p_molecule, const VISIBILITY_MODE p_mode ) :
					Visible::ChangeVisibility( p_molecule, p_mode )
				{
				}

				virtual void execute() override
				{
					for ( Generic::BaseVisible * const visible : _visibles )
					{
						const bool		  newVisibility = _getVisibilityBool( *visible );
						Model::Molecule * molecule		= ( (Model::Molecule *)visible );
						molecule->setVisible( newVisibility );
						molecule->computeRepresentationTargets();
					}
				}
			};

			class ChangeFPS : public BaseAction
			{
			  public:
				explicit ChangeFPS( Model::Molecule & p_molecule, const int p_fps ) :
					_molecule( p_molecule ), _fps( p_fps )
				{
				}

				virtual void execute() override
				{
					_molecule.setFPS( Util::Math::clamp( _fps, 0, int( VTX::Setting::VIDEO_FPS_DEFAULT ) ) );
				}

			  private:
				Model::Molecule & _molecule;
				const int		  _fps;
			};

			class ChangeFrame : public BaseAction
			{
			  public:
				explicit ChangeFrame( Model::Molecule & p_molecule, const int p_frame ) :
					_molecule( p_molecule ), _frame( p_frame )
				{
				}

				virtual void execute() override
				{
					_molecule.setFrame( Util::Math::clamp( _frame, 0, (int)_molecule.getFrameCount() - 1 ) );
				}

			  private:
				Model::Molecule & _molecule;
				const int		  _frame;
			};

			class ChangeIsPlaying : public BaseAction
			{
			  public:
				explicit ChangeIsPlaying( Model::Molecule & p_molecule, const bool p_isPlaying ) :
					_molecule( p_molecule ), _isPlaying( p_isPlaying )
				{
				}

				virtual void execute() override { _molecule.setIsPlaying( _isPlaying ); }

			  private:
				Model::Molecule & _molecule;
				const bool		  _isPlaying;
			};

			class ChangeShowIon : public BaseAction
			{
			  public:
				explicit ChangeShowIon( Model::Molecule & p_molecule, const bool p_showIon ) :
					_molecule( p_molecule ), _showIon( p_showIon )
				{
				}

				virtual void execute() override { _molecule.setShowIon( _showIon ); }

			  private:
				Model::Molecule & _molecule;
				const bool		  _showIon;
			};

			class ChangeShowSolvent : public BaseAction
			{
			  public:
				explicit ChangeShowSolvent( Model::Molecule & p_molecule, const bool p_showSolvent ) :
					_molecule( p_molecule ), _showSolvent( p_showSolvent )
				{
				}

				virtual void execute() override { _molecule.setShowSolvent( _showSolvent ); }

			  private:
				Model::Molecule & _molecule;
				const bool		  _showSolvent;
			};

			class ComputeSecondaryStructure : public BaseAction
			{
			  public:
				explicit ComputeSecondaryStructure( Model::Molecule & p_molecule ) : _molecule( p_molecule ) {}

				virtual void execute() override
				{
					Util::SecondaryStructure::computeSecondaryStructure( _molecule );
					_molecule.createSecondaryStructure();
				}

			  private:
				Model::Molecule & _molecule;
			};

			class Delete : public BaseAction
			{
			  public:
				explicit Delete( Model::Molecule & p_molecule ) : _molecule( p_molecule ) {}

				virtual void execute() override
				{
					VTXApp::get().getScene().removeMolecule( &_molecule );
					MVC::MvcManager::get().deleteModel( &_molecule );
				}

			  private:
				Model::Molecule & _molecule;
			};

			class ChangeColorModeSecondaryStructure : public BaseAction
			{
			  public:
				explicit ChangeColorModeSecondaryStructure( Model::Molecule &							p_molecule,
															const Model::SecondaryStructure::COLOR_MODE p_colorMode ) :
					_molecule( p_molecule ),
					_colorMode( p_colorMode )
				{
				}

				virtual void execute() override { _molecule.getSecondaryStructure().setColorMode( _colorMode ); }

			  private:
				Model::Molecule &							_molecule;
				const Model::SecondaryStructure::COLOR_MODE _colorMode;
			};

			class Orient : public BaseAction
			{
			  public:
				explicit Orient( Model::Molecule & p_molecule ) : _molecule( p_molecule ) {}

				virtual void execute() override
				{
					VTXApp::get()
						.getStateMachine()
						.getItem<State::Visualization>( ID::State::VISUALIZATION )
						->orientCameraController( _molecule.getAABB() );
				}

			  private:
				Model::Molecule & _molecule;
			};

			class Copy : public BaseAction
			{
			  public:
				explicit Copy( const Model::Selection & p_source ) : _selection( p_source ) {}
				virtual void execute() override
				{
					Model::GeneratedMolecule * generatedMolecule
						= MVC::MvcManager::get().instantiateModel<Model::GeneratedMolecule>();
					generatedMolecule->copyFromSelection( _selection );
					VTX_EVENT(
						new Event::VTXEventPtr<Model::Molecule>( Event::Global::MOLECULE_CREATED, generatedMolecule ) );

					generatedMolecule->setTranslation( VTX::Vec3f( 10, 0, 0 ) );
					VTXApp::get().getScene().addMolecule( generatedMolecule );
				}

			  private:
				const Model::Selection & _selection;
			};

			class Extract : public BaseAction
			{
			  public:
				explicit Extract( const Model::Selection & p_source ) : _selection( p_source ) {}
				virtual void execute() override
				{
					const Model::ID & idMolSource = _selection.getItems().begin()->first;
					Model::Molecule & molecule	  = MVC::MvcManager::get().getModel<Model::Molecule>( idMolSource );

					Model::GeneratedMolecule * const generatedMolecule
						= MVC::MvcManager::get().instantiateModel<Model::GeneratedMolecule>();
					generatedMolecule->extractFromSelection( _selection );

					VTX_EVENT(
						new Event::VTXEventPtr<Model::Molecule>( Event::Global::MOLECULE_CREATED, generatedMolecule ) );

					VTXApp::get().getScene().addMolecule( generatedMolecule );
				}

			  private:
				const Model::Selection & _selection;
			};
		} // namespace Molecule
	}	  // namespace Action
} // namespace VTX
#endif
