#ifndef __VTX_APP_ACTION_CATEGORY__
#define __VTX_APP_ACTION_CATEGORY__

#include "app/action.hpp"
#include "app/action/visible.hpp"
#include "app/core/action/base_action.hpp"
#include "app/component/chemistry/category.hpp"
// #include "app/model/representation/instantiated_representation.hpp"
// #include <unordered_set>

namespace VTX::App::Action::Category
{
	// class ChangeColor : public App::Core::Action::BaseAction
	//{
	//  public:
	//	explicit ChangeColor( App::Component::Chemistry::Category & p_category, const Color::Rgb & p_color ) :
	//		_color( p_color ), _categories { &p_category }
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}
	//	explicit ChangeColor( const std::unordered_set<App::Component::Chemistry::Category *> & p_categories, const Color::Rgb & p_color ) :
	//		_color( p_color ), _categories( p_categories )
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}

	//	virtual void execute() override;

	//  private:
	//	const std::unordered_set<App::Component::Chemistry::Category *> _categories = std::unordered_set<App::Component::Chemistry::Category *>();
	//	const Color::Rgb						 _color;
	//};

	class ChangeVisibility : public Visible::ChangeVisibility
	{
	  public:
		explicit ChangeVisibility( App::Component::Chemistry::Category & p_category, const VISIBILITY_MODE p_mode ) :
			Visible::ChangeVisibility( p_category, p_mode )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;
	};

	// class ChangeRepresentationPreset : public App::Core::Action::BaseAction
	//{
	//  public:
	//	explicit ChangeRepresentationPreset( App::Component::Chemistry::Category & p_category, const int p_indexPreset ) :
	//		_indexPreset( p_indexPreset ), _categories { &p_category }
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}
	//	explicit ChangeRepresentationPreset( const std::unordered_set<App::Component::Chemistry::Category *> & p_categories,
	//										 const int									p_indexPreset ) :
	//		_indexPreset( p_indexPreset ),
	//		_categories( p_categories )
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}

	//	virtual void execute() override;

	//  private:
	//	const std::unordered_set<App::Component::Chemistry::Category *> _categories = std::unordered_set<App::Component::Chemistry::Category *>();
	//	const int								 _indexPreset;
	//};

	// class RemoveRepresentation : public App::Core::Action::BaseAction
	//{
	//  public:
	//	explicit RemoveRepresentation( App::Component::Chemistry::Category & p_category ) : _categories { &p_category }
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}
	//	explicit RemoveRepresentation( const std::unordered_set<App::Component::Chemistry::Category *> & p_categories ) : _categories(
	// p_categories )
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}

	//	virtual void execute() override;

	//  private:
	//	const std::unordered_set<App::Component::Chemistry::Category *> _categories;
	//};

	// class RemoveChildrenRepresentations : public App::Core::Action::BaseAction
	//{
	//  public:
	//	explicit RemoveChildrenRepresentations( App::Component::Chemistry::Category & p_category ) : _categories { &p_category }
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}
	//	explicit RemoveChildrenRepresentations( const std::unordered_set<App::Component::Chemistry::Category *> & p_categories ) :
	//		_categories( p_categories )
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}

	//	virtual void execute() override;

	//  private:
	//	const std::unordered_set<App::Component::Chemistry::Category *> _categories;
	//};

	// class Delete : public App::Core::Action::BaseAction
	//{
	//  public:
	//	explicit Delete( App::Component::Chemistry::Category & p_category ) : _category( p_category )
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}

	//	virtual void execute() override;

	//  private:
	//	App::Component::Chemistry::Category & _category;
	//};

	// class Copy : public App::Core::Action::BaseAction
	//{
	//  public:
	//	explicit Copy( const App::Component::Chemistry::Category & p_target ) : _target( p_target )
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}
	//	virtual void execute() override;

	//  private:
	//	const App::Component::Chemistry::Category & _target;
	//};

	// class Extract : public App::Core::Action::BaseAction
	//{
	//  public:
	//	explicit Extract( const App::Component::Chemistry::Category & p_target ) : _target( p_target )
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}
	//	virtual void execute() override;

	//  private:
	//	const App::Component::Chemistry::Category & _target;
	//};

	// class ApplyRepresentation : public App::Core::Action::BaseAction
	//{
	//  public:
	//	explicit ApplyRepresentation( const std::unordered_set<App::Component::Chemistry::Category *> &				p_categories,
	//								  const Model::Representation::InstantiatedRepresentation & p_source,
	//								  const Model::Representation::MEMBER_FLAG &				p_flag ) :
	//		_representation( p_source ),
	//		_flag( p_flag ), _categories( p_categories )
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}

	//	virtual void execute() override;

	//  private:
	//	const std::unordered_set<App::Component::Chemistry::Category *>				  _categories = std::unordered_set<App::Component::Chemistry::Category *>();
	//	const Model::Representation::InstantiatedRepresentation & _representation;
	//	const Model::Representation::MEMBER_FLAG				  _flag;
	//};

} // namespace VTX::App::Action::Category
#endif
