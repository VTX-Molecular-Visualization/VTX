#include "app/old/action/dev.hpp"
#include "app/old/internal/io/filesystem.hpp"
#include "app/old/render/renderer/gl/program_manager.hpp"
#include "app/old/vtx_app.hpp"
#include <filesystem>
#include <util/filesystem.hpp>

namespace VTX::App::Old::Action::Dev
{
	void CompileShaders::execute()
	{
		// Delete files.
		Util::Filesystem::removeAll( App::Old::Internal::IO::Filesystem::getShadersDir() );
		// Copy from sources.
		std::filesystem::copy( App::Old::Internal::IO::Filesystem::SHADERS_DIR_SRC,
							   App::Old::Internal::IO::Filesystem::getShadersDir() );
		// Recompile.
		VTX_PROGRAM_MANAGER().refreshShaders();
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_NEED_UPDATE;
	}

} // namespace VTX::App::Old::Action::Dev
