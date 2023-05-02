#include "app/action/dev.hpp"
#include "app/internal/io/filesystem.hpp"
#include "app/render/renderer/gl/program_manager.hpp"
#include "app/old_app/vtx_app.hpp"
#include <filesystem>
#include <util/filesystem.hpp>

namespace VTX::App::Action::Dev
{
	void CompileShaders::execute()
	{
		// Delete files.
		Util::Filesystem::removeAll( App::Internal::IO::Filesystem::getShadersDir() );
		// Copy from sources.
		std::filesystem::copy( App::Internal::IO::Filesystem::SHADERS_DIR_SRC,
							   App::Internal::IO::Filesystem::getShadersDir() );
		// Recompile.
		VTX_PROGRAM_MANAGER().refreshShaders();
		VTXApp::get().MASK |= Render::VTX_MASK_NEED_UPDATE;
	}

} // namespace VTX::App::Action::Dev
