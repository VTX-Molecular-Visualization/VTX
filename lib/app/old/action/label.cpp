#include "app/old/action/label.hpp"
#include "app/old/mvc.hpp"
#include "app/old/application/selection/selection.hpp"
#include "app/old/application/scene.hpp"
#include "app/old/application/selection/selection_manager.hpp"
#include "app/old/vtx_app.hpp"

namespace VTX::App::Old::Action::Label
{
	void SetEnable::execute()
	{
		for ( App::Old::Component::Object3D::Label * label : _labels )
			label->setEnable( _enabled );
	}

	void Delete::execute()
	{
		VTX::App::Old::Application::Selection::SelectionManager::get().getSelectionModel().unselectModels<App::Old::Component::Object3D::Label>( _labels );

		for ( App::Old::Component::Object3D::Label * label : _labels )
		{
			App::Old::VTXApp::get().getScene().removeLabel( label );
			VTX::MVC_MANAGER().deleteModel<App::Old::Component::Object3D::Label>( label );
		}
	}

	void EnableAutoNaming::execute()
	{
		for ( App::Old::Component::Object3D::Label * const label : _labels )
			label->setAutoNaming( _enable );
	}

	void Rename::execute()
	{
		for ( App::Old::Component::Object3D::Label * const label : _labels )
		{
			label->setAutoNaming( false );
			label->setName( _name );
		}
	}

	void ChangeColor::execute()
	{
		for ( App::Old::Component::Object3D::Label * const label : _labels )
		{
			label->setColor( _color );
		}
	}

} // namespace VTX::App::Old::Action::Label
