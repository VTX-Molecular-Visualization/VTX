#ifndef __VTX_UI_QT_WIDGET_LIBRARY_GRAPHICS_CONFIG_SECTION__
#define __VTX_UI_QT_WIDGET_LIBRARY_GRAPHICS_CONFIG_SECTION__

#include "ui/qt/services.hpp"
#include "ui/qt/widget/hideable_goupe_box.hpp"
#include <app/action/action_manager.hpp>
#include <app/action/graphics_config.hpp>
#include <app/services.hpp>

namespace VTX::UI::QT::Widget::Library::GraphicsConfig
{
	class Section : public HideableGroupBox
	{
	  public:
		Section( const QString & p_title, QWidget * const p_parent ) : HideableGroupBox( p_title, p_parent ) {}

	  protected:
		void _setCurrentPreset( const Entity p_preset ) { _currentPreset = p_preset; }

		Entity _getCurrentPreset() const { return _currentPreset; }

		template<VTX::Renderer::E_GRAPHICS_CONFIG_VALUES S, typename T>
		void _changeValue( const T & p_value )
		{
			using ChangeAction = ::VTX::App::Action::GraphicsConfig::Change<S, T>;
			ChangeAction action;
			::VTX::App::ACTION().execute( action, _currentPreset, p_value );
		}

	  private:
		Entity _currentPreset;
	};
} // namespace VTX::UI::QT::Widget::Library::GraphicsConfig

#endif
