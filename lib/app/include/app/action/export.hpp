#ifndef __VTX_UI_ACTION_EXPORT__
#define __VTX_UI_ACTION_EXPORT__

#include <app/core/action/base_action.hpp>
#include <util/image.hpp>

namespace VTX::App::Action::Export
{
	class Snapshot final : public App::Core::Action::BaseAction
	{
	  public:
		Snapshot( const FilePath, const Util::Image::E_FORMAT, const size_t, const size_t );
		Snapshot();
		void execute() override;

	  private:
		FilePath			  _path;
		Util::Image::E_FORMAT _format;
		size_t				  _width;
		size_t				  _height;
	};

} // namespace VTX::App::Action::Export
#endif

