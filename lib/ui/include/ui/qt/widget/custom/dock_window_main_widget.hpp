#ifndef __VTX_UI_QT_WIDGET_CUSTOM_DOCK_WINDOW_MAIN_WIDGET__
#define __VTX_UI_QT_WIDGET_CUSTOM_DOCK_WINDOW_MAIN_WIDGET__

#include <QSize>
#include <QWidget>
#include <optional>

namespace VTX::UI::QT::Widget::Custom
{
	class DockWindowMainWidget : public QWidget
	{
	  public:
		DockWindowMainWidget( const QSize & p_sizeHint, const QSize & p_minimumSizeHint, QWidget * p_parent = nullptr );
		DockWindowMainWidget( QWidget * p_parent = nullptr );

		QSize sizeHint() const override;
		QSize minimumSizeHint() const override;

		void setSizeHint( const QSize & p_sizeHint );
		void setMinimumSizeHint( const QSize & p_sizeHint );

	  private:
		std::optional<QSize> _overriddenSizeHint		= std::nullopt;
		std::optional<QSize> _overriddenMinimumSizeHint = std::nullopt;
	};

} // namespace VTX::UI::QT::Widget::Custom
#endif
