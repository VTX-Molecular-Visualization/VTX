#ifndef __VTX_APP_ACTION_SCENE__
#define __VTX_APP_ACTION_SCENE__

#include "app/action.hpp"
#include "app/core/action/base_action.hpp"
#include "app/core/scene/base_scene_item.hpp"
#include <vector>

namespace VTX::App::Action::Scene
{
	class ResetScene : public App::Core::Action::BaseAction
	{
	  public:
		virtual void execute() override;
	};

	class ChangeItemIndex : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeItemIndex( const App::Core::Scene::BaseSceneItem & p_item, const int p_position ) :
			ChangeItemIndex( { &p_item }, p_position )
		{
		}
		explicit ChangeItemIndex( const std::vector<const App::Core::Scene::BaseSceneItem *> & p_items, const int p_position ) :
			_items( p_items ), _position( p_position )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		const std::vector<const App::Core::Scene::BaseSceneItem *> _items;
		const int										  _position;
	};

	class ShowAllMolecules : public App::Core::Action::BaseAction
	{
	  public:
		explicit ShowAllMolecules()
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		virtual void execute() override;
	};
} // namespace VTX::App::Action::Scene
#endif
