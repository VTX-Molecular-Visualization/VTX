#ifndef __VTX_ACTION_MOLECULE__
#define __VTX_ACTION_MOLECULE__

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
#include "trajectory/trajectory_enum.hpp"
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
		explicit ChangeColor( Model::Molecule & p_molecule, const Color::Rgb & p_color ) :
			_color( p_color ), _molecules { &p_molecule }
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		explicit ChangeColor( const std::unordered_set<Model::Molecule *> & p_molecules, const Color::Rgb & p_color ) :
			_color( p_color ), _molecules( p_molecules )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			for ( Model::Molecule * const molecule : _molecules )
			{
				molecule->setColor( _color );
				molecule->refreshColors();
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		const std::unordered_set<Model::Molecule *> _molecules;
		const Color::Rgb							_color;
	};

	class ChangeVisibility : public Visible::ChangeVisibility
	{
	  public:
		explicit ChangeVisibility( Model::Molecule & p_molecule, const VISIBILITY_MODE p_mode ) :
			Visible::ChangeVisibility( p_molecule, p_mode )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			for ( Generic::BaseVisible * const visible : _visibles )
			{
				Model::Molecule * molecule = static_cast<Model::Molecule *>( visible );

				switch ( _mode )
				{
				case VISIBILITY_MODE::SHOW:
				case VISIBILITY_MODE::HIDE:
				case VISIBILITY_MODE::ALL:
					Util::Molecule::show( *molecule, _getVisibilityBool( *molecule ), true, true );
					break;
				case VISIBILITY_MODE::SOLO: Util::Molecule::solo( *molecule, false ); break;
				}
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}
	};

	class ChangeRepresentationPreset : public BaseAction
	{
	  public:
		explicit ChangeRepresentationPreset( Model::Molecule & p_molecule, const int p_indexPreset ) :
			_indexPreset( p_indexPreset ), _molecules { &p_molecule }
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		explicit ChangeRepresentationPreset( const std::unordered_set<Model::Molecule *> & p_molecules,
											 const int									   p_indexPreset ) :
			_indexPreset( p_indexPreset ),
			_molecules( p_molecules )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			Model::Representation::Representation * const preset
				= Model::Representation::RepresentationLibrary::get().getRepresentation( _indexPreset );

			Representation::RepresentationManager::get().instantiateRepresentations( preset, _molecules );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		const std::unordered_set<Model::Molecule *> _molecules;
		const int									_indexPreset;
	};
	class RemoveRepresentation : public BaseAction
	{
	  public:
		explicit RemoveRepresentation( Model::Molecule & p_molecule ) : _molecules { &p_molecule }
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		explicit RemoveRepresentation( const std::unordered_set<Model::Molecule *> & p_molecules ) :
			_molecules( p_molecules )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			for ( Model::Molecule * const molecule : _molecules )
				VTX::Representation::RepresentationManager::get().instantiateDefaultRepresentation( *molecule );

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		const std::unordered_set<Model::Molecule *> _molecules;
	};
	class RemoveChildrenRepresentations : public BaseAction
	{
	  public:
		explicit RemoveChildrenRepresentations( Model::Molecule & p_molecule ) : _molecules { &p_molecule }
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		explicit RemoveChildrenRepresentations( const std::unordered_set<Model::Molecule *> & p_molecules ) :
			_molecules( p_molecules )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			for ( Model::Molecule * const molecule : _molecules )
			{
				molecule->removeChildrenRepresentations();
				molecule->computeAllRepresentationData();
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		const std::unordered_set<Model::Molecule *> _molecules;
	};

	class ChangeFPS : public BaseAction
	{
	  public:
		explicit ChangeFPS( Model::Molecule & p_molecule, const int p_fps ) : _fps( p_fps ), _molecules { &p_molecule }
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		explicit ChangeFPS( std::unordered_set<Model::Molecule *> & p_molecules, const int p_fps ) :
			_fps( p_fps ), _molecules( p_molecules )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			for ( Model::Molecule * molecule : _molecules )
				molecule->setFPS(
					Util::Math::clamp( _fps, VTX::Setting::MIN_TRAJECTORY_SPEED, VTX::Setting::MAX_TRAJECTORY_SPEED ) );
		}

	  private:
		const std::unordered_set<Model::Molecule *> _molecules;
		const int									_fps;
	};

	class ChangeFrame : public BaseAction
	{
	  public:
		explicit ChangeFrame( Model::Molecule & p_molecule, const int p_frame, const bool p_pause = false ) :
			_frame( p_frame ), _pause( p_pause ), _molecules { &p_molecule }
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		explicit ChangeFrame( std::unordered_set<Model::Molecule *> & p_molecules,
							  const int								  p_frame,
							  const bool							  p_pause = false ) :
			_frame( p_frame ),
			_pause( p_pause ), _molecules( p_molecules )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			for ( Model::Molecule * const molecule : _molecules )
			{
				molecule->setFrame( Util::Math::clamp( _frame, 0, (int)molecule->getFrameCount() - 1 ) );
				if ( _pause )
					molecule->setIsPlaying( false );
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		const std::unordered_set<Model::Molecule *> _molecules;
		const int									_frame;
		const int									_pause;
	};

	class PreviousFrame : public BaseAction
	{
	  public:
		explicit PreviousFrame( Model::Molecule & p_molecule, const bool p_pause = false ) :
			_pause( p_pause ), _molecules { &p_molecule }
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		explicit PreviousFrame( std::unordered_set<Model::Molecule *> & p_molecules, const bool p_pause = false ) :
			_pause( p_pause ), _molecules( p_molecules )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			for ( Model::Molecule * const molecule : _molecules )
			{
				const int previousFrame		   = molecule->getFrame() - 1;
				const int clampedPreviousFrame = previousFrame < 0 ? 0 : previousFrame;

				molecule->setFrame( clampedPreviousFrame );

				if ( _pause )
					molecule->setIsPlaying( false );
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		const std::unordered_set<Model::Molecule *> _molecules;
		const bool									_pause;
	};

	class NextFrame : public BaseAction
	{
	  public:
		explicit NextFrame( Model::Molecule & p_molecule, const bool p_pause = false ) :
			_pause( p_pause ), _molecules { &p_molecule }
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		explicit NextFrame( std::unordered_set<Model::Molecule *> & p_molecules, const bool p_pause = false ) :
			_pause( p_pause ), _molecules( p_molecules )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			for ( Model::Molecule * const molecule : _molecules )
			{
				const int nextFrame		   = molecule->getFrame() + 1;
				const int lastFrame		   = molecule->getFrameCount() - 1;
				const int clampedNextFrame = nextFrame > lastFrame ? lastFrame : nextFrame;

				molecule->setFrame( clampedNextFrame );

				if ( _pause )
					molecule->setIsPlaying( false );
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		const std::unordered_set<Model::Molecule *> _molecules;
		const bool									_pause;
	};

	class ChangeIsPlaying : public BaseAction
	{
	  public:
		explicit ChangeIsPlaying( Model::Molecule & p_molecule, const bool p_isPlaying ) :
			_isPlaying( p_isPlaying ), _molecules { &p_molecule }
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		explicit ChangeIsPlaying( std::unordered_set<Model::Molecule *> & p_molecules, const bool p_isPlaying ) :
			_isPlaying( p_isPlaying ), _molecules( p_molecules )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			for ( Model::Molecule * const molecule : _molecules )
			{
				if ( molecule->isAtEndOfTrajectoryPlay() && _isPlaying )
					molecule->resetTrajectoryPlay();

				molecule->setIsPlaying( _isPlaying );
			}
		}

	  private:
		const std::unordered_set<Model::Molecule *> _molecules;
		const bool									_isPlaying;
	};
	class ChangePlayMode : public BaseAction
	{
	  public:
		explicit ChangePlayMode( Model::Molecule & p_molecule, const Trajectory::PlayMode & p_playMode ) :
			_playMode( p_playMode ), _molecules { &p_molecule }
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		explicit ChangePlayMode( std::unordered_set<Model::Molecule *> & p_molecules,
								 const Trajectory::PlayMode &			 p_playMode ) :
			_playMode( p_playMode ),
			_molecules( p_molecules )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			for ( Model::Molecule * const molecule : _molecules )
				molecule->setPlayMode( _playMode );
		}

	  private:
		const std::unordered_set<Model::Molecule *> _molecules;
		const Trajectory::PlayMode					_playMode;
	};

	class ChangeShowIon : public BaseAction
	{
	  public:
		explicit ChangeShowIon( Model::Molecule & p_molecule, const bool p_showIon ) :
			_showIon( p_showIon ), _molecules { &p_molecule }
		{
		}
		explicit ChangeShowIon( std::unordered_set<Model::Molecule *> & p_molecules, const bool p_showIon ) :
			_showIon( p_showIon ), _molecules( p_molecules )
		{
		}

		virtual void execute() override
		{
			for ( Model::Molecule * const molecule : _molecules )
				molecule->setShowIon( _showIon );

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		const std::unordered_set<Model::Molecule *> _molecules;
		const bool									_showIon;
	};

	class ChangeShowSolvent : public BaseAction
	{
	  public:
		explicit ChangeShowSolvent( Model::Molecule & p_molecule, const bool p_showSolvent ) :
			_showSolvent( p_showSolvent ), _molecules { &p_molecule }
		{
		}
		explicit ChangeShowSolvent( std::unordered_set<Model::Molecule *> & p_molecules, const bool p_showSolvent ) :
			_showSolvent( p_showSolvent ), _molecules( p_molecules )
		{
		}

		virtual void execute() override
		{
			for ( Model::Molecule * const molecule : _molecules )
				molecule->setShowSolvent( _showSolvent );

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		const std::unordered_set<Model::Molecule *> _molecules;
		const bool									_showSolvent;
	};

	class ChangeShowWater : public BaseAction
	{
	  public:
		explicit ChangeShowWater( Model::Molecule & p_molecule, const bool p_showWater ) :
			_showWater( p_showWater ), _molecules { &p_molecule }
		{
		}
		explicit ChangeShowWater( std::unordered_set<Model::Molecule *> & p_molecules, const bool p_showWater ) :
			_showWater( p_showWater ), _molecules( p_molecules )
		{
		}

		virtual void execute() override
		{
			for ( Model::Molecule * const molecule : _molecules )
				molecule->setShowWater( _showWater );

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		const std::unordered_set<Model::Molecule *> _molecules;
		const bool									_showWater;
	};
	class ChangeShowHydrogen : public BaseAction
	{
	  public:
		explicit ChangeShowHydrogen( Model::Molecule & p_molecule, const bool p_showHydrogen ) :
			_showHydrogen( p_showHydrogen ), _molecules { &p_molecule }
		{
		}
		explicit ChangeShowHydrogen( std::unordered_set<Model::Molecule *> & p_molecules, const bool p_showHydrogen ) :
			_showHydrogen( p_showHydrogen ), _molecules( p_molecules )
		{
		}

		virtual void execute() override
		{
			for ( Model::Molecule * const molecule : _molecules )
				molecule->setShowHydrogen( _showHydrogen );

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		const std::unordered_set<Model::Molecule *> _molecules;
		const bool									_showHydrogen;
	};

	class Delete : public BaseAction
	{
	  public:
		explicit Delete( Model::Molecule & p_molecule ) : _molecule( p_molecule )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			VTXApp::get().getScene().removeMolecule( &_molecule );
			MVC::MvcManager::get().deleteModel( &_molecule );
		}

	  private:
		Model::Molecule & _molecule;
	};

	class Orient : public BaseAction
	{
	  public:
		explicit Orient( const Model::Molecule & p_molecule ) : _molecule( p_molecule ) {}

		virtual void execute() override
		{
			VTXApp::get()
				.getStateMachine()
				.getState<State::Visualization>( ID::State::VISUALIZATION )
				->orientCameraController( _molecule.getWorldAABB() );
		}

	  private:
		const Model::Molecule & _molecule;
	};

	class Copy : public BaseAction
	{
	  public:
		explicit Copy( const Model::Molecule & p_target ) : _target( p_target )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		virtual void execute() override
		{
			Model::GeneratedMolecule * generatedMolecule
				= MVC::MvcManager::get().instantiateModel<Model::GeneratedMolecule>();

			generatedMolecule->copyFromMolecule( _target );
			generatedMolecule->applyTransform( _target.getTransform() );

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
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
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
			_flag( p_flag ), _molecules( p_molecules )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			Representation::RepresentationManager::get().applyRepresentation( _molecules, _representation, _flag );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		const std::unordered_set<Model::Molecule *>				  _molecules;
		const Model::Representation::InstantiatedRepresentation & _representation;
		const Model::Representation::MEMBER_FLAG				  _flag;
	};

} // namespace VTX::Action::Molecule
#endif
