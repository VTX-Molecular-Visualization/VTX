#ifndef __VTX_ACTION_MOLECULE__
#define __VTX_ACTION_MOLECULE__

#include "app/action/visible.hpp"
#include "app/core/action/base_action.hpp"
#include "app/core/action/base_action_undonable.hpp"
#include "app/model/molecule.hpp"
#include "app/model/representation/instantiated_representation.hpp"
#include "app/old_app/trajectory/trajectory_enum.hpp"
#include <string>
#include <unordered_set>

namespace VTX::Action::Molecule
{
	class RefreshSolventExcludedSurface : public Core::Action::BaseAction
	{
	  public:
		explicit RefreshSolventExcludedSurface( Model::Molecule & p_molecule ) : _molecule( p_molecule ) {}

		virtual void execute() override;

	  private:
		Model::Molecule & _molecule;
	};

	class ChangeColor : public Core::Action::BaseAction
	{
	  public:
		explicit ChangeColor( Model::Molecule & p_molecule, const Color::Rgba & p_color ) :
			_color( p_color ), _molecules { &p_molecule }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit ChangeColor( const std::unordered_set<Model::Molecule *> & p_molecules, const Color::Rgba & p_color ) :
			_color( p_color ), _molecules( p_molecules )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<Model::Molecule *> _molecules;
		const Color::Rgba							_color;
	};

	class ChangeVisibility : public Visible::ChangeVisibility
	{
	  public:
		explicit ChangeVisibility( Model::Molecule & p_molecule, const VISIBILITY_MODE p_mode ) :
			Visible::ChangeVisibility( p_molecule, p_mode )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;
	};

	class ChangeRepresentationPreset : public Core::Action::BaseAction
	{
	  public:
		explicit ChangeRepresentationPreset( Model::Molecule & p_molecule, const int p_indexPreset ) :
			_indexPreset( p_indexPreset ), _molecules { &p_molecule }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit ChangeRepresentationPreset( const std::unordered_set<Model::Molecule *> & p_molecules,
											 const int									   p_indexPreset ) :
			_indexPreset( p_indexPreset ),
			_molecules( p_molecules )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<Model::Molecule *> _molecules;
		const int									_indexPreset;
	};
	class RemoveRepresentation : public Core::Action::BaseAction
	{
	  public:
		explicit RemoveRepresentation( Model::Molecule & p_molecule ) : _molecules { &p_molecule }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit RemoveRepresentation( const std::unordered_set<Model::Molecule *> & p_molecules ) :
			_molecules( p_molecules )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<Model::Molecule *> _molecules;
	};
	class RemoveChildrenRepresentations : public Core::Action::BaseAction
	{
	  public:
		explicit RemoveChildrenRepresentations( Model::Molecule & p_molecule ) : _molecules { &p_molecule }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit RemoveChildrenRepresentations( const std::unordered_set<Model::Molecule *> & p_molecules ) :
			_molecules( p_molecules )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<Model::Molecule *> _molecules;
	};

	class ChangeFPS : public Core::Action::BaseAction
	{
	  public:
		explicit ChangeFPS( Model::Molecule & p_molecule, const int p_fps ) : _fps( p_fps ), _molecules { &p_molecule }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit ChangeFPS( std::unordered_set<Model::Molecule *> & p_molecules, const int p_fps ) :
			_fps( p_fps ), _molecules( p_molecules )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<Model::Molecule *> _molecules;
		const int									_fps;
	};

	class ChangeFrame : public Core::Action::BaseAction
	{
	  public:
		explicit ChangeFrame( Model::Molecule & p_molecule, const int p_frame, const bool p_pause = false ) :
			_frame( p_frame ), _pause( p_pause ), _molecules { &p_molecule }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit ChangeFrame( std::unordered_set<Model::Molecule *> & p_molecules,
							  const int								  p_frame,
							  const bool							  p_pause = false ) :
			_frame( p_frame ),
			_pause( p_pause ), _molecules( p_molecules )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<Model::Molecule *> _molecules;
		const int									_frame;
		const int									_pause;
	};

	class PreviousFrame : public Core::Action::BaseAction
	{
	  public:
		explicit PreviousFrame( Model::Molecule & p_molecule, const bool p_pause = false ) :
			_pause( p_pause ), _molecules { &p_molecule }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit PreviousFrame( std::unordered_set<Model::Molecule *> & p_molecules, const bool p_pause = false ) :
			_pause( p_pause ), _molecules( p_molecules )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<Model::Molecule *> _molecules;
		const bool									_pause;
	};

	class NextFrame : public Core::Action::BaseAction
	{
	  public:
		explicit NextFrame( Model::Molecule & p_molecule, const bool p_pause = false ) :
			_pause( p_pause ), _molecules { &p_molecule }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit NextFrame( std::unordered_set<Model::Molecule *> & p_molecules, const bool p_pause = false ) :
			_pause( p_pause ), _molecules( p_molecules )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<Model::Molecule *> _molecules;
		const bool									_pause;
	};

	class ChangeIsPlaying : public Core::Action::BaseAction
	{
	  public:
		explicit ChangeIsPlaying( Model::Molecule & p_molecule, const bool p_isPlaying ) :
			_isPlaying( p_isPlaying ), _molecules { &p_molecule }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit ChangeIsPlaying( std::unordered_set<Model::Molecule *> & p_molecules, const bool p_isPlaying ) :
			_isPlaying( p_isPlaying ), _molecules( p_molecules )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<Model::Molecule *> _molecules;
		const bool									_isPlaying;
	};
	class ChangePlayMode : public Core::Action::BaseAction
	{
	  public:
		explicit ChangePlayMode( Model::Molecule & p_molecule, const Trajectory::PlayMode & p_playMode ) :
			_playMode( p_playMode ), _molecules { &p_molecule }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit ChangePlayMode( std::unordered_set<Model::Molecule *> & p_molecules,
								 const Trajectory::PlayMode &			 p_playMode ) :
			_playMode( p_playMode ),
			_molecules( p_molecules )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<Model::Molecule *> _molecules;
		const Trajectory::PlayMode					_playMode;
	};

	class ChangeShowIon : public Core::Action::BaseAction
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

		virtual void execute() override;

	  private:
		const std::unordered_set<Model::Molecule *> _molecules;
		const bool									_showIon;
	};

	class ChangeShowSolvent : public Core::Action::BaseAction
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

		virtual void execute() override;

	  private:
		const std::unordered_set<Model::Molecule *> _molecules;
		const bool									_showSolvent;
	};

	class ChangeShowWater : public Core::Action::BaseAction
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

		virtual void execute() override;

	  private:
		const std::unordered_set<Model::Molecule *> _molecules;
		const bool									_showWater;
	};
	class ChangeShowHydrogen : public Core::Action::BaseAction
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

		virtual void execute() override;

	  private:
		const std::unordered_set<Model::Molecule *> _molecules;
		const bool									_showHydrogen;
	};

	class Delete : public Core::Action::BaseAction
	{
	  public:
		explicit Delete( Model::Molecule & p_molecule ) : _molecule( p_molecule )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		Model::Molecule & _molecule;
	};

	class Copy : public Core::Action::BaseAction
	{
	  public:
		explicit Copy( const Model::Molecule & p_target ) : _target( p_target )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		virtual void execute() override;

	  private:
		const Model::Molecule & _target;
	};

	class Rename : public Core::Action::BaseActionUndonable
	{
	  public:
		explicit Rename( Model::Molecule & p_target, const std::string & p_newName ) :
			_target( p_target ), _oldName( p_target.getDisplayName() ), _newName( p_newName )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		void execute() override;
		void undo() override;

	  private:
		Model::Molecule & _target;

		const std::string _oldName;
		const std::string _newName;
	};

	class ApplyRepresentation : public Core::Action::BaseAction
	{
	  public:
		explicit ApplyRepresentation( const std::unordered_set<Model::Molecule *> &				p_molecules,
									  const Model::Representation::InstantiatedRepresentation & p_source,
									  const Model::Representation::MEMBER_FLAG &				p_flag ) :
			_representation( p_source ),
			_flag( p_flag ), _molecules( p_molecules )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<Model::Molecule *>				  _molecules;
		const Model::Representation::InstantiatedRepresentation & _representation;
		const Model::Representation::MEMBER_FLAG				  _flag;
	};

} // namespace VTX::Action::Molecule
#endif
