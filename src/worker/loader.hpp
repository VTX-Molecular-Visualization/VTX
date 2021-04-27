#ifndef __VTX_LOADER__
#define __VTX_LOADER__

#ifdef _MSC_VER
#pragma once
#endif

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

			explicit Loader( const std::vector<FilePath *> & p_paths ) : _paths( p_paths ) {}
			explicit Loader( const std::map<FilePath *, std::string *> & p_buffers ) : _mapFileNameBuffer( p_buffers )
			{
			}
			~Loader()
			{
				_paths.clear();
				_mapFileNameBuffer.clear();
			}

			inline std::vector<Model::Molecule *> &		getMolecules() { return _molecules; }
			inline std::vector<Model::MeshTriangle *> & getMeshes() { return _meshes; }

			uint _run() override;

		  private:
			std::vector<FilePath *>				_paths			   = std::vector<FilePath *>();
			std::map<FilePath *, std::string *> _mapFileNameBuffer = std::map<FilePath *, std::string *>();

			std::vector<Model::Molecule *>	   _molecules = std::vector<Model::Molecule *>();
			std::vector<Model::MeshTriangle *> _meshes	  = std::vector<Model::MeshTriangle *>();

			MODE _getMode( const FilePath & ) const;
		};
	} // namespace Worker
} // namespace VTX
#endif
