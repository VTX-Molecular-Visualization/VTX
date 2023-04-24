#ifndef __VTX_ACTION_CATEGORY__
#define __VTX_ACTION_CATEGORY__

#include "app/action/visible.hpp"
#include "app/core/action/base_action.hpp"
#include "app/old_app/model/category.hpp"
// #include "app/old_app/model/representation/instantiated_representation.hpp"
// #include <unordered_set>

namespace VTX::Action::Category
{
	// class ChangeColor : public Core::Action::BaseAction
	//{
	//  public:
	//	explicit ChangeColor( Model::Category & p_category, const Color::Rgb & p_color ) :
	//		_color( p_color ), _categories { &p_category }
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}
	//	explicit ChangeColor( const std::unordered_set<Model::Category *> & p_categories, const Color::Rgb & p_color ) :
	//		_color( p_color ), _categories( p_categories )
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}

	//	virtual void execute() override;

	//  private:
	//	const std::unordered_set<Model::Category *> _categories = std::unordered_set<Model::Category *>();
	//	const Color::Rgb						 _color;
	//};

	class ChangeVisibility : public Visible::ChangeVisibility
	{
	  public:
		explicit ChangeVisibility( Model::Category & p_category, const VISIBILITY_MODE p_mode ) :
			Visible::ChangeVisibility( p_category, p_mode )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;
	};

	// class ChangeRepresentationPreset : public Core::Action::BaseAction
	//{
	//  public:
	//	explicit ChangeRepresentationPreset( Model::Category & p_category, const int p_indexPreset ) :
	//		_indexPreset( p_indexPreset ), _categories { &p_category }
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}
	//	explicit ChangeRepresentationPreset( const std::unordered_set<Model::Category *> & p_categories,
	//										 const int									p_indexPreset ) :
	//		_indexPreset( p_indexPreset ),
	//		_categories( p_categories )
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}

	//	virtual void execute() override;

	//  private:
	//	const std::unordered_set<Model::Category *> _categories = std::unordered_set<Model::Category *>();
	//	const int								 _indexPreset;
	//};

	// class RemoveRepresentation : public Core::Action::BaseAction
	//{
	//  public:
	//	explicit RemoveRepresentation( Model::Category & p_category ) : _categories { &p_category }
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}
	//	explicit RemoveRepresentation( const std::unordered_set<Model::Category *> & p_categories ) : _categories(
	// p_categories )
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}

	//	virtual void execute() override;

	//  private:
	//	const std::unordered_set<Model::Category *> _categories;
	//};

	// class RemoveChildrenRepresentations : public Core::Action::BaseAction
	//{
	//  public:
	//	explicit RemoveChildrenRepresentations( Model::Category & p_category ) : _categories { &p_category }
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}
	//	explicit RemoveChildrenRepresentations( const std::unordered_set<Model::Category *> & p_categories ) :
	//		_categories( p_categories )
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}

	//	virtual void execute() override;

	//  private:
	//	const std::unordered_set<Model::Category *> _categories;
	//};

	// class Delete : public Core::Action::BaseAction
	//{
	//  public:
	//	explicit Delete( Model::Category & p_category ) : _category( p_category )
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}

	//	virtual void execute() override;

	//  private:
	//	Model::Category & _category;
	//};

	// class Copy : public Core::Action::BaseAction
	//{
	//  public:
	//	explicit Copy( const Model::Category & p_target ) : _target( p_target )
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}
	//	virtual void execute() override;

	//  private:
	//	const Model::Category & _target;
	//};

	// class Extract : public Core::Action::BaseAction
	//{
	//  public:
	//	explicit Extract( const Model::Category & p_target ) : _target( p_target )
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}
	//	virtual void execute() override;

	//  private:
	//	const Model::Category & _target;
	//};

	// class ApplyRepresentation : public Core::Action::BaseAction
	//{
	//  public:
	//	explicit ApplyRepresentation( const std::unordered_set<Model::Category *> &				p_categories,
	//								  const Model::Representation::InstantiatedRepresentation & p_source,
	//								  const Model::Representation::MEMBER_FLAG &				p_flag ) :
	//		_representation( p_source ),
	//		_flag( p_flag ), _categories( p_categories )
	//	{
	//		_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
	//	}

	//	virtual void execute() override;

	//  private:
	//	const std::unordered_set<Model::Category *>				  _categories = std::unordered_set<Model::Category *>();
	//	const Model::Representation::InstantiatedRepresentation & _representation;
	//	const Model::Representation::MEMBER_FLAG				  _flag;
	//};

} // namespace VTX::Action::Category
#endif
