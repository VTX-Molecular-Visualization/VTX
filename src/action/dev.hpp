#ifndef __VTX_ACTION_DEV__
#define __VTX_ACTION_DEV__

#include "base_action.hpp"
#include "renderer/gl/program_manager.hpp"
#include "util/filesystem.hpp"
#include "vtx_app.hpp"

namespace VTX::Action::Dev
{
	class CompileShaders : public BaseAction
	{
	  public:
		virtual void execute() override
		{
			// Delete files.
			Util::Filesystem::removeAll( Util::Filesystem::SHADERS_DIR );
			// Copy from sources.
			Util::Filesystem::copy( Util::Filesystem::SHADERS_DIR_SRC, Util::Filesystem::SHADERS_DIR );
			// Recompile.
			VTX_PROGRAM_MANAGER().refreshShaders();
			VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
		}
	};

} // namespace VTX::Action::Dev

#endif
