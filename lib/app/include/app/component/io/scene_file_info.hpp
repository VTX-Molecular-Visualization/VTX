#ifndef __VTX_APP_COMPONENT_IO_SCENE_FILE_INFO__
#define __VTX_APP_COMPONENT_IO_SCENE_FILE_INFO__

// #include <map>
#include "app/core/ecs/base_component.hpp"
#include <util/types.hpp>

namespace VTX::App::Component::IO
{
	class SceneFileInfo : public Core::ECS::BaseComponent
	{
	  public:
		// Copy of function in VTXApp that must be here
		//	bool VTXApp::hasAnyModifications() const
		//	{
		//		const bool hasSavePath	   = !getScenePathData().getCurrentPath().empty();
		//		const bool sceneIsEmpty	   = getScene().isEmpty();
		//		const bool sceneHasChanged = getScenePathData().sceneHasModifications();
		// #ifdef VTX_PRODUCTION
		//		return ( !hasSavePath && !sceneIsEmpty ) || ( hasSavePath && sceneHasChanged );
		// #else
		//		return hasSavePath && sceneHasChanged;
		// #endif
		//	}

		// struct Data
		//{
		//   public:
		//	Data();
		//	Data( const FilePath & p_path );

		//	void			 registerPath( const FilePath & p_filepath );
		//	bool			 hasFilepath() const;
		//	const FilePath & getFilepath() const;

		//	bool hasChanged() const;
		//	void setHasChanged( const bool p_hasChanged );

		//	// void							registerWriter( Writer::ChemfilesWriter * const p_writer );
		//	// Writer::ChemfilesWriter * const getWriter();
		//	// void							deleteWriter();

		//	bool needToSaveMolecule() const;

		//  private:
		//	FilePath				  _path		  = FilePath( "" );
		//	bool					  _hasChanged = false;
		//	// Writer::ChemfilesWriter * _writer	  = nullptr;
		//};

		// ScenePathData();
		// virtual void receiveEvent( const App::Core::Event::VTXEvent & p_event ) override;

		// void registerLoading( const App::Component::Chemistry::System * const p_molecule,
		//					  const FilePath &									p_filepath );

		// inline const FilePath & getCurrentPath() const { return _currentFilePath; }
		// void					setCurrentPath( const FilePath & p_filePath, const bool p_addInRecentPath = true );
		// void					clearCurrentPath();

		// Data &						getData( const App::Component::Chemistry::System * const p_molecule );
		// const ScenePathData::Data & getData( const App::Component::Chemistry::System * const p_molecule ) const;
		// FilePath					getFilepath( const App::Component::Chemistry::System * const p_molecule ) const;

		// void	   incrementSceneModifications();
		// void	   decrementSceneModifications();
		// const int  getSceneModificationsCounter() const;
		// const bool sceneHasModifications() const;
		// void	   forceSceneModifications();
		// void	   resetSceneModifications();

	  private:
		FilePath _currentFilePath = FilePath();
		// std::map<const App::Component::Chemistry::System *, Data> _mapMoleculePath;
		int	 _sceneModificationsCount = 0;
		bool _forceSceneModifications = false;
	};
} // namespace VTX::App::Component::IO
#endif
