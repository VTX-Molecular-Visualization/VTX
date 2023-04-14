#ifndef __VTX_UI_WIDGET_REPRESENTABLE_FIELD_WIDGET__
#define __VTX_UI_WIDGET_REPRESENTABLE_FIELD_WIDGET__

#include "old_ui/ui/widget/base_manual_widget.hpp"
#include <QBoxLayout>
#include <QDragEnterEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QPixmap>
#include <QPushButton>
#include <QWidget>
#include <app/old_app/generic/base_representable.hpp>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace CustomWidget
			{
				class RepresentableFieldWidget : public BaseManualWidget<QLabel>
				{
					Q_OBJECT
					VTX_WIDGET

				  public:
					~RepresentableFieldWidget() {};
					void localize() override;

					inline Generic::BaseRepresentable * const getPreviousRepresentable() { return _previousValue; };
					inline const Generic::BaseRepresentable * const getPreviousRepresentable() const
					{
						return _previousValue;
					};
					inline Generic::BaseRepresentable * const		getRepresentable() { return _representable; };
					inline const Generic::BaseRepresentable * const getRepresentable() const { return _representable; };
					inline void setValue( Generic::BaseRepresentable * const p_representable )
					{
						if ( _representable != p_representable )
						{
							_previousValue = _representable;
							_representable = p_representable;

							emit dataChanged();
						}
					};

				  signals:
					void dataChanged();

				  protected:
					RepresentableFieldWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {};
					void _setupUi( const QString & p_name ) override;
					void _setupSlots() override;

					void dragEnterEvent( QDragEnterEvent * event ) override;
					void dropEvent( QDropEvent * event ) override;

				  private:
					Generic::BaseRepresentable * _previousValue = nullptr;
					Generic::BaseRepresentable * _representable = nullptr;
				};
			} // namespace CustomWidget
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
#endif
