#ifndef __VTX_UI_QT_DOCK_WIDGET_INSPECTOR__
#define __VTX_UI_QT_DOCK_WIDGET_INSPECTOR__

#include "ui/qt/dock_widget/base_dock_widget.hpp"
#include "ui/qt/widget/library/color_layout.hpp"
#include "ui/qt/widget/library/graphics_config.hpp"
#include "ui/qt/widget/library/representation.hpp"
#include "ui/qt/widget/selection.hpp"
#include <QComboBox>

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
		QPointer<QWidget> _filler;
		QPointer<Widget::Selection>				  _selectionListWidget;
		QPointer<Widget::Library::GraphicsConfig> _graphicsConfigWidget;
		QPointer<Widget::Library::ColorLayout>	  _colorLayoutWidget;
		QPointer<Widget::Library::Representation> _representationWidget;

		/**
		 * @brief Clear all widgets except the filler.
		 */
		void _clear();

		bool _hasSystemSelection() const;
		void _onSystemSelectionUpdated( App::ECS::Registry &, App::ECS::Entity );
	};

} // namespace VTX::UI::QT::DockWidget

#endif
