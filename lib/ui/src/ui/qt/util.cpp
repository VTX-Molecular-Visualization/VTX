#include "ui/qt/util.hpp"
#include <QAction>

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

} // namespace VTX::UI::QT::Util
