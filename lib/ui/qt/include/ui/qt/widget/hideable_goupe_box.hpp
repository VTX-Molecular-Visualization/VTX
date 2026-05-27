#ifndef __VTX_UI_QT_WIDGET_HIDEABLE_GROUP_BOX__
#define __VTX_UI_QT_WIDGET_HIDEABLE_GROUP_BOX__

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
		explicit HideableGroupBox( const QString & p_title, QWidget * p_parent ) : QWidget( p_parent )
		{
			_toggleButton = new QToolButton( this );
			_toggleButton->setText( p_title );
			_toggleButton->setCheckable( true );
			_toggleButton->setChecked( false );
			_toggleButton->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
			_toggleButton->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
			_toggleButton->setArrowType( Qt::RightArrow );

			_groupbox = new QGroupBox( this );
			_groupbox->setVisible( false );
			auto * layout	  = new QVBoxLayout( _groupbox );
			auto * mainLayout = new QVBoxLayout( this );

			mainLayout->setContentsMargins( 0, 0, 0, 0 );
			// mainLayout->setSpacing( 0 );

			mainLayout->addWidget( _toggleButton );
			mainLayout->addWidget( _groupbox );

			connect(
				_toggleButton,
				&QToolButton::toggled,
				this,
				[ this ]( const bool p_visible )
				{
					_groupbox->setVisible( p_visible );
					_toggleButton->setArrowType( p_visible ? Qt::DownArrow : Qt::RightArrow );
					emit toggled( p_visible );
				}
			);
		}

		inline void setChecked( const bool p_checked ) { _toggleButton->setChecked( p_checked ); }

		inline bool isChecked() const { return _toggleButton->isChecked(); }

		inline void addWidget( QWidget * widget ) { _groupbox->layout()->addWidget( widget ); }

		inline void freeze( const bool p_freeze ) { _toggleButton->blockSignals( p_freeze ); }

		inline void setTitle( const QString & p_title ) { _toggleButton->setText( p_title ); }

	  signals:
		void toggled( bool );

	  private:
		QPointer<QToolButton> _toggleButton;
		QPointer<QGroupBox>	  _groupbox;
	};
} // namespace VTX::UI::QT::Widget
#endif
