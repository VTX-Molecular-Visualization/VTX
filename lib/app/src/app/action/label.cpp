#include "app/action/label.hpp"
#include "app/mvc.hpp"
#include "app/model/selection.hpp"
#include "app/old_app/object3d/scene.hpp"
#include "app/old_app/selection/selection_manager.hpp"
#include "app/old_app/vtx_app.hpp"

namespace VTX::App::Action::Label
{
	void SetEnable::execute()
	{
		for ( Model::Label * label : _labels )
			label->setEnable( _enabled );
	}

	void Delete::execute()
	{
		VTX::Selection::SelectionManager::get().getSelectionModel().unselectModels<Model::Label>( _labels );

		for ( Model::Label * label : _labels )
		{
			VTXApp::get().getScene().removeLabel( label );
			VTX::MVC_MANAGER().deleteModel<Model::Label>( label );
		}
	}

	void EnableAutoNaming::execute()
	{
		for ( Model::Label * const label : _labels )
			label->setAutoNaming( _enable );
	}

	void Rename::execute()
	{
		for ( Model::Label * const label : _labels )
		{
			label->setAutoNaming( false );
			label->setName( _name );
		}
	}

	void ChangeColor::execute()
	{
		for ( Model::Label * const label : _labels )
		{
			label->setColor( _color );
		}
	}

} // namespace VTX::App::Action::Label
