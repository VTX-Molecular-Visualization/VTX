#include "filesystem.hpp"
#include "io/scene_path_data.hpp"
#include "model/molecule.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "object3d/scene.hpp"
#include "selection/selection_manager.hpp"
#include "vtx_app.hpp"

namespace VTX::Util::Filesystem
{
	FilePath getDefaultMoleculeExportPath()
	{
		const FilePath defaultFolder = FilePath( Setting::getLastExportedMoleculeFolder().toStdString() );

		const Model::Molecule * exportedMolecule = nullptr;
		const int				nbMoleculeInSelection
			= Selection::SelectionManager::get().getSelectionModel().getMoleculeSelectedCount();
		if ( nbMoleculeInSelection == 1 )
		{
			const Model::ID & moleculeID
				= Selection::SelectionManager::get().getSelectionModel().getItems().begin()->first;
			exportedMolecule = &( MVC::MvcManager::get().getModel<Model::Molecule>( moleculeID ) );
		}
		else if ( nbMoleculeInSelection == 0 && VTXApp::get().getScene().getMolecules().size() == 1 )
		{
			exportedMolecule = VTXApp::get().getScene().getMolecules().begin()->first;
		}

		std::string filename
			= exportedMolecule == nullptr ? DEFAULT_MOLECULE_FILENAME : exportedMolecule->getDisplayName();
		filename = filename + '.' + Util::Filesystem::DEFAULT_MOLECULE_WRITE_EXTENSION.toStdString();

		return defaultFolder / filename;
	}
	FilePath getDefaultSceneSavePath()
	{
		if ( !VTXApp::get().getScenePathData().getCurrentPath().empty() )
		{
			return VTXApp::get().getScenePathData().getCurrentPath();
		}

		const FilePath defaultFolder = FilePath( Setting::getLastSavedSessionFolder().toStdString() );
		FilePath	   defaultPath	 = defaultFolder / ( DEFAULT_SCENE_FILENAME + ".vtx" );

		generateUniqueFileName( defaultPath );

		return defaultPath;
	}

} // namespace VTX::Util::Filesystem
