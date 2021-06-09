#include "scene_path_data.hpp"
#include "event/event.hpp"
#include "model/molecule.hpp"

namespace VTX::IO
{
	ScenePathData::Data::Data() : _path( FilePath() ) {}
	ScenePathData::Data::Data( const FilePath & p_path ) : _path( p_path ) {}

	ScenePathData::ScenePathData() : Event::BaseEventReceiverVTX()
	{
		_registerEvent( Event::Global::MOLECULE_ADDED );
		_registerEvent( Event::Global::MOLECULE_REMOVED );
		_registerEvent( Event::Global::MOLECULE_STRUCTURE_CHANGE );
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
	}

	void ScenePathData::registerLoading( const Model::Molecule * const p_molecule, const FilePath & p_filepath )
	{
		if ( _mapMoleculePath.find( p_molecule ) == _mapMoleculePath.end() )
			_mapMoleculePath[ p_molecule ] = Data( p_filepath );
		else // If the file already exists, we add an empty entry to force a new save file
			_mapMoleculePath[ p_molecule ] = Data();
	}

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

} // namespace VTX::IO
