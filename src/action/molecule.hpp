#ifndef __VTX_ACTION_MOLECULE__
#define __VTX_ACTION_MOLECULE__

#ifdef _MSC_VER
#pragma once
#endif

#include "event/event_manager.hpp"
#include "generic/base_colorable.hpp"
#include "model/generated_molecule.hpp"
#include "model/molecule.hpp"
#include "model/representation/instantiated_representation.hpp"
#include "model/representation/representation.hpp"
#include "model/representation/representation_library.hpp"
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
#include <unordered_set>

namespace VTX::Action::Molecule
{
	class ChangeColor : public BaseAction
	{
	  public:
		explicit ChangeColor( Model::Molecule & p_molecule, const Color::Rgb & p_color ) : _color( p_color )
		{
			_molecules.emplace( &p_molecule );
		}
		explicit ChangeColor( const std::unordered_set<Model::Molecule *> & p_molecules, const Color::Rgb & p_color ) :
			_color( p_color )
		{
			for ( Model::Molecule * const molecule : p_molecules )
				_molecules.emplace( molecule );
		}

		virtual void execute() override
		{
			for ( Model::Molecule * const molecule : _molecules )
			{
				molecule->setColor( _color );
				molecule->refreshColors();
				molecule->getSecondaryStructure().refreshColors();
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		std::unordered_set<Model::Molecule *> _molecules = std::unordered_set<Model::Molecule *>();
		const Color::Rgb					  _color;
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

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}
	};

	class ChangeRepresentationPreset : public BaseAction
	{
	  public:
		explicit ChangeRepresentationPreset( Model::Molecule & p_molecule, const int p_indexPreset ) :
			_indexPreset( p_indexPreset )
		{
			_molecules.emplace( &p_molecule );
		}
		explicit ChangeRepresentationPreset( const std::unordered_set<Model::Molecule *> & p_molecules,
											 const int									   p_indexPreset ) :
			_indexPreset( p_indexPreset )
		{
			for ( Model::Molecule * const molecule : p_molecules )
				_molecules.emplace( molecule );
		}

		virtual void execute() override
		{
			Model::Representation::BaseRepresentation * const preset
				= Model::Representation::RepresentationLibrary::get().getRepresentation( _indexPreset );

			for ( Model::Molecule * const molecule : _molecules )
			{
				Model::Representation::InstantiatedRepresentation * const instantiatedRepresentation
					= MVC::MvcManager::get().instantiateModel<Model::Representation::InstantiatedRepresentation>(
						preset );

				molecule->applyRepresentation( instantiatedRepresentation );
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		std::unordered_set<Model::Molecule *> _molecules;
		const int							  _indexPreset;
	};

	class RemoveRepresentation : public BaseAction
	{
	  public:
		explicit RemoveRepresentation( Model::Molecule & p_chain ) { _molecules.emplace( &p_chain ); }
		explicit RemoveRepresentation( const std::unordered_set<Model::Molecule *> & p_chains )
		{
			for ( Model::Molecule * const molecule : p_chains )
				_molecules.emplace( molecule );
		}

		virtual void execute() override
		{
			for ( Model::Molecule * const molecule : _molecules )
			{
				molecule->removeRepresentation();
				molecule->computeAllRepresentationData();
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		std::unordered_set<Model::Molecule *> _molecules = std::unordered_set<Model::Molecule *>();
	};
	class ChangeFPS : public BaseAction
	{
	  public:
		explicit ChangeFPS( Model::Molecule & p_molecule, const int p_fps ) : _molecule( p_molecule ), _fps( p_fps ) {}

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
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
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

		virtual void execute() override
		{
			_molecule.setShowIon( _showIon );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

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

		virtual void execute() override
		{
			_molecule.setShowSolvent( _showSolvent );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

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
		explicit ChangeColorModeSecondaryStructure( Model::Molecule &								p_molecule,
													const Generic::SECONDARY_STRUCTURE_COLOR_MODE & p_colorMode ) :
			_molecule( p_molecule ),
			_colorMode( p_colorMode )
		{
		}

		virtual void execute() override
		{
			_molecule.getSecondaryStructure().setColorMode( _colorMode );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		Model::Molecule &							  _molecule;
		const Generic::SECONDARY_STRUCTURE_COLOR_MODE _colorMode;
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
				->orientCameraController( _molecule.getWorldAABB() );
		}

	  private:
		Model::Molecule & _molecule;
	};

	class Copy : public BaseAction
	{
	  public:
		explicit Copy( const Model::Molecule & p_target ) : _target( p_target ) {}
		virtual void execute() override
		{
			Model::GeneratedMolecule * generatedMolecule
				= MVC::MvcManager::get().instantiateModel<Model::GeneratedMolecule>();

			generatedMolecule->copyFromMolecule( _target );

			VTX_EVENT( new Event::VTXEventPtr<Model::Molecule>( Event::Global::MOLECULE_CREATED, generatedMolecule ) );

			const float offset = generatedMolecule->getAABB().radius() + _target.getAABB().radius()
								 + VTX::Setting::COPIED_MOLECULE_OFFSET;
			generatedMolecule->setTranslation( VTX::Vec3f( offset, 0, 0 ) );

			VTXApp::get().getScene().addMolecule( generatedMolecule );
		}

	  private:
		const Model::Molecule & _target;
	};

	class Rename : public BaseActionUndonable
	{
	  public:
		explicit Rename( Model::Molecule & p_target, const std::string & p_newName ) :
			_target( p_target ), _oldName( p_target.getDisplayName() ), _newName( p_newName )
		{
		}

		void execute() override { _target.setDisplayName( _newName ); }
		void undo() override { _target.setDisplayName( _oldName ); }

	  private:
		Model::Molecule & _target;

		const std::string _oldName;
		const std::string _newName;
	};

	class ApplyRepresentation : public BaseAction
	{
	  public:
		explicit ApplyRepresentation( const std::unordered_set<Model::Molecule *> &				p_molecules,
									  const Model::Representation::InstantiatedRepresentation & p_source,
									  const Model::Representation::MEMBER_FLAG &				p_flag ) :
			_representation( p_source ),
			_flag( p_flag )
		{
			for ( Model::Molecule * const molecule : p_molecules )
				_molecules.emplace( molecule );
		}

		virtual void execute() override
		{
			for ( Model::Molecule * const molecule : _molecules )
			{
				Model::Representation::InstantiatedRepresentation * moleculeRepresentation;

				if ( molecule->hasCustomRepresentation() )
				{
					moleculeRepresentation = molecule->getCustomRepresentation();
				}
				else
				{
					moleculeRepresentation = Model::Representation::InstantiatedRepresentation::instantiateCopy(
						molecule->getRepresentation() );

					molecule->applyRepresentation( moleculeRepresentation );
				}

				moleculeRepresentation->applyData( _representation, _flag );
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		std::unordered_set<Model::Molecule *>					  _molecules = std::unordered_set<Model::Molecule *>();
		const Model::Representation::InstantiatedRepresentation & _representation;
		const Model::Representation::MEMBER_FLAG				  _flag;
	};

} // namespace VTX::Action::Molecule
#endif
