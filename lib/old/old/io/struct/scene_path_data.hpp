#ifndef __VTX_IO_STRUCT_SCENE_PATH_DATA__
#define __VTX_IO_STRUCT_SCENE_PATH_DATA__

#include <util/types.hpp>
#include "event/base_event_receiver_vtx.hpp"
#include "event/event.hpp"
#include "io/writer/writer_chemfiles.hpp"
#include <map>

namespace VTX
{
	namespace Model
	{
		class Molecule;
	}

	namespace IO::Struct
	{
		class ScenePathData : Event::BaseEventReceiverVTX
		{
		  public:
			struct Data
			{
			  public:
				Data();
				Data( const Util::FilePath & p_path );

				void				 registerPath( const Util::FilePath & p_filepath );
				bool				 hasFilepath() const;
				const Util::FilePath & getFilepath() const;

				bool hasChanged() const;
				void setHasChanged( const bool p_hasChanged );

				void							registerWriter( Writer::ChemfilesWriter * const p_writer );
				Writer::ChemfilesWriter * const getWriter();
				void							deleteWriter();

				bool needToSaveMolecule() const;

			  private:
				Util::FilePath			  _path		  = Util::FilePath( "" );
				bool					  _hasChanged = false;
				Writer::ChemfilesWriter * _writer	  = nullptr;
			};

			ScenePathData();
			virtual void receiveEvent( const Event::VTXEvent & p_event ) override;

			void registerLoading( const Model::Molecule * const p_molecule, const Util::FilePath & p_filepath );

			inline const Util::FilePath & getCurrentPath() const { return _currentFilePath; }
			void setCurrentPath( const Util::FilePath & p_filePath, const bool p_addInRecentPath = true );
			void clearCurrentPath();

			Data &						getData( const Model::Molecule * const p_molecule );
			const ScenePathData::Data & getData( const Model::Molecule * const p_molecule ) const;
			Util::FilePath				getFilepath( const Model::Molecule * const p_molecule ) const;

			void	   incrementSceneModifications();
			void	   decrementSceneModifications();
			const int  getSceneModificationsCounter() const;
			const bool sceneHasModifications() const;
			void	   forceSceneModifications();
			void	   resetSceneModifications();

		  private:
			Util::FilePath							_currentFilePath = Util::FilePath();
			std::map<const Model::Molecule *, Data> _mapMoleculePath;
			int										_sceneModificationsCount = 0;
			bool									_forceSceneModifications = false;
		};
	} // namespace IO::Struct
} // namespace VTX
#endif