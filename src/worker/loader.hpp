#ifndef __VTX_LOADER__
#define __VTX_LOADER__

#include "base_thread.hpp"
#include "define.hpp"
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

			explicit Loader( const FilePath & p_path ) { _paths.emplace_back( p_path ); }
			explicit Loader( const std::vector<FilePath> & p_paths ) : _paths( p_paths ) {}
			explicit Loader( const std::map<FilePath, std::string *> & p_buffers ) : _mapFileNameBuffer( p_buffers ) {}
			~Loader() = default;

			inline const std::map<FilePath, Result> & getPathsResult() const { return _pathResult; }

		  protected:
			uint _run() override;

		  private:
			std::vector<FilePath>			  _paths			 = std::vector<FilePath>();
			std::map<FilePath, std::string *> _mapFileNameBuffer = std::map<FilePath, std::string *>();

			std::map<FilePath, Result> _pathResult = std::map<FilePath, Result>();

			MODE _getMode( const FilePath & ) const;
		};
	} // namespace Worker
} // namespace VTX
#endif
