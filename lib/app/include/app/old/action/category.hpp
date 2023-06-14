#ifndef __VTX_APP_OLD_ACTION_CATEGORY__
#define __VTX_APP_OLD_ACTION_CATEGORY__

#include "app/old/action.hpp"
#include "app/old/action/visible.hpp"
#include "app/old/core/action/base_action.hpp"
#include "app/old/component/chemistry/category.hpp"
// #include "app/old/application/representation/instantiated_representation.hpp"
// #include <unordered_set>

namespace VTX::App::Old::Action::Category
{
	// class ChangeColor : public App::Old::Core::Action::BaseAction
	//{
	//  public:
	//	explicit ChangeColor( App::Old::Component::Chemistry::Category & p_category, const Color::Rgb & p_color ) :
	//		_color( p_color ), _categories { &p_category }
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}
	//	explicit ChangeColor( const std::unordered_set<App::Old::Component::Chemistry::Category *> & p_categories, const Color::Rgb & p_color ) :
	//		_color( p_color ), _categories( p_categories )
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}

	//	virtual void execute() override;

	//  private:
	//	const std::unordered_set<App::Old::Component::Chemistry::Category *> _categories = std::unordered_set<App::Old::Component::Chemistry::Category *>();
	//	const Color::Rgb						 _color;
	//};

	class ChangeVisibility : public Visible::ChangeVisibility
	{
	  public:
		explicit ChangeVisibility( App::Old::Component::Chemistry::Category & p_category, const VISIBILITY_MODE p_mode ) :
			Visible::ChangeVisibility( p_category, p_mode )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;
	};

	// class ChangeRepresentationPreset : public App::Old::Core::Action::BaseAction
	//{
	//  public:
	//	explicit ChangeRepresentationPreset( App::Old::Component::Chemistry::Category & p_category, const int p_indexPreset ) :
	//		_indexPreset( p_indexPreset ), _categories { &p_category }
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}
	//	explicit ChangeRepresentationPreset( const std::unordered_set<App::Old::Component::Chemistry::Category *> & p_categories,
	//										 const int									p_indexPreset ) :
	//		_indexPreset( p_indexPreset ),
	//		_categories( p_categories )
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}

	//	virtual void execute() override;

	//  private:
	//	const std::unordered_set<App::Old::Component::Chemistry::Category *> _categories = std::unordered_set<App::Old::Component::Chemistry::Category *>();
	//	const int								 _indexPreset;
	//};

	// class RemoveRepresentation : public App::Old::Core::Action::BaseAction
	//{
	//  public:
	//	explicit RemoveRepresentation( App::Old::Component::Chemistry::Category & p_category ) : _categories { &p_category }
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}
	//	explicit RemoveRepresentation( const std::unordered_set<App::Old::Component::Chemistry::Category *> & p_categories ) : _categories(
	// p_categories )
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}

	//	virtual void execute() override;

	//  private:
	//	const std::unordered_set<App::Old::Component::Chemistry::Category *> _categories;
	//};

	// class RemoveChildrenRepresentations : public App::Old::Core::Action::BaseAction
	//{
	//  public:
	//	explicit RemoveChildrenRepresentations( App::Old::Component::Chemistry::Category & p_category ) : _categories { &p_category }
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}
	//	explicit RemoveChildrenRepresentations( const std::unordered_set<App::Old::Component::Chemistry::Category *> & p_categories ) :
	//		_categories( p_categories )
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}

	//	virtual void execute() override;

	//  private:
	//	const std::unordered_set<App::Old::Component::Chemistry::Category *> _categories;
	//};

	// class Delete : public App::Old::Core::Action::BaseAction
	//{
	//  public:
	//	explicit Delete( App::Old::Component::Chemistry::Category & p_category ) : _category( p_category )
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}

	//	virtual void execute() override;

	//  private:
	//	App::Old::Component::Chemistry::Category & _category;
	//};

	// class Copy : public App::Old::Core::Action::BaseAction
	//{
	//  public:
	//	explicit Copy( const App::Old::Component::Chemistry::Category & p_target ) : _target( p_target )
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}
	//	virtual void execute() override;

	//  private:
	//	const App::Old::Component::Chemistry::Category & _target;
	//};

	// class Extract : public App::Old::Core::Action::BaseAction
	//{
	//  public:
	//	explicit Extract( const App::Old::Component::Chemistry::Category & p_target ) : _target( p_target )
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}
	//	virtual void execute() override;

	//  private:
	//	const App::Old::Component::Chemistry::Category & _target;
	//};

	// class ApplyRepresentation : public App::Old::Core::Action::BaseAction
	//{
	//  public:
	//	explicit ApplyRepresentation( const std::unordered_set<App::Old::Component::Chemistry::Category *> &				p_categories,
	//								  const App::Old::Application::Representation::InstantiatedRepresentation & p_source,
	//								  const Application::Representation::MEMBER_FLAG &				p_flag ) :
	//		_representation( p_source ),
	//		_flag( p_flag ), _categories( p_categories )
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}

	//	virtual void execute() override;

	//  private:
	//	const std::unordered_set<App::Old::Component::Chemistry::Category *>				  _categories = std::unordered_set<App::Old::Component::Chemistry::Category *>();
	//	const App::Old::Application::Representation::InstantiatedRepresentation & _representation;
	//	const Application::Representation::MEMBER_FLAG				  _flag;
	//};

} // namespace VTX::App::Old::Action::Category
#endif
