#include "app/action/dev.hpp"
#include "app/old_app/io/filesystem.hpp"
#include "app/old_app/renderer/gl/program_manager.hpp"
#include "app/old_app/vtx_app.hpp"
#include <filesystem>
#include <util/filesystem.hpp>

namespace VTX::App::Action::Dev
{
	void CompileShaders::execute()
	{
		// Delete files.
		Util::Filesystem::removeAll( IO::Filesystem::getShadersDir() );
		// Copy from sources.
		std::filesystem::copy( IO::Filesystem::SHADERS_DIR_SRC, IO::Filesystem::getShadersDir() );
		// Recompile.
		VTX_PROGRAM_MANAGER().refreshShaders();
		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	}

} // namespace VTX::App::Action::Dev
