#ifndef __VTX_LOADER__
#define __VTX_LOADER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_worker.hpp"
#include "io/path.hpp"
#include "io/reader/base_reader.hpp"
#include "model/molecule.hpp"

namespace VTX
{
	namespace Worker
	{
		class Loader : public Worker::BaseWorker
		{
		  public:
			explicit Loader( const std::vector<IO::Path *> & p_paths ) : _paths( p_paths ) {}
			~Loader() { _paths.clear(); }

			virtual void work() override;

		  private:
			std::vector<IO::Path *> _paths;

			IO::Reader::BaseReader<Model::Molecule> * _createReader( const IO::Path * const ) const;
			bool									  _loadMolecule( Model::Molecule * const,
																	 IO::Reader::BaseReader<Model::Molecule> * const,
																	 const IO::Path * const ) const;
		};
	} // namespace Worker
} // namespace VTX
#endif
