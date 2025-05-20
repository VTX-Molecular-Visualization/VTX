#ifndef __VTX_UI_QT_UTIL__
#define __VTX_UI_QT_UTIL__

#include <QBitmap>
#include <QColor>
#include <QComboBox>
#include <QEvent>
#include <QImage>
#include <QLabel>
#include <QLayout>
#include <QMenu>
#include <QPixmap>
#include <QPointer>
#include <QString>
#include <QStyle>
#include <QVariant>
#include <QWidget>
#include <qformlayout.h>
#include <qlineedit.h>
#include <set>
#include <string>
#include <util/color/rgba.hpp>
#include <vector>

namespace VTX::UI::QT::Util
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
	/*
	static QVariant widgetKeyToQVariant( const UI::Core::WidgetKey & p_key )
	{
		return QVariant( QString::fromStdString( p_key ) );
	}
	static UI::Core::WidgetKey QVariantTowWidgetKey( const QVariant & p_variant )
	{
		return static_cast<UI::Core::WidgetKey>( p_variant.toString().toStdString() );
	}
	*/

	void fillComboBox( QComboBox * const p_comboBox, const std::vector<std::string> & p_values );
	void fillComboBox( QComboBox * const p_comboBox, const std::vector<QString> & p_values );
	void fillMenu(
		QMenu &							 p_menu,
		const int						 p_enumSize,
		const std::vector<std::string> & p_names,
		const bool						 p_actionCheckable = false
	);

	static void appendColorHtmlTag( QString & p_txt, const VTX::Util::Color::Rgba & p_color )
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

	static QColor RgbToQColor( const VTX::Util::Color::Rgba & p_rgb )
	{
		return QColor( p_rgb.r * 255, p_rgb.g * 255, p_rgb.b * 255 );
	}

	static QBitmap generateAlphaMask( const QString & p_filepath )
	{
		return QBitmap::fromPixmap( QPixmap::fromImage( QImage( p_filepath ).createAlphaMask() ) );
	}

	// Class responsible for creating a label with tooltip as a question mark ? button next to it.
	// Implicitly convert in QWidget pointer which refers to the container of both the label and the question mark.
	class LabelWithHelper
	{
	  public:
		enum class E_QUESTIONMARK_POSITION
		{
			left,
			right
		};
		LabelWithHelper() = default;
		LabelWithHelper( const char * p_label, const char * p_helper, const E_QUESTIONMARK_POSITION & p_postion );
		QPointer<QWidget> container = nullptr;
		QLabel *		  label		= nullptr;
		operator QWidget *();
	};

	// Create a standardized separator with a slightly larger label on its left. Aim to split form sections.
	void addLabeledHLineSeparator( QBoxLayout * p_dest, const char * p_label ) noexcept;

	// The idea is to create a lineEdit field with a validator that enforce the UInt64 size input
	QLineEdit * addUInt64Field( QFormLayout * p_dest, const char * p_label, const char * p_tooltip ) noexcept;

	// Class responsible for deleting a widget on deletion. This class will follow ownership transfer for move
	// sementics and won't be copyable.
	class ObjectOwnership
	{
		QPointer<QObject> _obj = nullptr;

	  public:
		ObjectOwnership() = default;
		ObjectOwnership( QObject * ) noexcept;
		void release() noexcept;

		ObjectOwnership & operator=( QObject * ) noexcept;

		~ObjectOwnership();
		ObjectOwnership( ObjectOwnership && ) noexcept;
		ObjectOwnership & operator=( ObjectOwnership && ) noexcept;
		ObjectOwnership( const ObjectOwnership & )			   = delete;
		ObjectOwnership & operator=( const ObjectOwnership & ) = delete;
	};

	// Extract uint64 number from input textbox, or UINT64_MAX if the number couldn't be red. Do nothing if src is
	// nullptr.
	void get( const QLineEdit * p_src, uint64_t & p_dest ) noexcept;

	/**
	 * @brief Namespace containing event types that are defined by VTX for diverse purpose. Having all custom events
	 * here allows to quickly see if two values overlap so nothing clashes.
	 */
	namespace CustomEvent
	{
		/**
		 * @brief Event type used by the console so post an event responsible for scrolling down to the bottom.
		 */
		const QEvent::Type ConsoleScrollToBottom = static_cast<QEvent::Type>( 1001 );
	} // namespace CustomEvent

} // namespace VTX::UI::QT::Util

#endif
