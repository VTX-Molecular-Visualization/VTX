#ifndef __VTX_UI_WIDGET_RENDER_OVERLAY_VISUALIZATION_QUICK_ACCESS__
#define __VTX_UI_WIDGET_RENDER_OVERLAY_VISUALIZATION_QUICK_ACCESS__

#include "base_overlay.hpp"
#include "id.hpp"
#include "ui/multi_data_field.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include <QAction>
#include <QComboBox>
#include <QFrame>
#include <QToolButton>
#include <vector>

namespace VTX::UI::Widget::Render::Overlay
{
	class VisualizationQuickAccess : public BaseOverlay
	{
		Q_OBJECT
		VTX_WIDGET

	  private:
		template<typename T>
		class MenuItemData
		{
		  public:
			MenuItemData() : ControllerData( T(), "", "" ) {};
			MenuItemData( const T & p_data, const QString & p_name, const QString & p_iconPath ) :
				data( p_data ), name( p_name ), iconPath( p_iconPath )
			{
			}

			const T & data;
			const QString	   name;
			const QString	   iconPath;
		};


		inline static std::vector<MenuItemData<ID::VTX_ID>> CONTROLLERS {
			MenuItemData<ID::VTX_ID>( ID::Controller::TRACKBALL, "Trackball", ":/sprite/camera_trackball_mode.png" ),
			MenuItemData<ID::VTX_ID>( ID::Controller::FREEFLY, "Freefly", ":/sprite/camera_freefly_mode.png" )
		};

		inline static const char * CONTROLLER_PROPERTY_NAME = "CONTROLLER";

	  public:
		~VisualizationQuickAccess() = default;
		void localize() override;

		void receiveEvent( const Event::VTXEvent & p_event ) override;

	  protected:
		VisualizationQuickAccess( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;
		void _refreshController();

	  private:
		QToolButton * _orientWidget;
		QToolButton * _changeControllerWidget;

		void _orientAction();
		void _changeControllerAction( const QAction * const p_action );
	};
} // namespace VTX::UI::Widget::Render::Overlay
#endif
