#ifndef __VTX_APP_OLD_ACTION_CHAIN__
#define __VTX_APP_OLD_ACTION_CHAIN__

#include "app/old/action.hpp"
#include "app/old/action/visible.hpp"
#include "app/old/core/action/base_action.hpp"
#include "app/old/component/chemistry/chain.hpp"
#include "app/old/application/representation/instantiated_representation.hpp"
#include <util/color/rgba.hpp>
#include <unordered_set>

namespace VTX::App::Old::Action::Chain
{
	class ChangeColor : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeColor( App::Old::Component::Chemistry::Chain & p_chain, const Util::Color::Rgba & p_color ) :
			_color( p_color ), _chains { &p_chain }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit ChangeColor( const std::unordered_set<App::Old::Component::Chemistry::Chain *> & p_chains, const Util::Color::Rgba & p_color ) :
			_color( p_color ), _chains( p_chains )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<App::Old::Component::Chemistry::Chain *> _chains = std::unordered_set<App::Old::Component::Chemistry::Chain *>();
		const Util::Color::Rgba						 _color;
	};

	class ChangeVisibility : public Visible::ChangeVisibility
	{
	  public:
		explicit ChangeVisibility( App::Old::Component::Chemistry::Chain & p_chain, const VISIBILITY_MODE p_mode ) :
			Visible::ChangeVisibility( p_chain, p_mode )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;
	};

	class ChangeRepresentationPreset : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeRepresentationPreset( App::Old::Component::Chemistry::Chain & p_chain, const int p_indexPreset ) :
			_indexPreset( p_indexPreset ), _chains { &p_chain }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit ChangeRepresentationPreset( const std::unordered_set<App::Old::Component::Chemistry::Chain *> & p_chains,
											 const int									p_indexPreset ) :
			_indexPreset( p_indexPreset ),
			_chains( p_chains )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<App::Old::Component::Chemistry::Chain *> _chains = std::unordered_set<App::Old::Component::Chemistry::Chain *>();
		const int								 _indexPreset;
	};

	class RemoveRepresentation : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit RemoveRepresentation( App::Old::Component::Chemistry::Chain & p_chain ) : _chains { &p_chain }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit RemoveRepresentation( const std::unordered_set<App::Old::Component::Chemistry::Chain *> & p_chains ) : _chains( p_chains )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<App::Old::Component::Chemistry::Chain *> _chains;
	};

	class RemoveChildrenRepresentations : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit RemoveChildrenRepresentations( App::Old::Component::Chemistry::Chain & p_chain ) : _chains { &p_chain }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit RemoveChildrenRepresentations( const std::unordered_set<App::Old::Component::Chemistry::Chain *> & p_chains ) :
			_chains( p_chains )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<App::Old::Component::Chemistry::Chain *> _chains;
	};

	class Delete : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit Delete( App::Old::Component::Chemistry::Chain & p_chain ) : _chain( p_chain )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		App::Old::Component::Chemistry::Chain & _chain;
	};

	class Copy : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit Copy( const App::Old::Component::Chemistry::Chain & p_target ) : _target( p_target )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		virtual void execute() override;

	  private:
		const App::Old::Component::Chemistry::Chain & _target;
	};

	class Extract : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit Extract( const App::Old::Component::Chemistry::Chain & p_target ) : _target( p_target )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		virtual void execute() override;

	  private:
		const App::Old::Component::Chemistry::Chain & _target;
	};

	class ApplyRepresentation : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ApplyRepresentation( const std::unordered_set<App::Old::Component::Chemistry::Chain *> &				p_chains,
									  const App::Old::Application::Representation::InstantiatedRepresentation & p_source,
									  const Application::Representation::MEMBER_FLAG &				p_flag ) :
			_representation( p_source ),
			_flag( p_flag ), _chains( p_chains )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<App::Old::Component::Chemistry::Chain *>				  _chains = std::unordered_set<App::Old::Component::Chemistry::Chain *>();
		const App::Old::Application::Representation::InstantiatedRepresentation & _representation;
		const Application::Representation::MEMBER_FLAG				  _flag;
	};

} // namespace VTX::App::Old::Action::Chain
#endif
