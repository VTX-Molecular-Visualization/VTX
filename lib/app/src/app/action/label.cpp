#include "app/action/label.hpp"
#include "app/mvc.hpp"
#include "app/application/selection/selection.hpp"
#include "app/application/scene.hpp"
#include "app/application/selection/selection_manager.hpp"
#include "app/old_app/vtx_app.hpp"

namespace VTX::App::Action::Label
{
	void SetEnable::execute()
	{
		for ( App::Component::Object3D::Label * label : _labels )
			label->setEnable( _enabled );
	}

	void Delete::execute()
	{
		VTX::App::Application::Selection::SelectionManager::get().getSelectionModel().unselectModels<App::Component::Object3D::Label>( _labels );

		for ( App::Component::Object3D::Label * label : _labels )
		{
			VTXApp::get().getScene().removeLabel( label );
			VTX::MVC_MANAGER().deleteModel<App::Component::Object3D::Label>( label );
		}
	}

	void EnableAutoNaming::execute()
	{
		for ( App::Component::Object3D::Label * const label : _labels )
			label->setAutoNaming( _enable );
	}

	void Rename::execute()
	{
		for ( App::Component::Object3D::Label * const label : _labels )
		{
			label->setAutoNaming( false );
			label->setName( _name );
		}
	}

	void ChangeColor::execute()
	{
		for ( App::Component::Object3D::Label * const label : _labels )
		{
			label->setColor( _color );
		}
	}

} // namespace VTX::App::Action::Label
