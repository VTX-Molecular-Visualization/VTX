#ifndef __VTX_ACTION_CHAIN__
#define __VTX_ACTION_CHAIN__

#include "app/action/visible.hpp"
#include "app/core/action/base_action.hpp"
#include "app/old_app/color/rgba.hpp"
#include "app/model/chain.hpp"
#include "app/model/representation/instantiated_representation.hpp"
#include <unordered_set>

namespace VTX::Action::Chain
{
	class ChangeColor : public Core::Action::BaseAction
	{
	  public:
		explicit ChangeColor( Model::Chain & p_chain, const Color::Rgba & p_color ) :
			_color( p_color ), _chains { &p_chain }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit ChangeColor( const std::unordered_set<Model::Chain *> & p_chains, const Color::Rgba & p_color ) :
			_color( p_color ), _chains( p_chains )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<Model::Chain *> _chains = std::unordered_set<Model::Chain *>();
		const Color::Rgba						 _color;
	};

	class ChangeVisibility : public Visible::ChangeVisibility
	{
	  public:
		explicit ChangeVisibility( Model::Chain & p_chain, const VISIBILITY_MODE p_mode ) :
			Visible::ChangeVisibility( p_chain, p_mode )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;
	};

	class ChangeRepresentationPreset : public Core::Action::BaseAction
	{
	  public:
		explicit ChangeRepresentationPreset( Model::Chain & p_chain, const int p_indexPreset ) :
			_indexPreset( p_indexPreset ), _chains { &p_chain }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit ChangeRepresentationPreset( const std::unordered_set<Model::Chain *> & p_chains,
											 const int									p_indexPreset ) :
			_indexPreset( p_indexPreset ),
			_chains( p_chains )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<Model::Chain *> _chains = std::unordered_set<Model::Chain *>();
		const int								 _indexPreset;
	};

	class RemoveRepresentation : public Core::Action::BaseAction
	{
	  public:
		explicit RemoveRepresentation( Model::Chain & p_chain ) : _chains { &p_chain }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit RemoveRepresentation( const std::unordered_set<Model::Chain *> & p_chains ) : _chains( p_chains )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<Model::Chain *> _chains;
	};

	class RemoveChildrenRepresentations : public Core::Action::BaseAction
	{
	  public:
		explicit RemoveChildrenRepresentations( Model::Chain & p_chain ) : _chains { &p_chain }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit RemoveChildrenRepresentations( const std::unordered_set<Model::Chain *> & p_chains ) :
			_chains( p_chains )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<Model::Chain *> _chains;
	};

	class Delete : public Core::Action::BaseAction
	{
	  public:
		explicit Delete( Model::Chain & p_chain ) : _chain( p_chain )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		Model::Chain & _chain;
	};

	class Copy : public Core::Action::BaseAction
	{
	  public:
		explicit Copy( const Model::Chain & p_target ) : _target( p_target )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		virtual void execute() override;

	  private:
		const Model::Chain & _target;
	};

	class Extract : public Core::Action::BaseAction
	{
	  public:
		explicit Extract( const Model::Chain & p_target ) : _target( p_target )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		virtual void execute() override;

	  private:
		const Model::Chain & _target;
	};

	class ApplyRepresentation : public Core::Action::BaseAction
	{
	  public:
		explicit ApplyRepresentation( const std::unordered_set<Model::Chain *> &				p_chains,
									  const Model::Representation::InstantiatedRepresentation & p_source,
									  const Model::Representation::MEMBER_FLAG &				p_flag ) :
			_representation( p_source ),
			_flag( p_flag ), _chains( p_chains )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::unordered_set<Model::Chain *>				  _chains = std::unordered_set<Model::Chain *>();
		const Model::Representation::InstantiatedRepresentation & _representation;
		const Model::Representation::MEMBER_FLAG				  _flag;
	};

} // namespace VTX::Action::Chain
#endif
