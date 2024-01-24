#include "ui/qt/mode/visualization.hpp"
#include "ui/qt/controller/base_camera_controller.hpp"
#include "ui/qt/controller/base_controller.hpp"
#include "ui/qt/controller/base_picker_controller.hpp"
#include "ui/qt/controller/controller_manager.hpp"
#include "ui/qt/controller/freefly.hpp"
// #include "ui/qt/controller/global_shorcuts.hpp"
#include "ui/qt/controller/selection_picker.hpp"
#include "ui/qt/controller/trackball.hpp"
// #include "ui/qt/controller/visualization_shorcuts.hpp"

namespace VTX::UI::QT::Mode
{
	Visualization::Visualization() : BaseMode()
	{
		// TODO replace that with an initialisation with custom data.
		// addCameraController( CONTROLLER_MANAGER().instantiateController<Controller::Trackball>( "TRACKBALL" ) );

		std::unique_ptr<Controller::BaseController> ptr
			= Controller::ControllerCollection::get().instantiateItem( Controller::Freefly::HASHED_COLLECTION_ID );

		addCameraController( ptr );

		// addPickerController( Controller::ControllerCollection::get().instantiateItem<Controller::SelectionPicker>(
		// "SELECTION_PICKER" ) );

		// addController( Controller::ControllerCollection::get().instantiateItem( "GLOBAL_SHORTCUTS" ) );
		// addController( Controller::ControllerCollection::get().instantiateItem( "VISUALIZATION_SHORTCUTS" ) );

		_currentCameraController
			= dynamic_cast<Controller::BaseCameraController *>( _cameraControllers.begin()->get() );
	}

	// void Visualization::init( VisualizationData & p_data )
	//{
	//	while ( p_data.cameraControllers.size() > 0 )
	//	{
	//		std::unique_ptr<Controller::BaseCameraController> & controllerPtr
	//			= p_data.cameraControllers.extract( p_data.cameraControllers.begin() ).value();

	//		addCameraController( controllerPtr );
	//	}
	//	while ( p_data.pickerControllers.size() > 0 )
	//	{
	//		std::unique_ptr<Controller::BasePickerController> & controllerPtr
	//			= p_data.pickerControllers.extract( p_data.pickerControllers.begin() ).value();

	//		addPickerController( controllerPtr );
	//	}
	//	while ( p_data.cameraControllers.size() > 0 )
	//	{
	//		std::unique_ptr<Controller::BaseController> & controllerPtr
	//			= p_data.otherControllers.extract( p_data.otherControllers.begin() ).value();

	//		addController( controllerPtr );
	//	}

	//	_currentCameraController = p_data.cameraControllers.begin()->get();
	//	_currentPickerController = p_data.pickerControllers.begin()->get();
	//}

	void Visualization::enter()
	{
		BaseMode::enter();

		if ( _currentCameraController != nullptr )
			getCurrentCameraController().setActive( true );
		if ( _currentPickerController != nullptr )
			getCurrentPickerController().setActive( true );
	}
	void Visualization::exit()
	{
		BaseMode::exit();

		getCurrentCameraController().setActive( false );
		getCurrentPickerController().setActive( false );
	}

	void Visualization::addCameraController( std::unique_ptr<Controller::BaseController> & p_cameraControllerPtr )
	{
		assert( dynamic_cast<Controller::BaseCameraController *>( p_cameraControllerPtr.get() ) != nullptr );
		_cameraControllers.emplace( std::move( p_cameraControllerPtr ) );
	}
	void Visualization::addPickerController( std::unique_ptr<Controller::BaseController> & p_pickerControllerPtr )
	{
		assert( dynamic_cast<Controller::BasePickerController *>( p_pickerControllerPtr.get() ) != nullptr );
		_pickerControllers.emplace( std::move( p_pickerControllerPtr ) );
	}
	void Visualization::addController( std::unique_ptr<Controller::BaseController> & p_controllerPtr )
	{
		_otherControllers.emplace( std::move( p_controllerPtr ) );
	}

	Controller::BaseController & Visualization::getController( const App::Core::CollectionKey & p_controllerKey )
	{
		const auto controllerIt = std::find_if(
			_otherControllers.begin(),
			_otherControllers.end(),
			[ p_controllerKey ]( const std::unique_ptr<Controller::BaseController> & p_controllerPtr )
			{ return p_controllerPtr->getHashedCollectionID() == Util::Hashing::hash( p_controllerKey ); }
		);
		return *( *controllerIt );
	}

	void Visualization::setCameraController( const App::Core::CollectionKey & p_controllerKey )
	{
		if ( !getCurrentCameraController().canBeStopped() )
			return;

		const auto newControllerIt = std::find_if(
			_cameraControllers.begin(),
			_cameraControllers.end(),
			[ p_controllerKey ]( const std::unique_ptr<Controller::BaseController> & p_controllerPtr )
			{ return p_controllerPtr->getHashedCollectionID() == Util::Hashing::hash( p_controllerKey ); }
		);

		// Do nothing if id not in collection or already in use
		if ( newControllerIt == _cameraControllers.end() || newControllerIt->get() == _currentCameraController )
			return;

		getCurrentCameraController().setActive( false );
		_currentCameraController = dynamic_cast<Controller::BaseCameraController *>( newControllerIt->get() );
		getCurrentCameraController().setActive( true );

		onCameraControllerChange.call( *_currentCameraController );
	}

	void Visualization::setPickerController( const App::Core::CollectionKey & p_controllerKey )
	{
		const auto newControllerIt = std::find_if(
			_pickerControllers.begin(),
			_pickerControllers.end(),
			[ p_controllerKey ]( const std::unique_ptr<Controller::BaseController> & p_controllerPtr )
			{ return p_controllerPtr->getHashedCollectionID() == Util::Hashing::hash( p_controllerKey ); }
		);

		// Do nothing if id not in collection or already in use
		if ( newControllerIt == _pickerControllers.end() || newControllerIt->get() == _currentPickerController )
			return;

		getCurrentPickerController().setActive( false );
		_currentPickerController = dynamic_cast<Controller::BasePickerController *>( newControllerIt->get() );
		getCurrentPickerController().setActive( true );

		onPickerControllerChange.call( *_currentPickerController );
	}

	void Visualization::resetCameraController()
	{
		//// Reset camera.
		// App::Old::Component::Render::Camera & camera = App::Old::VTXApp::get().getScene().getCamera();

		// const Vec3f defaultPos
		//	= App::Old::VTXApp::get().getScene().getAABB().centroid()
		//	  - App::Old::Component::CAMERA_FRONT_DEFAULT * App::Old::VTXApp::get().getScene().getAABB().radius()
		//			/ (float)( tan( Util::Math::radians( camera.getFov() ) * UI::Style::ORIENT_ZOOM_FACTOR ) );
		// camera.reset( defaultPos );

		//// Reset controllers.
		// getController<Controller::Trackball>( UI::ID::Controller::TRACKBALL )->reset();
		// getController<Controller::Freefly>( UI::ID::Controller::FREEFLY )->reset();
	}

	// void Visualization::orientCameraController( const App::Old::Component::Object3D::Helper::AABB & p_aabb )
	//{
	//	getController<VTX::Controller::BaseCameraController>( _cameraController )->orient( p_aabb );

	//	// Override Trackball distance.
	//	if ( _cameraController == UI::ID::Controller::FREEFLY )
	//	{
	//		const Controller::Freefly * const freefly
	//			= getController<Controller::Freefly>( UI::ID::Controller::FREEFLY );
	//		if ( freefly->isOrienting() )
	//		{
	//			getController<Controller::Trackball>( UI::ID::Controller::TRACKBALL )
	//				->setDistanceForced( Util::Math::distance( p_aabb.centroid(), freefly->getOrientTargetPosition() )
	//				);
	//		}
	//	}
	//}
	// void Visualization::orientCameraController( const Vec3f & p_position, const Quatf & p_orientation )
	//{
	//	getController<VTX::Controller::BaseCameraController>( _cameraController )->orient( p_position, p_orientation );

	//	// Override Trackball distance.
	//	if ( _cameraController == UI::ID::Controller::FREEFLY )
	//	{
	//		const Controller::Freefly * const freefly
	//			= getController<Controller::Freefly>( UI::ID::Controller::FREEFLY );
	//		if ( freefly->isOrienting() )
	//		{
	//			getController<Controller::Trackball>( UI::ID::Controller::TRACKBALL )
	//				->setDistanceForced( Util::Math::distance( p_position, freefly->getOrientTargetPosition() ) );
	//		}
	//	}
	//}

	// void Visualization::receiveEvent( const VTX::App::Old::Core::Event::VTXEvent & p_event )
	//{
	//	// Recenter when add the first element in scene
	//	if ( p_event.name == VTX::App::Old::Event::Global::MOLECULE_ADDED )
	//	{
	//		if ( App::Old::VTXApp::get().getScene().getMolecules().size() == 1
	//			 && App::Old::VTXApp::get().getScene().getMeshes().size() == 0 )
	//		{
	//			const VTX::App::Old::Core::Event::VTXEventArg<App::Old::Component::Chemistry::Molecule *> & castedEvent
	//				= dynamic_cast<
	//					const VTX::App::Old::Core::Event::VTXEventArg<App::Old::Component::Chemistry::Molecule *> &>(
	//					p_event
	//				);

	//			orientCameraController( castedEvent.get()->getWorldAABB() );
	//		}
	//	}
	//	else if ( p_event.name == VTX::App::Old::Event::Global::MESH_ADDED )
	//	{
	//		if ( App::Old::VTXApp::get().getScene().getMolecules().size() == 0
	//			 && App::Old::VTXApp::get().getScene().getMeshes().size() == 1 )
	//			resetCameraController();
	//	}
	//}

} // namespace VTX::UI::QT::Mode
