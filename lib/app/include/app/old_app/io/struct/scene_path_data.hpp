#ifndef __VTX_IO_STRUCT_SCENE_PATH_DATA__
#define __VTX_IO_STRUCT_SCENE_PATH_DATA__

#include "app/component/chemistry/_fwd.hpp"
#include "app/core/event/base_event_receiver_vtx.hpp"
#include "app/core/event/vtx_event.hpp"
#include "app/old_app/io/writer/writer_chemfiles.hpp"
#include <map>
#include <util/types.hpp>

namespace VTX
{
	// namespace Model
	//{
	//	class Molecule;
	// }

	namespace IO::Struct
	{
		class ScenePathData : public App::Core::Event::BaseEventReceiverVTX
		{
		  public:
			struct Data
			{
			  public:
				Data();
				Data( const FilePath & p_path );

				void			 registerPath( const FilePath & p_filepath );
				bool			 hasFilepath() const;
				const FilePath & getFilepath() const;

				bool hasChanged() const;
				void setHasChanged( const bool p_hasChanged );

				void							registerWriter( Writer::ChemfilesWriter * const p_writer );
				Writer::ChemfilesWriter * const getWriter();
				void							deleteWriter();

				bool needToSaveMolecule() const;

			  private:
				FilePath				  _path		  = FilePath( "" );
				bool					  _hasChanged = false;
				Writer::ChemfilesWriter * _writer	  = nullptr;
			};

			ScenePathData();
			virtual void receiveEvent( const App::Core::Event::VTXEvent & p_event ) override;

			void registerLoading( const App::Component::Chemistry::Molecule * const p_molecule,
								  const FilePath &									p_filepath );

			inline const FilePath & getCurrentPath() const { return _currentFilePath; }
			void					setCurrentPath( const FilePath & p_filePath, const bool p_addInRecentPath = true );
			void					clearCurrentPath();

			Data &						getData( const App::Component::Chemistry::Molecule * const p_molecule );
			const ScenePathData::Data & getData( const App::Component::Chemistry::Molecule * const p_molecule ) const;
			FilePath getFilepath( const App::Component::Chemistry::Molecule * const p_molecule ) const;

			void	   incrementSceneModifications();
			void	   decrementSceneModifications();
			const int  getSceneModificationsCounter() const;
			const bool sceneHasModifications() const;
			void	   forceSceneModifications();
			void	   resetSceneModifications();

		  private:
			FilePath													_currentFilePath = FilePath();
			std::map<const App::Component::Chemistry::Molecule *, Data> _mapMoleculePath;
			int															_sceneModificationsCount = 0;
			bool														_forceSceneModifications = false;
		};
	} // namespace IO::Struct
} // namespace VTX
#endif
