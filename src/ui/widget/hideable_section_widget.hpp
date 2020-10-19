#ifndef __VTX_UI_WIDGET_HIDEABLE_SECTION_WIDGET__
#define __VTX_UI_WIDGET_HIDEABLE_SECTION_WIDGET__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_manual_widget.hpp"
#include <QBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class HideableSectionWidget : public BaseManualWidget<QWidget>
			{
				VTX_MANUAL_WIDGET_DECLARATION

			  public:
				~HideableSectionWidget() {};

				void setHeaderTitle( const QString & p_text ) { _titleLabel->setText( p_text ); };
				void setHeaderIcon( const QPixmap & p_pixmap ) { _iconLabel->setPixmap( p_pixmap ); };
				void displayIconInHeader( bool p_display ) { _iconLabel->setVisible( p_display ); };
				void setSection( QWidget * p_section )
				{
					_sectionWidget = p_section;
					_sectionWidget->setObjectName( objectName() + "_content" );
					_mainLayout->insertWidget( 1, _sectionWidget );
				};

				void showSection() { _sectionWidget->setVisible( true ); };
				void hideSection() { _sectionWidget->setVisible( false ); };
				void toggleSectionVisibility() { _sectionWidget->setVisible( !_sectionWidget->isVisible() ); };

				QLabel & getHeaderLabelWidget() { return *_titleLabel; };
				void	 setHeaderHeight( const int p_height ) { _headerWidget->setFixedHeight( p_height ); };

			  protected:
				HideableSectionWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {};
				void _setupUi( const QString & p_name ) override;
				void _setupSlots() override;
				void localize() override;

			  private:
				QVBoxLayout * _mainLayout	 = nullptr;
				QPushButton * _headerWidget	 = nullptr;
				QHBoxLayout * _headerLayout	 = nullptr;
				QLabel *	  _titleLabel	 = nullptr;
				QLabel *	  _iconLabel	 = nullptr;
				QWidget *	  _sectionWidget = nullptr;
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
#endif
