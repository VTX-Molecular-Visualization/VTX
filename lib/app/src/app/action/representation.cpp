#include "app/action/representation.hpp"
#include "app/application/representation/representation_manager.hpp"
#include "app/internal/worker/representation_loader.hpp"
#include "app/internal/worker/representation_saver.hpp"
#include "app/mvc.hpp"
#include "app/old_app/io/filesystem.hpp"
#include "app/application/setting.hpp"
#include "app/old_app/vtx_app.hpp"
#include "app/worker.hpp"
#include <filesystem>
#include <util/filesystem.hpp>

namespace VTX::App::Action::Representation
{
	void ReloadPresets::execute()
	{
		Worker::RepresentationLibraryLoader * libraryLoader
			= new Worker::RepresentationLibraryLoader( App::Application::Representation::RepresentationLibrary::get() );
		VTX_WORKER( libraryLoader );
	}

	void ResetPresetsToDefault::execute()
	{
		App::Application::Representation::RepresentationManager::get().resetRepresentations();
	}

	void SavePreset::execute()
	{
		if ( _clearDirectory )
		{
			Util::Filesystem::removeAll( IO::Filesystem::getRepresentationsLibraryDir() );
			std::filesystem::create_directory( IO::Filesystem::getRepresentationsLibraryDir() );
		}

		for ( const App::Application::Representation::RepresentationPreset * const representation : _representations )
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
				FilePath path = IO::Filesystem::getRepresentationPath( representation->getName() );
				Util::Filesystem::generateUniqueFileName( path );

				Worker::RepresentationSaver * librarySaver = new Worker::RepresentationSaver( representation, path );

				VTX_WORKER( librarySaver );
			}
		}
	}

	void ChangeName::execute() { _representation->setName( _name ); }

	void ChangeQuickAccess::execute()
	{
		App::Application::Representation::RepresentationManager::get().setQuickAccessToPreset( _representation,
																							   _quickAccess );
	}

	void ChangeRepresentation::execute()
	{
		_representation->changeRepresentationType( _representationType );
		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeColorMode::execute()
	{
		_representation->getData().setColorMode( _colorMode );
		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeColor::execute()
	{
		_representation->setColor( _color );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	}

	void ChangeSphereRadius::execute()
	{
		_representation->getData().setSphereRadius( _radius );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	}

	void ChangeCylinderRadius::execute()
	{
		_representation->getData().setCylinderRadius( _radius );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	}

	void ChangeCylinderColorBendingMode::execute()
	{
		_representation->getData().setCylinderColorBlendingMode( _colorBendingMode );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	}

	void ChangeRibbonColorMode::execute()
	{
		_representation->getData().setRibbonColorMode( _colorMode );
		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeRibbonColorBendingMode::execute()
	{
		_representation->getData().setRibbonColorBlendingMode( _colorBendingMode );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	}

	void AddNewPresetInLibrary::execute()
	{
		App::Application::Representation::RepresentationPreset * const newRepresentation
			= VTX::MVC_MANAGER().instantiateModel<App::Application::Representation::RepresentationPreset>(
				VTX::App::Application::Setting::DEFAULT_REPRESENTATION_TYPE );

		newRepresentation->setName( _representationName );
		App::Application::Representation::RepresentationLibrary::get().addRepresentation( newRepresentation );
	}

	void CopyPresetInLibrary::execute()
	{
		App::Application::Representation::RepresentationLibrary::get().copyRepresentation( _representationIndex );
	}

	void DeletePresetInLibrary::execute()
	{
		const App::Application::Representation::RepresentationPreset * representation
			= App::Application::Representation::RepresentationLibrary::get().getRepresentation( _representationIndex );

		App::Application::Representation::RepresentationManager::get().deleteRepresentation( representation );

		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	}

	void SetAsDefaultRepresentation::execute()
	{
		App::Application::Representation::RepresentationLibrary::get().setDefaultRepresentation( _representationIndex );
		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	}

} // namespace VTX::App::Action::Representation
