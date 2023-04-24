#include "app/action/representation.hpp"
#include "app/core/mvc/mvc_manager.hpp"
#include "app/core/worker/worker_manager.hpp"
#include "app/old_app/io/filesystem.hpp"
#include "app/old_app/representation/representation_manager.hpp"
#include "app/old_app/setting.hpp"
#include "app/old_app/vtx_app.hpp"
#include "app/worker/representation_loader.hpp"
#include "app/worker/representation_saver.hpp"
#include <filesystem>
#include <util/filesystem.hpp>

namespace VTX::App::Action::Representation
{
	void ReloadPresets::execute()
	{
		Worker::RepresentationLibraryLoader * libraryLoader
			= new Worker::RepresentationLibraryLoader( Model::Representation::RepresentationLibrary::get() );
		VTX_WORKER( libraryLoader );
	}

	void ResetPresetsToDefault::execute() { VTX::Representation::RepresentationManager::get().resetRepresentations(); }

	void SavePreset::execute()
	{
		if ( _clearDirectory )
		{
			Util::Filesystem::removeAll( IO::Filesystem::getRepresentationsLibraryDir() );
			std::filesystem::create_directory( IO::Filesystem::getRepresentationsLibraryDir() );
		}

		for ( const Model::Representation::Representation * const representation : _representations )
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
		VTX::Representation::RepresentationManager::get().setQuickAccessToPreset( _representation, _quickAccess );
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
		Model::Representation::Representation * const newRepresentation
			= VTX::Core::MVC::MvcManager::get().instantiateModel<Model::Representation::Representation>(
				VTX::Setting::DEFAULT_REPRESENTATION_TYPE );

		newRepresentation->setName( _representationName );
		Model::Representation::RepresentationLibrary::get().addRepresentation( newRepresentation );
	}

	void CopyPresetInLibrary::execute()
	{
		Model::Representation::RepresentationLibrary::get().copyRepresentation( _representationIndex );
	}

	void DeletePresetInLibrary::execute()
	{
		const Model::Representation::Representation * representation
			= Model::Representation::RepresentationLibrary::get().getRepresentation( _representationIndex );

		VTX::Representation::RepresentationManager::get().deleteRepresentation( representation );

		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	}

	void SetAsDefaultRepresentation::execute()
	{
		Model::Representation::RepresentationLibrary::get().setDefaultRepresentation( _representationIndex );
		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	}

} // namespace VTX::App::Action::Representation