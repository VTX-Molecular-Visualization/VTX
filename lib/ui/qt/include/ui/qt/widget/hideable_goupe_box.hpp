#ifndef VTX_UI_QT_WIDGET_HIDEABLE_GOUPE_BOX_HPP
#define VTX_UI_QT_WIDGET_HIDEABLE_GOUPE_BOX_HPP

#include "ui/qt/helper.hpp"
#include <QGroupBox>
#include <QPointer>
#include <QToolButton>
#include <QVBoxLayout>

namespace VTX::UI::QT::Widget
{

	/**
	 * @brief QGroupBox that hide content when unselected.
	 */
	class HideableGroupBox : public QWidget
	{
		Q_OBJECT

	  public:
		explicit HideableGroupBox( const QString & p_title, QWidget * const p_parent ) : QWidget( p_parent )
		{
			_toggleButton = Helper::makeQObject<QToolButton>( this );
			_toggleButton->setText( p_title );
			_toggleButton->setCheckable( true );
			_toggleButton->setChecked( false );
			_toggleButton->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
			_toggleButton->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
			_toggleButton->setArrowType( Qt::RightArrow );

			_groupbox = Helper::makeQObject<QGroupBox>( this );
			_groupbox->setVisible( false );
			_groupbox->setLayout( Helper::makeQObject<QVBoxLayout>( _groupbox ) );
			auto * mainLayout = Helper::makeQObject<QVBoxLayout>( this );

			mainLayout->setContentsMargins( 0, 0, 0, 0 );
			// mainLayout->setSpacing( 0 );

			mainLayout->addWidget( _toggleButton );
			mainLayout->addWidget( _groupbox );

			connect( _toggleButton,
					 &QToolButton::toggled,
					 this,
					 [ this ]( const bool p_visible )
					 {
						 _groupbox->setVisible( p_visible );
						 _toggleButton->setArrowType( p_visible ? Qt::DownArrow : Qt::RightArrow );
						 emit toggled( p_visible );
					 } );
		}

		void setChecked( const bool p_checked ) { _toggleButton->setChecked( p_checked ); }

		bool isChecked() const { return _toggleButton->isChecked(); }

		void addWidget( QWidget * widget ) { _groupbox->layout()->addWidget( widget ); }

		void freeze( const bool p_freeze ) { _toggleButton->blockSignals( p_freeze ); }

		void setTitle( const QString & p_title ) { _toggleButton->setText( p_title ); }

	  signals:
		void toggled( bool );

	  private:
		QPointer<QToolButton> _toggleButton;
		QPointer<QGroupBox>	  _groupbox;
	};
} // namespace VTX::UI::QT::Widget
#endif
