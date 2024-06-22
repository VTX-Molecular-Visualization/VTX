#include "app/mode/visualization.hpp"
#include "app/core/animation/helper.hpp"
#include "app/core/controller/base_camera_controller.hpp"
#include "app/core/controller/base_controller.hpp"
#include "app/core/controller/base_picker_controller.hpp"
#include <app/application/scene.hpp>
#include <app/vtx_app.hpp>

namespace VTX::App::Mode
{
	Visualization::Visualization() : BaseMode() {}

	void Visualization::enter()
	{
		BaseMode::enter();

		if ( _currentCameraController != nullptr )
			_currentCameraController->setActive( true );

		if ( _currentPickerController != nullptr )
			_currentPickerController->setActive( true );

		for ( const std::unique_ptr<Core::Controller::BaseController> & controller : _otherControllers )
		{
			controller->setActive( true );
		}

		APP().onUpdate += [ this ]( float p_deltaTime ) { update( p_deltaTime ); };
	}

	void Visualization::update( float p_deltaTime )
	{
		if ( _currentCameraController != nullptr )
			_currentCameraController->update( p_deltaTime );

		if ( _currentPickerController != nullptr )
			_currentPickerController->update( p_deltaTime );

		for ( const std::unique_ptr<Core::Controller::BaseController> & controller : _otherControllers )
		{
			if ( controller->isActive() )
			{
				controller->update( true );
			}
		}
	}

	void Visualization::exit()
	{
		BaseMode::exit();

		if ( _currentCameraController != nullptr )
			_currentCameraController->setActive( false );

		if ( _currentPickerController != nullptr )
			_currentPickerController->setActive( false );

		for ( const std::unique_ptr<Core::Controller::BaseController> & controller : _otherControllers )
		{
			controller->setActive( false );
		}

		// TODO: remove the callback from the app?
		// APP().onUpdate().removeCallback( this );
	}

	void Visualization::addCameraController( std::unique_ptr<Core::Controller::BaseController> & p_cameraControllerPtr )
	{
		assert( dynamic_cast<Core::Controller::BaseCameraController *>( p_cameraControllerPtr.get() ) != nullptr );
		_cameraControllers.emplace( std::move( p_cameraControllerPtr ) );

		if ( _currentCameraController == nullptr )
		{
			_affectCameraController(
				dynamic_cast<Core::Controller::BaseCameraController *>( _cameraControllers.begin()->get() )
			);
		}
	}
	void Visualization::addPickerController( std::unique_ptr<Core::Controller::BaseController> & p_pickerControllerPtr )
	{
		assert( dynamic_cast<Core::Controller::BasePickerController *>( p_pickerControllerPtr.get() ) != nullptr );
		_pickerControllers.emplace( std::move( p_pickerControllerPtr ) );

		if ( _currentPickerController == nullptr )
		{
			_affectPickerController(
				dynamic_cast<Core::Controller::BasePickerController *>( _pickerControllers.begin()->get() )
			);
		}
	}
	void Visualization::addController( std::unique_ptr<Core::Controller::BaseController> & p_controllerPtr )
	{
		_otherControllers.emplace( std::move( p_controllerPtr ) );
	}

	Core::Controller::BaseController & Visualization::getController( const App::Core::CollectionKey & p_controllerKey )
	{
		const auto controllerIt = std::find_if(
			_otherControllers.begin(),
			_otherControllers.end(),
			[ p_controllerKey ]( const std::unique_ptr<Core::Controller::BaseController> & p_controllerPtr )
			{ return p_controllerPtr->getHashedCollectionID() == Util::Hashing::hash( p_controllerKey ); }
		);
		return *( *controllerIt );
	}

	void Visualization::setCameraController( const Util::Hashing::Hash & p_controllerHash )
	{
		if ( !getCurrentCameraController().canBeStopped() )
			return;

		const auto newControllerIt = std::find_if(
			_cameraControllers.begin(),
			_cameraControllers.end(),
			[ p_controllerHash ]( const std::unique_ptr<Core::Controller::BaseController> & p_controllerPtr )
			{ return p_controllerPtr->getHashedCollectionID() == p_controllerHash; }
		);

		// Do nothing if id not in collection or already in use
		if ( newControllerIt == _cameraControllers.end() || newControllerIt->get() == _currentCameraController )
			return;

		getCurrentCameraController().setActive( false );
		_affectCameraController( dynamic_cast<Core::Controller::BaseCameraController *>( newControllerIt->get() ) );

		onCameraController( *_currentCameraController );
	}
	void Visualization::setCameraController( const App::Core::CollectionKey & p_controllerKey )
	{
		setCameraController( Util::Hashing::hash( p_controllerKey ) );
	}

	void Visualization::setPickerController( const Util::Hashing::Hash & p_controllerHash )
	{
		const auto newControllerIt = std::find_if(
			_pickerControllers.begin(),
			_pickerControllers.end(),
			[ p_controllerHash ]( const std::unique_ptr<Core::Controller::BaseController> & p_controllerPtr )
			{ return p_controllerPtr->getHashedCollectionID() == p_controllerHash; }
		);

		// Do nothing if id not in collection or already in use
		if ( newControllerIt == _pickerControllers.end() || newControllerIt->get() == _currentPickerController )
			return;

		getCurrentPickerController().setActive( false );
		_currentPickerController = dynamic_cast<Core::Controller::BasePickerController *>( newControllerIt->get() );
		getCurrentPickerController().setActive( true );

		onPickerController( *_currentPickerController );
	}
	void Visualization::setPickerController( const App::Core::CollectionKey & p_controllerKey )
	{
		setPickerController( Util::Hashing::hash( p_controllerKey ) );
	}

	void Visualization::_affectCameraController( Core::Controller::BaseCameraController * p_ptr )
	{
		_currentCameraController = p_ptr;
		_currentCameraController->setCamera( App::SCENE().getCamera() );
		_currentCameraController->setActive( true );
	}
	void Visualization::_affectPickerController( Core::Controller::BasePickerController * p_ptr )
	{
		_currentPickerController = p_ptr;
		_currentPickerController->setActive( true );
	}

} // namespace VTX::App::Mode
