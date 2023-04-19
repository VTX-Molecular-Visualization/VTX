#ifndef __VTX_ACTION_DEV__
#define __VTX_ACTION_DEV__

#include "app/core/action/base_action.hpp"
#include "app/old_app/io/filesystem.hpp"
#include "app/old_app/renderer/gl/program_manager.hpp"
#include "app/old_app/vtx_app.hpp"

namespace VTX::Action::Dev
{
	class CompileShaders : public Core::Action::BaseAction
	{
	  public:
		virtual void execute() override
		{
			// Delete files.
			Util::Filesystem::removeAll( IO::Filesystem::getShadersDir() );
			// Copy from sources.
			std::filesystem::copy( IO::Filesystem::SHADERS_DIR_SRC, IO::Filesystem::getShadersDir() );
			// Recompile.
			VTX_PROGRAM_MANAGER().refreshShaders();
			VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
		}
	};

} // namespace VTX::Action::Dev

#endif
