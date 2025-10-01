#ifndef __VTX_APP_ACTION_APPLICATION__
#define __VTX_APP_ACTION_APPLICATION__

#include "app/action/base_action.hpp"
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Action::Application
{
	class NewScene final : public BaseAction
	{
	  public:
		void execute() override;
	};

	class Quit final : public BaseAction
	{
	  public:
		Quit() {};
		void execute() override;
	};

	class Resize final : public BaseAction
	{
	  public:
		Resize( const size_t p_width, const size_t p_height ) : _width( p_width ), _height( p_height ) {}
		void execute() override;

	  private:
		const size_t _width;
		const size_t _height;
	};

} // namespace VTX::App::Action::Application
#endif
