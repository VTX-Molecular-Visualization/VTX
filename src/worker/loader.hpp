#ifndef __VTX_LOADER__
#define __VTX_LOADER__

#include "base_thread.hpp"
#include "define.hpp"
#include "io/filepath.hpp"
#include "tool/chrono.hpp"
#include <map>
#include <vector>

namespace VTX
{
	namespace Model
	{
		namespace Configuration
		{
			struct Molecule;
		}

		class Molecule;
		class MeshTriangle;
	} // namespace Model

	namespace Object3D
	{
		class Scene;
	}

	namespace Worker
	{
		class Loader : public Worker::BaseThread
		{
			Q_OBJECT

		  public:
			// Order in enum define opening order
			enum class MODE : int
			{
				SCENE,
				CONFIGURATION,
				MOLECULE,
				TRAJECTORY,
				MESH,
				UNKNOWN,

				COUNT
			};

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
				bool				  state	   = false;
				Model::Molecule *	  molecule = nullptr;
				Model::MeshTriangle * mesh	   = nullptr;

				SOURCE_TYPE sourceType = SOURCE_TYPE::UNKNOWN;
			};

			explicit Loader( const IO::FilePath & p_path ) { _paths.emplace_back( p_path ); }
			explicit Loader( const std::vector<IO::FilePath> & p_paths ) : _paths( p_paths ) {}
			explicit Loader( const std::map<IO::FilePath, std::string *> & p_buffers ) : _mapFileNameBuffer( p_buffers )
			{
			}
			~Loader() = default;

			inline const std::map<IO::FilePath, Result> & getPathsResult() const { return _pathResult; }
			inline void									  addDynamicTarget( Model::Molecule * const p_target )
			{
				_moleculeTargetsForDynamics.emplace_back( p_target );
			}

		  protected:
			uint _run() override;

			void _loadSceneFiles();
			void _loadConfigurationFiles( Model::Configuration::Molecule & );
			void _loadMoleculeFiles( const Model::Configuration::Molecule & );
			void _loadTrajectoriesFiles( const Model::Configuration::Molecule & p_config );
			void _loadMeshFiles();

			void _loadMoleculeBuffers();

		  private:
			std::vector<IO::FilePath>			  _paths			 = std::vector<IO::FilePath>();
			std::map<IO::FilePath, std::string *> _mapFileNameBuffer = std::map<IO::FilePath, std::string *>();

			std::map<IO::FilePath, Result> _pathResult = std::map<IO::FilePath, Result>();

			std::vector<std::vector<IO::FilePath>> _filepathsPerMode = std::vector<std::vector<IO::FilePath>>();
			std::vector<Model::Molecule *>		   _moleculeTargetsForDynamics = std::vector<Model::Molecule *>();

			Tool::Chrono _loadingFileChrono;

			void _fillFilepathPerMode();

			MODE _getMode( const IO::FilePath & ) const;

			void _startLoadingFile( const IO::FilePath & p_path, const SOURCE_TYPE & p_sourceType );
			void _endLoadingFileSuccess( const IO::FilePath & p_path );
			void _endLoadingFileFail( const IO::FilePath & p_path, const std::string & p_message );
		};
	} // namespace Worker
} // namespace VTX
#endif
