#ifndef __VTX_APP_OLD_ACTION_MOLECULE__
#define __VTX_APP_OLD_ACTION_MOLECULE__

#include "app/old/action.hpp"
#include "app/old/action/visible.hpp"
#include "app/old/component/chemistry/enum_trajectory.hpp"
#include "app/old/core/action/base_action.hpp"
#include "app/old/core/action/base_action_undonable.hpp"
#include "app/old/component/chemistry/molecule.hpp"
#include "app/old/application/representation/instantiated_representation.hpp"
#include <string>
#include <unordered_set>

namespace VTX::App::Old::Action::Molecule
{
	class RefreshSolventExcludedSurface : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit RefreshSolventExcludedSurface( App::Old::Component::Chemistry::Molecule & p_molecule ) : _molecule( p_molecule ) {}

		virtual void execute() override;

	  private:
		App::Old::Component::Chemistry::Molecule & _molecule;
	};

	class ChangeColor : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeColor( App::Old::Component::Chemistry::Molecule & p_molecule, const Util::Color::Rgba & p_color ) :
			_color( p_color ), _molecules { &p_molecule }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit ChangeColor( const std::unordered_set<App::Old::Component::Chemistry::Molecule *> & p_molecules, const Util::Color::Rgba & p_color ) :
			_color( p_color ), _molecules( p_molecules )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<App::Old::Component::Chemistry::Molecule *> _molecules;
		const Util::Color::Rgba							_color;
	};

	class ChangeVisibility : public Visible::ChangeVisibility
	{
	  public:
		explicit ChangeVisibility( App::Old::Component::Chemistry::Molecule & p_molecule, const VISIBILITY_MODE p_mode ) :
			Visible::ChangeVisibility( p_molecule, p_mode )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;
	};

	class ChangeRepresentationPreset : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeRepresentationPreset( App::Old::Component::Chemistry::Molecule & p_molecule, const int p_indexPreset ) :
			_indexPreset( p_indexPreset ), _molecules { &p_molecule }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit ChangeRepresentationPreset( const std::unordered_set<App::Old::Component::Chemistry::Molecule *> & p_molecules,
											 const int									   p_indexPreset ) :
			_indexPreset( p_indexPreset ),
			_molecules( p_molecules )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<App::Old::Component::Chemistry::Molecule *> _molecules;
		const int									_indexPreset;
	};
	class RemoveRepresentation : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit RemoveRepresentation( App::Old::Component::Chemistry::Molecule & p_molecule ) : _molecules { &p_molecule }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit RemoveRepresentation( const std::unordered_set<App::Old::Component::Chemistry::Molecule *> & p_molecules ) :
			_molecules( p_molecules )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<App::Old::Component::Chemistry::Molecule *> _molecules;
	};
	class RemoveChildrenRepresentations : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit RemoveChildrenRepresentations( App::Old::Component::Chemistry::Molecule & p_molecule ) : _molecules { &p_molecule }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit RemoveChildrenRepresentations( const std::unordered_set<App::Old::Component::Chemistry::Molecule *> & p_molecules ) :
			_molecules( p_molecules )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<App::Old::Component::Chemistry::Molecule *> _molecules;
	};

	class ChangeFPS : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeFPS( App::Old::Component::Chemistry::Molecule & p_molecule, const int p_fps ) : _fps( p_fps ), _molecules { &p_molecule }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit ChangeFPS( std::unordered_set<App::Old::Component::Chemistry::Molecule *> & p_molecules, const int p_fps ) :
			_fps( p_fps ), _molecules( p_molecules )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<App::Old::Component::Chemistry::Molecule *> _molecules;
		const int									_fps;
	};

	class ChangeFrame : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeFrame( App::Old::Component::Chemistry::Molecule & p_molecule, const int p_frame, const bool p_pause = false ) :
			_frame( p_frame ), _pause( p_pause ), _molecules { &p_molecule }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit ChangeFrame( std::unordered_set<App::Old::Component::Chemistry::Molecule *> & p_molecules,
							  const int								  p_frame,
							  const bool							  p_pause = false ) :
			_frame( p_frame ),
			_pause( p_pause ), _molecules( p_molecules )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<App::Old::Component::Chemistry::Molecule *> _molecules;
		const int									_frame;
		const int									_pause;
	};

	class PreviousFrame : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit PreviousFrame( App::Old::Component::Chemistry::Molecule & p_molecule, const bool p_pause = false ) :
			_pause( p_pause ), _molecules { &p_molecule }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit PreviousFrame( std::unordered_set<App::Old::Component::Chemistry::Molecule *> & p_molecules, const bool p_pause = false ) :
			_pause( p_pause ), _molecules( p_molecules )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<App::Old::Component::Chemistry::Molecule *> _molecules;
		const bool									_pause;
	};

	class NextFrame : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit NextFrame( App::Old::Component::Chemistry::Molecule & p_molecule, const bool p_pause = false ) :
			_pause( p_pause ), _molecules { &p_molecule }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit NextFrame( std::unordered_set<App::Old::Component::Chemistry::Molecule *> & p_molecules, const bool p_pause = false ) :
			_pause( p_pause ), _molecules( p_molecules )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<App::Old::Component::Chemistry::Molecule *> _molecules;
		const bool									_pause;
	};

	class ChangeIsPlaying : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeIsPlaying( App::Old::Component::Chemistry::Molecule & p_molecule, const bool p_isPlaying ) :
			_isPlaying( p_isPlaying ), _molecules { &p_molecule }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit ChangeIsPlaying( std::unordered_set<App::Old::Component::Chemistry::Molecule *> & p_molecules, const bool p_isPlaying ) :
			_isPlaying( p_isPlaying ), _molecules( p_molecules )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<App::Old::Component::Chemistry::Molecule *> _molecules;
		const bool									_isPlaying;
	};
	class ChangePlayMode : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangePlayMode( App::Old::Component::Chemistry::Molecule & p_molecule, const Component::Chemistry::PlayMode & p_playMode ) :
			_playMode( p_playMode ), _molecules { &p_molecule }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit ChangePlayMode( std::unordered_set<App::Old::Component::Chemistry::Molecule *> & p_molecules,
								 const Component::Chemistry::PlayMode &	 p_playMode ) :
			_playMode( p_playMode ),
			_molecules( p_molecules )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<App::Old::Component::Chemistry::Molecule *> _molecules;
		const Component::Chemistry::PlayMode		_playMode;
	};

	class ChangeShowIon : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeShowIon( App::Old::Component::Chemistry::Molecule & p_molecule, const bool p_showIon ) :
			_showIon( p_showIon ), _molecules { &p_molecule }
		{
		}
		explicit ChangeShowIon( std::unordered_set<App::Old::Component::Chemistry::Molecule *> & p_molecules, const bool p_showIon ) :
			_showIon( p_showIon ), _molecules( p_molecules )
		{
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<App::Old::Component::Chemistry::Molecule *> _molecules;
		const bool									_showIon;
	};

	class ChangeShowSolvent : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeShowSolvent( App::Old::Component::Chemistry::Molecule & p_molecule, const bool p_showSolvent ) :
			_showSolvent( p_showSolvent ), _molecules { &p_molecule }
		{
		}
		explicit ChangeShowSolvent( std::unordered_set<App::Old::Component::Chemistry::Molecule *> & p_molecules, const bool p_showSolvent ) :
			_showSolvent( p_showSolvent ), _molecules( p_molecules )
		{
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<App::Old::Component::Chemistry::Molecule *> _molecules;
		const bool									_showSolvent;
	};

	class ChangeShowWater : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeShowWater( App::Old::Component::Chemistry::Molecule & p_molecule, const bool p_showWater ) :
			_showWater( p_showWater ), _molecules { &p_molecule }
		{
		}
		explicit ChangeShowWater( std::unordered_set<App::Old::Component::Chemistry::Molecule *> & p_molecules, const bool p_showWater ) :
			_showWater( p_showWater ), _molecules( p_molecules )
		{
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<App::Old::Component::Chemistry::Molecule *> _molecules;
		const bool									_showWater;
	};
	class ChangeShowHydrogen : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeShowHydrogen( App::Old::Component::Chemistry::Molecule & p_molecule, const bool p_showHydrogen ) :
			_showHydrogen( p_showHydrogen ), _molecules { &p_molecule }
		{
		}
		explicit ChangeShowHydrogen( std::unordered_set<App::Old::Component::Chemistry::Molecule *> & p_molecules, const bool p_showHydrogen ) :
			_showHydrogen( p_showHydrogen ), _molecules( p_molecules )
		{
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<App::Old::Component::Chemistry::Molecule *> _molecules;
		const bool									_showHydrogen;
	};

	class Delete : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit Delete( App::Old::Component::Chemistry::Molecule & p_molecule ) : _molecule( p_molecule )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		App::Old::Component::Chemistry::Molecule & _molecule;
	};

	class Copy : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit Copy( const App::Old::Component::Chemistry::Molecule & p_target ) : _target( p_target )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		virtual void execute() override;

	  private:
		const App::Old::Component::Chemistry::Molecule & _target;
	};

	class Rename : public App::Old::Core::Action::BaseActionUndonable
	{
	  public:
		explicit Rename( App::Old::Component::Chemistry::Molecule & p_target, const std::string & p_newName ) :
			_target( p_target ), _oldName( p_target.getDisplayName() ), _newName( p_newName )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		void execute() override;
		void undo() override;

	  private:
		App::Old::Component::Chemistry::Molecule & _target;

		const std::string _oldName;
		const std::string _newName;
	};

	class ApplyRepresentation : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ApplyRepresentation( const std::unordered_set<App::Old::Component::Chemistry::Molecule *> &				p_molecules,
									  const App::Old::Application::Representation::InstantiatedRepresentation & p_source,
									  const Application::Representation::MEMBER_FLAG &				p_flag ) :
			_representation( p_source ),
			_flag( p_flag ), _molecules( p_molecules )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<App::Old::Component::Chemistry::Molecule *>				  _molecules;
		const App::Old::Application::Representation::InstantiatedRepresentation & _representation;
		const Application::Representation::MEMBER_FLAG				  _flag;
	};

} // namespace VTX::App::Old::Action::Molecule
#endif