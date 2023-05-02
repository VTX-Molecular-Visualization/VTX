#ifndef __VTX_APP_INTERNAL_WORKER_LOADER__
#define __VTX_APP_INTERNAL_WORKER_LOADER__

#include "app/component/chemistry/_fwd.hpp"
#include "app/component/io/_fwd.hpp"
#include "app/component/object3d/_fwd.hpp"
#include "app/core/worker/base_thread.hpp"
#include <map>
#include <string>
#include <util/chrono.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Internal::Worker
{
	class Loader : public App::Core::Worker::BaseThread
	{
		// Q_OBJECT

	  public:
		enum class SOURCE_TYPE : int
		{
			FILE,
			BUFFER,
			UNKNOWN,
		};

		struct Result
		{
			Result() {};
			Result( const SOURCE_TYPE p_sourceType ) : sourceType( p_sourceType ) {};

		  public:
			bool									 state	  = false;
			App::Component::Chemistry::Molecule *	 molecule = nullptr;
			App::Component::Object3D::MeshTriangle * mesh	  = nullptr;

			SOURCE_TYPE sourceType = SOURCE_TYPE::UNKNOWN;
		};

		explicit Loader( const FilePath & p_path ) { _paths.emplace_back( p_path ); }
		explicit Loader( const std::vector<FilePath> & p_paths ) : _paths( p_paths ) {}
		explicit Loader( const std::map<FilePath, std::string *> & p_buffers ) : _mapFileNameBuffer( p_buffers ) {}
		~Loader() = default;

		inline const std::map<FilePath, Result> & getPathsResult() const { return _pathResult; }
		inline void addDynamicTarget( App::Component::Chemistry::Molecule * const p_target )
		{
			_moleculeTargetsForDynamics.emplace_back( p_target );
		}
		inline void setOpenTrajectoryAsMoleculeIfTargetFail( const bool p_openAsMolecule )
		{
			_openTrajectoryAsMolecule = p_openAsMolecule;
		}

	  protected:
		uint _run() override;

		void _loadSceneFiles();
		void _loadConfigurationFiles( App::Component::IO::MoleculeConfiguration & );
		void _loadMoleculeFiles( const App::Component::IO::MoleculeConfiguration & );
		void _loadTrajectoriesFiles( const App::Component::IO::MoleculeConfiguration & p_config );
		void _loadMeshFiles();

		void _loadMoleculeBuffers();

	  private:
		std::vector<FilePath>			  _paths			 = std::vector<FilePath>();
		std::map<FilePath, std::string *> _mapFileNameBuffer = std::map<FilePath, std::string *>();

		std::map<FilePath, Result> _pathResult = std::map<FilePath, Result>();

		std::vector<std::vector<FilePath>>				   _filepathsPerMode = std::vector<std::vector<FilePath>>();
		std::vector<App::Component::Chemistry::Molecule *> _moleculeTargetsForDynamics
			= std::vector<App::Component::Chemistry::Molecule *>();

		Util::Chrono _loadingFileChrono;
		bool		 _openTrajectoryAsMolecule = true;

		void _startLoadingFile( const FilePath & p_path, const SOURCE_TYPE & p_sourceType );
		void _endLoadingFileSuccess( const FilePath & p_path );
		void _endLoadingFileFail( const FilePath & p_path, const std::string & p_message );
	};
} // namespace VTX::App::Internal::Worker

#endif
