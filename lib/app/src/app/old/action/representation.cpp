#include "app/old/action/representation.hpp"
#include "app/old/application/representation/representation_manager.hpp"
#include "app/old/application/setting.hpp"
#include "app/old/internal/io/filesystem.hpp"
#include "app/old/internal/worker/representation_loader.hpp"
#include "app/old/internal/worker/representation_saver.hpp"
#include "app/old/mvc.hpp"
#include "app/old/vtx_app.hpp"
#include "app/old/worker.hpp"
#include <filesystem>
#include <util/filesystem.hpp>

namespace VTX::App::Old::Action::Representation
{
	void ReloadPresets::execute()
	{
		Internal::Worker::RepresentationLibraryLoader * libraryLoader
			= new Internal::Worker::RepresentationLibraryLoader(
				App::Old::Application::Representation::RepresentationLibrary::get() );
		VTX_WORKER( libraryLoader );
	}

	void ResetPresetsToDefault::execute()
	{
		App::Old::Application::Representation::RepresentationManager::get().resetRepresentations();
	}

	void SavePreset::execute()
	{
		if ( _clearDirectory )
		{
			Util::Filesystem::removeAll( App::Old::Internal::IO::Filesystem::getRepresentationsLibraryDir() );
			std::filesystem::create_directory( App::Old::Internal::IO::Filesystem::getRepresentationsLibraryDir() );
		}

		for ( const App::Old::Application::Representation::RepresentationPreset * const representation : _representations )
		{
			// Don't think it's a good idea to run a thread at app exit.
			/*
			if ( _async )
			{
				Worker::RepresentationSaverThread * librarySaver
					= new Worker::RepresentationSaverThread( representation );
				Worker::Callback * callback = new Worker::Callback( [ librarySaver ]( const uint p_code ) {} );

				VTX_WORKER( librarySaver, callback );
			}
			else
			*/
			{
				FilePath path = App::Old::Internal::IO::Filesystem::getRepresentationPath( representation->getName() );
				Util::Filesystem::generateUniqueFileName( path );

				Internal::Worker::RepresentationSaver * librarySaver
					= new Internal::Worker::RepresentationSaver( representation, path );

				VTX_WORKER( librarySaver );
			}
		}
	}

	void ChangeName::execute() { _representation->setName( _name ); }

	void ChangeQuickAccess::execute()
	{
		App::Old::Application::Representation::RepresentationManager::get().setQuickAccessToPreset( _representation,
																							   _quickAccess );
	}

	void ChangeRepresentation::execute()
	{
		_representation->changeRepresentationType( _representationType );
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeColorMode::execute()
	{
		_representation->getData().setColorMode( _colorMode );
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeColor::execute()
	{
		_representation->setColor( _color );
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_UNIFORM_UPDATED;
	}

	void ChangeSphereRadius::execute()
	{
		_representation->getData().setSphereRadius( _radius );
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_UNIFORM_UPDATED;
	}

	void ChangeCylinderRadius::execute()
	{
		_representation->getData().setCylinderRadius( _radius );
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_UNIFORM_UPDATED;
	}

	void ChangeCylinderColorBendingMode::execute()
	{
		_representation->getData().setCylinderColorBlendingMode( _colorBendingMode );
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_UNIFORM_UPDATED;
	}

	void ChangeRibbonColorMode::execute()
	{
		_representation->getData().setRibbonColorMode( _colorMode );
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeRibbonColorBendingMode::execute()
	{
		_representation->getData().setRibbonColorBlendingMode( _colorBendingMode );
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_UNIFORM_UPDATED;
	}

	void AddNewPresetInLibrary::execute()
	{
		App::Old::Application::Representation::RepresentationPreset * const newRepresentation
			= VTX::MVC_MANAGER().instantiateModel<App::Old::Application::Representation::RepresentationPreset>(
				VTX::App::Old::Application::Setting::DEFAULT_REPRESENTATION_TYPE );

		newRepresentation->setName( _representationName );
		App::Old::Application::Representation::RepresentationLibrary::get().addRepresentation( newRepresentation );
	}

	void CopyPresetInLibrary::execute()
	{
		App::Old::Application::Representation::RepresentationLibrary::get().copyRepresentation( _representationIndex );
	}

	void DeletePresetInLibrary::execute()
	{
		const App::Old::Application::Representation::RepresentationPreset * representation
			= App::Old::Application::Representation::RepresentationLibrary::get().getRepresentation( _representationIndex );

		App::Old::Application::Representation::RepresentationManager::get().deleteRepresentation( representation );

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_NEED_UPDATE;
	}

	void SetAsDefaultRepresentation::execute()
	{
		App::Old::Application::Representation::RepresentationLibrary::get().setDefaultRepresentation( _representationIndex );
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_NEED_UPDATE;
	}

} // namespace VTX::App::Old::Action::Representation
