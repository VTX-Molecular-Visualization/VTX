#ifndef __VTX_ACTION_SCENE__
#define __VTX_ACTION_SCENE__

#include "action/action_manager.hpp"
#include "base_action.hpp"
#include "generic/base_scene_item.hpp"
#include "object3d/scene.hpp"
#include "util/molecule.hpp"
#include "vtx_app.hpp"

namespace VTX::Action::Scene
{
	class ResetScene : public BaseAction
	{
	  public:
		virtual void execute() override { VTXApp::get().getScene().reset(); }
	};

	class ChangeItemIndex : public BaseAction
	{
	  public:
		explicit ChangeItemIndex( const Generic::BaseSceneItem & p_item, const int p_position ) :
			_item( p_item ), _position( p_position )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override { VTXApp::get().getScene().changeModelPosition( _item, _position ); }

	  private:
		const Generic::BaseSceneItem & _item;
		const int					   _position;
	};

	class ShowAllMolecules : public BaseAction
	{
	  public:
		explicit ShowAllMolecules() { _tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE ); }
		virtual void execute() override
		{
			for ( Object3D::Scene::PairMoleculePtrFloat & pairMol : VTXApp::get().getScene().getMolecules() )
			{
				Util::Molecule::show( *pairMol.first, true, true );
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}
	};
} // namespace VTX::Action::Scene
#endif
