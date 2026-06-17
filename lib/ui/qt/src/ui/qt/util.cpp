#include "ui/qt/util.hpp"
#include "ui/qt/resources.hpp"
#include "ui/qt/validator.hpp"
#include <QAction>
#include <QBoxLayout>
#include <QEvent>
#include <QFile>
#include <QFont>
#include <QHoverEvent>
#include <QPushButton>
#include <QScreen>
#include <QScrollArea>
#include <QToolButton>
#include <algorithm>

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

	class Popup : public QWidget
	{
	  public:
		Popup( QWidget * p_parent ) : QWidget( p_parent )
		{
			setWindowFlag( Qt::ToolTip );
			setObjectName( "questionMarkPopup" );
		}

		bool isHovered() const { return _hovered; }

		void leaveEvent( QEvent * event )
		{
			_hovered = false;
			QWidget::leaveEvent( event );
			hide();
		}

		void hoverLeave( QEvent * event )
		{
			_hovered = false;
			QWidget::leaveEvent( event );
			hide();
		}

		void hoverEnter( QHoverEvent * p_event )
		{
			_hovered = true;
			QWidget::event( p_event );
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
			default: break;
			}
			return QWidget::event( e );
		}

	  private:
		bool _hovered = false;
	};

	class QHoverableQuestionMark : public QPushButton
	{
		// Side length of the clickable question-mark button, in pixels.
		static constexpr int BUTTON_SIDE = 16;

		Popup *	 popup	= new Popup( this );
		QLabel * _label = nullptr;

	  public:
		QHoverableQuestionMark( const char * p_popupText ) : QPushButton()
		{
			// Render the help affordance as a bold "?" glyph rather than an icon. The old resource
			// ":/sprite/citations_icon_hovered.png" was removed (turning this flat button invisible), and a
			// style standard icon is just as fragile: it can be a low-contrast monochrome glyph that blends
			// into a themed panel, or be missing entirely. Text uses the palette's text colour, so it always
			// contrasts with whatever background the panel uses.
			setText( QStringLiteral( "?" ) );
			QFont glyphFont = font();
			glyphFont.setBold( true );
			setFont( glyphFont );

			setAttribute( Qt::WA_Hover );
			setFlat( true );
			setCursor( QCursor( Qt::CursorShape::WhatsThisCursor ) );
			setFixedSize( BUTTON_SIDE, BUTTON_SIDE );

			_label = new QLabel( p_popupText );
			_label->setTextFormat( Qt::RichText );
			_label->setOpenExternalLinks( true );
			_label->setWordWrap( true );
			// Let the user select/copy the content (useful for long engine output such as gromacs logs).
			_label->setTextInteractionFlags( Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse );

			// Large reports (e.g. raw gromacs output) used to grow the tooltip off-screen. Wrap the text in
			// a scroll area so the popup stays bounded and the overflow is scrollable instead.
			auto scrollArea = new QScrollArea( popup );
			scrollArea->setWidget( _label );
			scrollArea->setWidgetResizable( true );
			scrollArea->setFrameShape( QFrame::NoFrame );
			scrollArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
			scrollArea->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
			scrollArea->setContentsMargins( { 5, 5, 5, 5 } );

			auto layout = new QVBoxLayout( popup );
			layout->setContentsMargins( { 4, 4, 4, 4 } );
			layout->addWidget( scrollArea );
		}

		~QHoverableQuestionMark()
		{
			if ( popup )
			{
				delete popup;
			}
		}

		QHoverableQuestionMark( QHoverableQuestionMark && )					 = delete;
		QHoverableQuestionMark( const QHoverableQuestionMark & )			 = delete;
		QHoverableQuestionMark & operator=( QHoverableQuestionMark && )		 = delete;
		QHoverableQuestionMark & operator=( const QHoverableQuestionMark & ) = delete;

		void mouseClicked( QMouseEvent * p_event )
		{
			if ( popup == nullptr )
			{
				return;
			}
			if ( popup->isHidden() )
			{
				_showPopup();
			}
			else
			{
				popup->hide();
			}
		}

	  private:
		// Show the popup, capped to a fraction of the current screen and repositioned so it always stays
		// fully visible even when the report is very long.
		void _showPopup()
		{
			const QScreen * scr	  = screen();
			const QRect		avail = scr ? scr->availableGeometry() : QRect( 0, 0, 800, 600 );

			const QSize maxSize(
				std::min( 480, avail.width() - 2 * BUTTON_SIDE ), std::min( 360, avail.height() - 2 * BUTTON_SIDE )
			);
			// Constrain the label width so the text wraps (rather than forcing a horizontal scrollbar) and
			// let adjustSize() resolve the height; the popup max size then bounds the whole tooltip.
			_label->setMaximumWidth( maxSize.width() );
			popup->setMaximumSize( maxSize );
			popup->adjustSize();

			const QSize sz	= popup->size();
			QPoint		pos = mapToGlobal( rect().bottomRight() );
			if ( pos.x() + sz.width() > avail.right() )
			{
				pos.setX( avail.right() - sz.width() );
			}
			if ( pos.y() + sz.height() > avail.bottom() )
			{
				pos.setY( mapToGlobal( rect().topRight() ).y() - sz.height() );
			}
			pos.setX( std::max( pos.x(), avail.left() ) );
			pos.setY( std::max( pos.y(), avail.top() ) );

			popup->move( pos );
			popup->show();
		}

	  public:
		bool event( QEvent * e )
		{
			switch ( e->type() )
			{
			case QEvent::MouseButtonRelease: mouseClicked( reinterpret_cast<QMouseEvent *>( e ) ); break;
			default: break;
			}
			return QWidget::event( e );
		}
	};

	LabelWithHelper::LabelWithHelper(
		const char *					p_label,
		const char *					p_helper,
		const E_QUESTIONMARK_POSITION & p_postion
	) : container( new QWidget ), label( new QLabel( p_label ) )
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

	ObjectOwnership::ObjectOwnership( QObject * p_ ) noexcept : _obj( p_ ) {}

	void ObjectOwnership::release() noexcept { _obj.clear(); }

	ObjectOwnership::~ObjectOwnership()
	{
		if ( not _obj.isNull() )
		{
			delete _obj;
		}
	}

	ObjectOwnership::ObjectOwnership( ObjectOwnership && p_ ) noexcept : _obj( p_._obj ) { p_.release(); }

	ObjectOwnership & ObjectOwnership::operator=( QObject * p_ ) noexcept
	{
		if ( not _obj.isNull() )
		{
			delete _obj;
		}

		_obj = p_;

		return *this;
	}

	ObjectOwnership & ObjectOwnership::operator=( ObjectOwnership && p_ ) noexcept
	{
		if ( &p_ == this )
		{
			return *this;
		}

		if ( not _obj.isNull() )
		{
			delete _obj;
		}
		_obj = p_._obj;
		p_.release();
		return *this;
	}

	void get( const QLineEdit * p_src, uint64_t & p_dest ) noexcept
	{
		if ( p_src == nullptr )
		{
			return;
		}
		try
		{
			p_dest = std::stoull( p_src->text().toStdString() );
		}
		catch ( std::invalid_argument & )
		{
			p_dest = UINT64_MAX;
		}
		catch ( std::out_of_range & )
		{
			p_dest = UINT64_MAX;
		}
	}
} // namespace VTX::UI::QT::Util
