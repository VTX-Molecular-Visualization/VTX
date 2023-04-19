#ifndef __VTX_ACTION_SCENE__
#define __VTX_ACTION_SCENE__

#include "app/core/action/action_manager.hpp"
#include "app/core/action/base_action.hpp"
#include "app/old_app/generic/base_scene_item.hpp"
#include "app/old_app/object3d/scene.hpp"
#include "app/old_app/util/molecule.hpp"
#include "app/old_app/vtx_app.hpp"
#include <vector>

namespace VTX::Action::Scene
{
	class ResetScene : public Core::Action::BaseAction
	{
	  public:
		virtual void execute() override { VTXApp::get().getScene().reset(); }
	};

	class ChangeItemIndex : public Core::Action::BaseAction
	{
	  public:
		explicit ChangeItemIndex( const Generic::BaseSceneItem & p_item, const int p_position ) :
			ChangeItemIndex( { &p_item }, p_position )
		{
		}
		explicit ChangeItemIndex( const std::vector<const Generic::BaseSceneItem *> & p_items, const int p_position ) :
			_items( p_items ), _position( p_position )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override { VTXApp::get().getScene().changeModelsPosition( _items, _position ); }

	  private:
		const std::vector<const Generic::BaseSceneItem *> _items;
		const int										  _position;
	};

	class ShowAllMolecules : public Core::Action::BaseAction
	{
	  public:
		explicit ShowAllMolecules()
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		virtual void execute() override
		{
			for ( Object3D::Scene::PairMoleculePtrFloat & pairMol : VTXApp::get().getScene().getMolecules() )
			{
				Util::Molecule::show( *pairMol.first, true, true, true );
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}
	};
} // namespace VTX::Action::Scene
#endif
