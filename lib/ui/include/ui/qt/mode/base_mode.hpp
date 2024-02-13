#ifndef __VTX_UI_QT_MODE_BASE_MODE__
#define __VTX_UI_QT_MODE_BASE_MODE__

#include <memory>

namespace VTX::UI::QT::Mode
{
	class BaseMode
	{
	  public:
		BaseMode()			= default;
		virtual ~BaseMode() = default;

		virtual const std::string & getName() const = 0;

		virtual void enter() {}
		virtual void update( const float p_deltaTime ) {};
		virtual void exit() {};
	};
} // namespace VTX::UI::QT::Mode
#endif
