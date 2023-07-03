#ifndef __VTX_APP_OLD_ACTION_RESIDUE__
#define __VTX_APP_OLD_ACTION_RESIDUE__

#include "app/old/action.hpp"
#include "app/old/action/visible.hpp"
#include "app/old/application/representation/instantiated_representation.hpp"
#include "app/old/component/chemistry/residue.hpp"
#include <util/color/rgba.hpp>
#include <unordered_set>

namespace VTX::App::Old::Action::Residue
{
	class ChangeColor : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeColor( App::Old::Component::Chemistry::Residue & p_residue, const Util::Color::Rgba & p_color ) :
			_color( p_color ), _residues { &p_residue }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit ChangeColor( const std::unordered_set<App::Old::Component::Chemistry::Residue *> & p_residues, const Util::Color::Rgba & p_color ) :
			_color( p_color ), _residues( p_residues )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<App::Old::Component::Chemistry::Residue *> _residues;
		const Util::Color::Rgba						   _color;
	};

	class ChangeVisibility : public Visible::ChangeVisibility
	{
	  public:
		explicit ChangeVisibility( App::Old::Component::Chemistry::Residue & p_residue, const VISIBILITY_MODE p_mode ) :
			Visible::ChangeVisibility( p_residue, p_mode )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;
	};

	class ChangeRepresentationPreset : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeRepresentationPreset( App::Old::Component::Chemistry::Residue & p_residue, const int p_indexPreset ) :
			_indexPreset( p_indexPreset ), _residues { &p_residue }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit ChangeRepresentationPreset( const std::unordered_set<App::Old::Component::Chemistry::Residue *> & p_residues,
											 const int									  p_indexPreset ) :
			_indexPreset( p_indexPreset ),
			_residues( p_residues )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<App::Old::Component::Chemistry::Residue *> _residues;
		const int								   _indexPreset;
	};

	class RemoveRepresentation : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit RemoveRepresentation( App::Old::Component::Chemistry::Residue & p_residue ) : _residues { &p_residue }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit RemoveRepresentation( const std::unordered_set<App::Old::Component::Chemistry::Residue *> & p_residues ) :
			_residues( p_residues )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<App::Old::Component::Chemistry::Residue *> _residues;
	};

	class Delete : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit Delete( App::Old::Component::Chemistry::Residue & p_residue ) : _residue( p_residue )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		App::Old::Component::Chemistry::Residue & _residue;
	};

	class Copy : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit Copy( const App::Old::Component::Chemistry::Residue & p_target ) : _target( p_target )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		virtual void execute() override;

	  private:
		const App::Old::Component::Chemistry::Residue & _target;
	};

	class Extract : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit Extract( const App::Old::Component::Chemistry::Residue & p_target ) : _target( p_target )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		virtual void execute() override;

	  private:
		const App::Old::Component::Chemistry::Residue & _target;
	};

	class ApplyRepresentation : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ApplyRepresentation( const std::unordered_set<App::Old::Component::Chemistry::Residue *> &				p_residues,
									  const App::Old::Application::Representation::InstantiatedRepresentation & p_source,
									  const Application::Representation::MEMBER_FLAG &				p_flag ) :
			_representation( p_source ),
			_flag( p_flag ), _residues( p_residues )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<App::Old::Component::Chemistry::Residue *>				  _residues;
		const App::Old::Application::Representation::InstantiatedRepresentation & _representation;
		const Application::Representation::MEMBER_FLAG				  _flag;
	};
} // namespace VTX::App::Old::Action::Residue
#endif
