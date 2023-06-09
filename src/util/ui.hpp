#ifndef __VTX_UTIL_UI__
#define __VTX_UTIL_UI__

#include "color/rgba.hpp"
#include <QBitmap>
#include <QColor>
#include <QComboBox>
#include <QEvent>
#include <QImage>
#include <QMenu>
#include <QPixmap>
#include <QString>
#include <QStyle>
#include <QVariant>
#include <QWidget>
#include <set>
#include <string>
#include <vector>

namespace VTX::Model
{
	class Bond;
}

namespace VTX::Util::UI
{
	class EventEater : public QObject
	{
		Q_OBJECT

	  public:
		EventEater( QEvent::Type p_type, QWidget * p_parent = nullptr ) : QObject( p_parent )
		{
			_filteredTypes.emplace( p_type );
		}
		EventEater( std::set<QEvent::Type> p_types, QWidget * p_parent = nullptr ) :
			QObject( p_parent ), _filteredTypes( p_types )
		{
		}

	  protected:
		bool eventFilter( QObject * p_obj, QEvent * p_event ) override
		{
			if ( _filteredTypes.find( p_event->type() ) != _filteredTypes.end() )
				return true;
			return QObject::eventFilter( p_obj, p_event );
		}

	  private:
		std::set<QEvent::Type> _filteredTypes = std::set<QEvent::Type>();
	};

	void fillComboBox( QComboBox * const p_comboBox, const std::vector<std::string> & p_values );
	void fillComboBox( QComboBox * const p_comboBox, const std::vector<QString> & p_values );
	void fillMenu( QMenu &							p_menu,
				   const int						p_enumSize,
				   const std::vector<std::string> & p_names,
				   const bool						p_actionCheckable = false );

	static void appendColorHtmlTag( QString & p_txt, const VTX::Color::Rgba & p_color )
	{
		const QString colorString = QString::fromStdString( p_color.toHexaString() );
		const QString tag		  = QString( "<font color=" + colorString + ">" );

		p_txt.append( tag );
	}
	static void appendEndColorHtmlTag( QString & p_txt ) { p_txt.append( "</font>" ); }

	static void setDynamicProperty( QWidget * const p_widget, const char * const p_property, const QVariant & p_value )
	{
		p_widget->setProperty( p_property, p_value );

		// Need manual refresh when property changed when widget is displayed
		p_widget->style()->unpolish( p_widget );
		p_widget->style()->polish( p_widget );
	}
	static void setDynamicProperty( QWidget * const p_widget, const char * const p_property )
	{
		setDynamicProperty( p_widget, p_property, true );
	}

	static Qt::CheckState getCheckState( const bool p_boolValue )
	{
		return p_boolValue ? Qt::CheckState::Checked : Qt::CheckState::Unchecked;
	}

	static void filterEventOnWidget( QWidget * const p_widget, const QEvent::Type & p_eventType )
	{
		EventEater * const eater = new EventEater( p_eventType, p_widget );
		p_widget->installEventFilter( eater );
	}
	static void filterEventOnWidget( QWidget * const p_widget, const std::set<QEvent::Type> & p_eventTypes )
	{
		EventEater * const eater = new EventEater( p_eventTypes, p_widget );
		p_widget->installEventFilter( eater );
	}

	static QColor RgbToQColor( const Color::Rgba & p_rgb )
	{
		return QColor( p_rgb.r * 255, p_rgb.g * 255, p_rgb.b * 255 );
	}

	static QBitmap generateAlphaMask( const QString & p_filepath )
	{
		return QBitmap( QPixmap::fromImage( QImage( ":/sprite/render/dihedral_angle_icon.png" ).createAlphaMask() ) );
	}

	void appendBondInfo( const Model::Bond & p_bond, QString & p_str );

} // namespace VTX::Util::UI

#endif
