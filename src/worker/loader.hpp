#ifndef __VTX_LOADER__
#define __VTX_LOADER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_worker.hpp"

#include "io/reader/base_reader.hpp"
#include "model/mesh_triangle.hpp"
#include "model/molecule.hpp"

namespace VTX
{
	namespace Worker
	{
		class Loader : public Worker::BaseWorker
		{
		  public:
			enum class MODE : int
			{
				MOLECULE,
				MESH,
				UNKNOWN,
			};

			explicit Loader( const std::vector<Path *> & p_paths ) : _paths( p_paths ) {}
			~Loader() { _paths.clear(); }

			virtual void work() override;

		  private:
			std::vector<Path *> _paths;

			template<typename T>
			void _load( T * const, IO::Reader::BaseReader<T> * const, const Path * const ) const;
			MODE _getMode( const Path & ) const;
		};
	} // namespace Worker
} // namespace VTX
#endif
