#ifndef __VTX_UI_WIDGET_COLLAPSING_HEADER_WIDGET__
#define __VTX_UI_WIDGET_COLLAPSING_HEADER_WIDGET__

#include "ui/old_ui/ui/widget/base_manual_widget.hpp"
#include <QBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPixmap>
#include <QPushButton>
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace CustomWidget
			{
				class CollapsingHeaderWidget : public BaseManualWidget<QWidget>
				{
					VTX_WIDGET

				  public:
					~CollapsingHeaderWidget() {};

					void setHeaderTitle( const QString & p_text ) { _titleLabel->setText( p_text ); };
					void setHeaderIcon( const QPixmap & p_pixmap ) { _iconLabel->setPixmap( p_pixmap ); };
					void displayIconInHeader( bool p_display ) { _iconLabel->setVisible( p_display ); };
					void setBody( QWidget * p_body )
					{
						_bodyWidget = p_body;
						_bodyWidget->setObjectName( objectName() + "_content" );
						_mainLayout->insertWidget( 1, _bodyWidget );
					};

					void showBody() { _bodyWidget->setVisible( true ); };
					void hideBody() { _bodyWidget->setVisible( false ); };
					void toggleBodyVisibility() { _bodyWidget->setVisible( !_bodyWidget->isVisible() ); };

					void setMenu( QMenu * const p_menu );

					QLabel & getHeaderLabelWidget() { return *_titleLabel; };
					void	 setHeaderHeight( const int p_height ) { _headerWidget->setFixedHeight( p_height ); };

					void localize() override;

				  protected:
					CollapsingHeaderWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {};
					void _setupUi( const QString & p_name ) override;
					void _setupSlots() override;

					void _displaySubmenu();

				  private:
					QVBoxLayout * _mainLayout	 = nullptr;
					QPushButton * _headerWidget	 = nullptr;
					QPushButton * _submenuButton = nullptr;
					QMenu *		  _submenu		 = nullptr;
					QHBoxLayout * _headerLayout	 = nullptr;
					QLabel *	  _titleLabel	 = nullptr;
					QLabel *	  _iconLabel	 = nullptr;
					QWidget *	  _bodyWidget	 = nullptr;
				};
			} // namespace CustomWidget
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
#endif
