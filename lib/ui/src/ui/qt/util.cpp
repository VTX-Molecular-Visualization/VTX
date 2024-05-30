#include "ui/qt/util.hpp"
#include "ui/qt/validator.hpp"
#include <QAction>
#include <qevent.h>
#include <qfont.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>

namespace VTX::UI::QT::Util
{
	void fillComboBox( QComboBox * const p_comboBox, const std::vector<std::string> & p_values )
	{
		for ( const std::string & str : p_values )
		{
			p_comboBox->addItem( QString::fromStdString( str ) );
		}
	}
	void fillComboBox( QComboBox * const p_comboBox, const std::vector<QString> & p_values )
	{
		for ( const QString & str : p_values )
		{
			p_comboBox->addItem( str );
		}
	}

	void fillMenu(
		QMenu &							 p_menu,
		const int						 p_enumSize,
		const std::vector<std::string> & p_names,
		const bool						 p_actionCheckable
	)
	{
		for ( int i = 0; i < p_enumSize; i++ )
		{
			QAction * const action = new QAction( QString::fromStdString( p_names[ i ] ), &p_menu );
			action->setData( i );
			action->setCheckable( p_actionCheckable );

			p_menu.addAction( action );
		}
	}
	class QHoverableQuestionMark : public QPushButton
	{
		int		  m_count = 0;
		QWidget * popup	  = nullptr;

	  public:
		QHoverableQuestionMark( const char * p_popupText ) : QPushButton()
		{
			setIcon( QIcon( ":/sprite/citations_icon_hovered.png" ) );
			setAttribute( Qt::WA_Hover );
			setFlat( true );
			setMaximumHeight( iconSize().height() );
			setMaximumWidth( iconSize().width() );
			setIconSize( iconSize() * 0.75 );
			setCursor( QCursor( Qt::CursorShape::WhatsThisCursor ) );
			auto label = new QLabel( p_popupText );
			label->setTextFormat( Qt::RichText );

			popup = new QWidget;
			popup->setAttribute( Qt::WA_TransparentForMouseEvents );
			popup->setWindowFlag( Qt::ToolTip );
			// popup->setWindowFlag( Qt::WindowTransparentForInput );
			auto layout = new QHBoxLayout( popup );
			layout->addWidget( label );
		}
		void enterEvent( QEnterEvent * event ) { QWidget::enterEvent( event ); }

		void leaveEvent( QEvent * event ) { QWidget::leaveEvent( event ); }
		void hoverEnter( QHoverEvent * p_event )
		{
			auto p = p_event->globalPosition().toPoint();
			p += QPoint( 5, 5 );
			popup->move( p );
			popup->show();
		}

		void hoverLeave( QHoverEvent * event ) { popup->hide(); }

		void hoverMove( QHoverEvent * p_event )
		{
			auto p = p_event->globalPosition().toPoint();
			p += QPoint( 5, 5 );
			popup->move( p );
		}
		bool event( QEvent * e )
		{
			switch ( e->type() )
			{
			case QEvent::HoverEnter:
				hoverEnter( reinterpret_cast<QHoverEvent *>( e ) );
				return true;
				break;
			case QEvent::HoverLeave:
				hoverLeave( reinterpret_cast<QHoverEvent *>( e ) );
				return true;
				break;
			case QEvent::HoverMove:
				hoverMove( reinterpret_cast<QHoverEvent *>( e ) );
				return true;
				break;
			default: break;
			}
			return QWidget::event( e );
		}
	};
	LabelWithHelper::LabelWithHelper(
		const char *					p_label,
		const char *					p_helper,
		const E_QUESTIONMARK_POSITION & p_postion
	) :
		container( new QWidget ),
		label( new QLabel( p_label ) )
	{
		QHBoxLayout *			 layout		  = new QHBoxLayout( container );
		QHoverableQuestionMark * questionMark = new QHoverableQuestionMark( p_helper );
		layout->setContentsMargins( { 0, 0, 0, 0 } );

		if ( p_postion == E_QUESTIONMARK_POSITION::left )
		{
			layout->addWidget( questionMark );
			layout->addWidget( label, 1 );
		}
		else
		{
			layout->addWidget( label, 1 );
			layout->addWidget( questionMark );
		}
	}
	LabelWithHelper::operator QWidget *() { return container; }

	QLabel * createLabelWithHelpTooltip( const char * p_label, const char * p_helpTooltip ) noexcept
	{
		QLabel * wlabel = new QLabel( p_label );
		wlabel->setToolTip( p_helpTooltip );
		wlabel->setCursor( QCursor( Qt::CursorShape::WhatsThisCursor ) );
		return wlabel;
	}
	void addLabeledHLineSeparator( QBoxLayout * p_dest, const char * p_label ) noexcept
	{
		QHBoxLayout * qLilLayout = new QHBoxLayout;
		p_dest->addLayout( qLilLayout );
		QLabel * qlabel = new QLabel( p_label );
		QFont	 font	= qlabel->font();
		font.setPointSize( font.pointSize() + 4 );
		qlabel->setFont( font );
		auto qLine = new QFrame;
		qLine->setFrameShape( QFrame::HLine );
		qLine->setFrameShadow( QFrame::Sunken );
		qLilLayout->addWidget( qlabel );
		qLilLayout->addWidget( qLine, 1 );
	}
	QLineEdit * addUInt64Field( QFormLayout * p_dest, const char * p_label, const char * p_tooltip ) noexcept
	{
		QLineEdit * out = new QLineEdit();
		out->setValidator( new VTX::UI::QT::QUInt64Validator( out ) );

		p_dest->addRow(
			VTX::UI::QT::Util::LabelWithHelper(
				p_label, p_tooltip, VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			),
			out
		);
		return out;
	}

	void addUInt64FieldWithRightLabel(
		QFormLayout * p_dest,
		const char *  p_label,
		const char *  p_tooltip,
		QLineEdit **  p_out_field,
		QLabel **	  p_out_label
	) noexcept
	{
		*p_out_field				  = new QLineEdit();
		*p_out_label				  = new QLabel;
		QWidget *	  qRightCol		  = new QWidget;
		QHBoxLayout * qRightColLayout = new QHBoxLayout( qRightCol );
		qRightColLayout->setContentsMargins( 0, 0, 0, 0 );
		qRightColLayout->addWidget( *p_out_field, 1 );
		qRightColLayout->addWidget( *p_out_label, 1 );

		( *p_out_field )->setValidator( new VTX::UI::QT::QUInt64Validator( *p_out_field ) );

		p_dest->addRow( UI::QT::Util::createLabelWithHelpTooltip( p_label, p_tooltip ), qRightCol );
	}
} // namespace VTX::UI::QT::Util
