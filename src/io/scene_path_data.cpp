#include "scene_path_data.hpp"
#include "event/event.hpp"
#include "event/event_manager.hpp"
#include "model/molecule.hpp"

namespace VTX::IO
{
	// ScenePathData::Data
	// /////////////////////////////////////////////////////////////////////////////////////////////////////////
	ScenePathData::Data::Data() : _path( FilePath() ) {}
	ScenePathData::Data::Data( const FilePath & p_path ) : _path( p_path ) {}
	ScenePathData::Data & ScenePathData::getData( const Model::Molecule * const p_molecule )
	{
		return _mapMoleculePath[ p_molecule ];
	}

	void			 ScenePathData::Data::registerPath( const FilePath & p_filePath ) { _path = p_filePath; }
	bool			 ScenePathData::Data::hasFilepath() const { return _path != ""; }
	const FilePath & ScenePathData::Data::getFilepath() const { return _path; }

	bool ScenePathData::Data::hasChanged() const { return _hasChanged; }
	void ScenePathData::Data::setHasChanged( const bool p_hasChanged ) { _hasChanged = p_hasChanged; };

	void ScenePathData::Data::registerWriter( Writer::ChemfilesWriter * const p_writer ) { _writer = p_writer; }
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	ScenePathData::ScenePathData() : Event::BaseEventReceiverVTX()
	{
		_registerEvent( Event::Global::MOLECULE_ADDED );
		_registerEvent( Event::Global::MOLECULE_REMOVED );
		_registerEvent( Event::Global::MOLECULE_STRUCTURE_CHANGE );
		_registerEvent( Event::Global::SCENE_SAVED );
	}

	void ScenePathData::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::MOLECULE_ADDED )
		{
			const Model::Molecule * const molecule
				= dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event ).ptr;

			if ( _mapMoleculePath.find( molecule ) == _mapMoleculePath.end() )
				_mapMoleculePath[ molecule ] = Data();
		}
		else if ( p_event.name == Event::Global::MOLECULE_REMOVED )
		{
			const Model::Molecule * const molecule
				= dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event ).ptr;
			_mapMoleculePath.erase( molecule );
		}
		else if ( p_event.name == Event::Global::MOLECULE_STRUCTURE_CHANGE )
		{
			const Model::Molecule * const molecule
				= dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event ).ptr;

			_mapMoleculePath[ molecule ].setHasChanged( true );
		}
		else if ( p_event.name == Event::Global::SCENE_SAVED )
		{
			resetSceneModifications();
		}
	}

	void ScenePathData::registerLoading( const Model::Molecule * const p_molecule, const FilePath & p_filepath )
	{
		if ( _mapMoleculePath.find( p_molecule ) == _mapMoleculePath.end() )
			_mapMoleculePath[ p_molecule ] = Data( p_filepath );
		else // If the file already exists, we add an empty entry to force a new save file
			_mapMoleculePath[ p_molecule ] = Data();
	}

	void ScenePathData::setCurrentPath( const FilePath & p_filePath, const bool p_addInRecentPath )
	{
		_currentFilePath = p_filePath;
		if ( p_addInRecentPath )
			Setting::enqueueNewLoadingPath( p_filePath );

		VTX_EVENT( new Event::VTXEventRef<const FilePath>( Event::Global::SCENE_PATH_CHANGE, _currentFilePath ) );
	}
	void ScenePathData::clearCurrentPath()
	{
		_currentFilePath = FilePath();
		VTX_EVENT( new Event::VTXEventRef<const FilePath>( Event::Global::SCENE_PATH_CHANGE, _currentFilePath ) );
	}

	Writer::ChemfilesWriter * const ScenePathData::Data::getWriter() { return _writer; }
	void							ScenePathData::Data::deleteWriter()
	{
		if ( _writer != nullptr )
		{
			delete _writer;
			_writer = nullptr;
		}
	}

	bool ScenePathData::Data::needToSaveMolecule() const { return _path == "" || _hasChanged; }

	void ScenePathData::incrementSceneModifications()
	{
		_sceneModificationsCount++;

		if ( _sceneModificationsCount == 0 )
			VTX_EVENT( new Event::VTXEventValue<bool>( Event::Global::SCENE_MODIFICATION_STATE_CHANGE, false ) );
		else if ( _sceneModificationsCount == 1 )
			VTX_EVENT( new Event::VTXEventValue<bool>( Event::Global::SCENE_MODIFICATION_STATE_CHANGE, true ) );
	}
	void ScenePathData::decrementSceneModifications()
	{
		_sceneModificationsCount--;

		if ( _sceneModificationsCount == 0 )
			VTX_EVENT( new Event::VTXEventValue<bool>( Event::Global::SCENE_MODIFICATION_STATE_CHANGE, false ) );
		else if ( _sceneModificationsCount == -1 )
			VTX_EVENT( new Event::VTXEventValue<bool>( Event::Global::SCENE_MODIFICATION_STATE_CHANGE, true ) );
	}
	const int  ScenePathData::getSceneModificationsCounter() const { return _sceneModificationsCount; }
	const bool ScenePathData::sceneHasModifications() const
	{
		return _forceSceneModifications || _sceneModificationsCount != 0;
	}
	void ScenePathData::forceSceneModifications()
	{
		const bool sceneWasModified = sceneHasModifications();
		_forceSceneModifications	= true;

		if ( !sceneWasModified )
			VTX_EVENT( new Event::VTXEventValue<bool>( Event::Global::SCENE_MODIFICATION_STATE_CHANGE, true ) );
	}
	void ScenePathData::resetSceneModifications()
	{
		const bool sceneWasModified = sceneHasModifications();

		_forceSceneModifications = false;
		_sceneModificationsCount = 0;

		if ( sceneWasModified )
			VTX_EVENT( new Event::VTXEventValue<bool>( Event::Global::SCENE_MODIFICATION_STATE_CHANGE, false ) );
	}

} // namespace VTX::IO
