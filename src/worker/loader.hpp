#ifndef __VTX_LOADER__
#define __VTX_LOADER__

#include "base_thread.hpp"
#include "define.hpp"
#include "io/filepath.hpp"
#include <map>
#include <vector>

namespace VTX
{
	namespace Model
	{
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
			enum class MODE : int
			{
				MOLECULE,
				MESH,
				VTX,
				UNKNOWN,
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

		  protected:
			uint _run() override;

		  private:
			std::vector<IO::FilePath>			  _paths			 = std::vector<IO::FilePath>();
			std::map<IO::FilePath, std::string *> _mapFileNameBuffer = std::map<IO::FilePath, std::string *>();

			std::map<IO::FilePath, Result> _pathResult = std::map<IO::FilePath, Result>();

			MODE _getMode( const IO::FilePath & ) const;
		};
	} // namespace Worker
} // namespace VTX
#endif
