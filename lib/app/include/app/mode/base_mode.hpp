#ifndef __VTX_APP_MODE_BASE_MODE__
#define __VTX_APP_MODE_BASE_MODE__

namespace VTX::App::Mode
{
	class BaseMode
	{
	  public:
		virtual ~BaseMode() = default;

		virtual void enter() = 0;
		virtual void exit()	 = 0;
	};

} // namespace VTX::App::Mode

#endif
