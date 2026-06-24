#ifndef __VTX_UI_QT_DOCK_WIDGET_INSPECTOR__
#define __VTX_UI_QT_DOCK_WIDGET_INSPECTOR__

#include "ui/qt/dock_widget/base_dock_widget.hpp"
#include "ui/qt/widget/library/color_layout.hpp"
#include "ui/qt/widget/library/graphics_config_widget.hpp"
#include "ui/qt/widget/library/representation.hpp"
#include "ui/qt/widget/selection.hpp"
#include "ui/qt/widget/viewpoint.hpp"
#include <QToolButton>

namespace VTX::UI::QT::DockWidget
{
	/**
	 * @brief Inspector dock widget showing widgets about the current selection.
	 */
	class Inspector : public BaseDockWidget<Inspector>
	{
	  public:
		Inspector( QWidget * );

	  private:
		QPointer<QToolButton>							_lockButton;
		QPointer<QWidget>								_filler;
		QPointer<Widget::Selection>						_selectionListWidget;
		QPointer<Widget::Library::GraphicsConfigWidget> _graphicsConfigWidget;
		QPointer<Widget::Library::ColorLayout>			_colorLayoutWidget;
		QPointer<Widget::Library::Representation>		_representationWidget;
		QPointer<Widget::ViewPoint>						_viewPointWidget;

		/**
		 * @brief Clear all widgets except the filler.
		 */
		void _clear();

		bool _hasSystemSelection() const;
		void _onSystemSelectionUpdated( Registry &, Entity );
	};

} // namespace VTX::UI::QT::DockWidget

#endif
