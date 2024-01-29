#ifndef __VTX_UI_QT_CONTROLLER_BASE_CONTROLLER__
#define __VTX_UI_QT_CONTROLLER_BASE_CONTROLLER__

#include <QWidget>
#include <memory>
#include <string>
#include <util/hashing.hpp>
#include <utility>

namespace VTX::UI::QT::Controller
{
	class BaseController
	{
	  public:
		BaseController()		  = default;
		virtual ~BaseController() = default;

		virtual void update( const float & p_deltaTime ) {};

		inline bool	 isActive() const { return _isActive; }
		virtual void setActive( const bool p_active ) { _isActive = p_active; }

		inline void setWidgetTarget( const QWidget * const p_widget ) { _widgetTarget = p_widget; }

		inline bool isTargetWidgetFocused() const { return _widgetTarget == nullptr || _widgetTarget->hasFocus(); }

		virtual Util::Hashing::Hash getHashedCollectionID() const = 0;

	  protected:
		inline const QWidget * _getWidgetTarget() { return _widgetTarget; }

	  private:
		const QWidget * _widgetTarget = nullptr;
		bool			_isActive	  = true;
	};
} // namespace VTX::UI::QT::Controller
#endif
