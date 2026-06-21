#ifndef __VTX_APP_EXTRACTOR_MESH__
#define __VTX_APP_EXTRACTOR_MESH__

#include "app/threading/base_thread.hpp"
#include <util/filesystem.hpp>
#include <util/thread.hpp>

namespace VTX::App::Extractor
{
	class Mesh
	{
	  public:
		Mesh() = delete;
		explicit Mesh( FilePath );

		uint operator()( Util::StopToken, Threading::OptionalThreadReference );

	  private:
		FilePath _sourcePath;
	};
} // namespace VTX::App::Extractor

#endif // !__VTX_APP_EXTRACTOR_MESH__
