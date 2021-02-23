#ifndef __VTX_LOADER__
#define __VTX_LOADER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_worker.hpp"
#include "define.hpp"
#include <map>
#include <vector>

namespace VTX
{
	namespace Worker
	{
		class Loader : public Worker::BaseWorker
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

			uint _run() override;

		  private:
			std::vector<FilePath *>				_paths			   = std::vector<FilePath *>();
			std::map<FilePath *, std::string *> _mapFileNameBuffer = std::map<FilePath *, std::string *>();

			MODE _getMode( const FilePath & ) const;
		};
	} // namespace Worker
} // namespace VTX
#endif
