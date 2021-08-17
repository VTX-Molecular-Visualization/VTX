#ifndef __VTX_ACTION_REPRESENTATION__
#define __VTX_ACTION_REPRESENTATION__

#include "base_action.hpp"
#include "generic/base_colorable.hpp"
#include "generic/base_representable.hpp"
#include "model/molecule.hpp"
#include "model/representation/instantiated_representation.hpp"
#include "model/representation/representation.hpp"
#include "model/representation/representation_library.hpp"
#include "model/secondary_structure.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "representation/representation_manager.hpp"
#include "setting.hpp"
#include "util/filesystem.hpp"
#include "vtx_app.hpp"
#include "worker/representation_loader.hpp"
#include "worker/representation_saver.hpp"
#include "worker/worker_manager.hpp"
#include <unordered_set>

namespace VTX::Action::Representation
{
	class ReloadPresets : public BaseAction
	{
	  public:
		ReloadPresets() {};
		virtual void execute() override
		{
			Worker::RepresentationLibraryLoader * libraryLoader
				= new Worker::RepresentationLibraryLoader( Model::Representation::RepresentationLibrary::get() );
			VTX_WORKER( libraryLoader );
		};
	};

	class SavePreset : public BaseAction
	{
	  public:
		SavePreset( const Model::Representation::Representation & p_representation )
		{
			_representations.emplace( &p_representation );
		};
		SavePreset( const std::unordered_set<const Model::Representation::Representation *> & p_representations )
		{
			for ( const Model::Representation::Representation * const representation : p_representations )
				_representations.emplace( representation );
		};
		SavePreset( Model::Representation::RepresentationLibrary & p_library )
		{
			for ( int i = 0; i < p_library.getRepresentationCount(); i++ )
				_representations.emplace( p_library.getRepresentation( i ) );
			_clearDirectory = true;
		};

		void setAsync( const bool p_async ) { _async = p_async; }

		virtual void execute() override
		{
			if ( _clearDirectory )
			{
				Util::Filesystem::removeAll( Util::Filesystem::getRepresentationsLibraryDir() );
				Util::Filesystem::createDirectory( Util::Filesystem::getRepresentationsLibraryDir() );
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
					FilePath path = Util::Filesystem::getRepresentationPath( representation->getName() );
					Util::Filesystem::generateUniqueFileName( path );

					Worker::RepresentationSaver * librarySaver
						= new Worker::RepresentationSaver( representation, path );

					VTX_WORKER( librarySaver );
				}
			}
		};

	  private:
		std::unordered_set<const Model::Representation::Representation *> _representations
			= std::unordered_set<const Model::Representation::Representation *>();

		bool _clearDirectory = false;
		bool _async			 = true;
	};

	class ChangeName : public BaseAction
	{
	  public:
		explicit ChangeName( Model::Representation::Representation * const p_representation,
							 const std::string &						   p_name ) :
			_representation( p_representation ),
			_name( Model::Representation::RepresentationLibrary::get().getValidName( p_name ) )
		{
		}

		void execute() { _representation->setName( _name ); };

	  private:
		const std::string							  _name;
		Model::Representation::Representation * const _representation;
	};
	class ChangeQuickAccess : public BaseAction
	{
	  public:
		explicit ChangeQuickAccess( Model::Representation::Representation * const p_representation,
									const bool									  p_quickAccess ) :
			_representation( p_representation ),
			_quickAccess( p_quickAccess )
		{
		}

		void execute()
		{
			VTX::Representation::RepresentationManager::get().setQuickAccessToPreset( _representation, _quickAccess );
		};

	  private:
		const bool									  _quickAccess;
		Model::Representation::Representation * const _representation;
	};

	class ChangeRepresentation : public BaseAction
	{
	  public:
		explicit ChangeRepresentation( Model::Representation::Representation * const p_representation,
									   const Generic::REPRESENTATION &				 p_representationType ) :
			_representation( p_representation ),
			_representationType( p_representationType )
		{
		}

		void execute()
		{
			_representation->changeRepresentationType( _representationType );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		};

	  private:
		const Generic::REPRESENTATION				  _representationType;
		Model::Representation::Representation * const _representation;
	};
	class ChangeColorMode : public BaseAction
	{
	  public:
		explicit ChangeColorMode( Model::Representation::Representation * const p_representation,
								  const Generic::COLOR_MODE &					p_colorMode ) :
			_representation( p_representation ),
			_colorMode( p_colorMode )
		{
		}

		void execute()
		{
			_representation->getData().setColorMode( _colorMode );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		};

	  private:
		const Generic::COLOR_MODE					  _colorMode;
		Model::Representation::Representation * const _representation;
	};

	class ChangeSecondaryStructureColorMode : public BaseAction
	{
	  public:
		explicit ChangeSecondaryStructureColorMode( Model::Representation::Representation * const p_representation,
													Generic::SECONDARY_STRUCTURE_COLOR_MODE &	  p_colorMode ) :
			_representation( p_representation ),
			_colorMode( p_colorMode )
		{
		}

		void execute()
		{
			_representation->getData().setSecondaryStructureColorMode( _colorMode );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		};

	  private:
		const Generic::SECONDARY_STRUCTURE_COLOR_MODE _colorMode;
		Model::Representation::Representation * const _representation;
	};

	class ChangeColor : public BaseAction
	{
	  public:
		explicit ChangeColor( Model::Representation::Representation * const p_representation,
							  const Color::Rgb &							p_color ) :
			_representation( p_representation ),
			_color( p_color )
		{
		}
		void execute()
		{
			_representation->setColor( _color );
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const Color::Rgb							  _color;
		Model::Representation::Representation * const _representation;
	};

	class ChangeSphereRadius : public BaseAction
	{
	  public:
		explicit ChangeSphereRadius( Model::Representation::Representation * const p_representation,
									 const float								   p_radius ) :
			_representation( p_representation ),
			_radius( p_radius )
		{
		}

		void execute()
		{
			_representation->getData().setSphereRadius( _radius );
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const float									  _radius;
		Model::Representation::Representation * const _representation;
	};
	class ChangeCylinderRadius : public BaseAction
	{
	  public:
		explicit ChangeCylinderRadius( Model::Representation::Representation * const p_representation,
									   const float									 p_radius ) :
			_representation( p_representation ),
			_radius( p_radius )
		{
		}

		void execute()
		{
			_representation->getData().setCylinderRadius( _radius );
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const float									  _radius;
		Model::Representation::Representation * const _representation;
	};

	class AddNewPresetInLibrary : public BaseAction
	{
	  public:
		explicit AddNewPresetInLibrary( const std::string & p_representationName ) :
			_representationName( p_representationName )
		{
		}

		void execute()
		{
			Model::Representation::Representation * const newRepresentation
				= MVC::MvcManager::get().instantiateModel<Model::Representation::Representation>(
					VTX::Setting::DEFAULT_REPRESENTATION_TYPE );

			newRepresentation->setName( _representationName );
			Model::Representation::RepresentationLibrary::get().addRepresentation( newRepresentation );
		};

		const std::string _representationName;
	};

	class CopyPresetInLibrary : public BaseAction
	{
	  public:
		explicit CopyPresetInLibrary( const int p_representationIndex ) : _representationIndex( p_representationIndex )
		{
		}

		void execute()
		{
			Model::Representation::RepresentationLibrary::get().copyRepresentation( _representationIndex );
		};

	  private:
		const int _representationIndex;
	};

	class DeletePresetInLibrary : public BaseAction
	{
	  public:
		explicit DeletePresetInLibrary( const int p_representationIndex ) :
			_representationIndex( p_representationIndex )
		{
		}

		void execute()
		{
			const Model::Representation::Representation * representation
				= Model::Representation::RepresentationLibrary::get().getRepresentation( _representationIndex );

			VTX::Representation::RepresentationManager::get().deleteRepresentation( representation );

			VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
		};

	  private:
		const int _representationIndex;
	};

	class SetAsDefaultRepresentation : public BaseAction
	{
	  public:
		explicit SetAsDefaultRepresentation( const int p_representationIndex ) :
			_representationIndex( p_representationIndex )
		{
		}

		void execute()
		{
			Model::Representation::RepresentationLibrary::get().setDefaultRepresentation( _representationIndex );
			VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
		};

	  private:
		const int _representationIndex;
	};

} // namespace VTX::Action::Representation
#endif
