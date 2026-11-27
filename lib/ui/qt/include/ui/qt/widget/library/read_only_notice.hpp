#ifndef VTX_UI_QT_WIDGET_LIBRARY_READ_ONLY_NOTICE_HPP
#define VTX_UI_QT_WIDGET_LIBRARY_READ_ONLY_NOTICE_HPP

#include "ui/qt/helper.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
#include <QLabel>
#include <QPointer>
#include <QPushButton>
#include <QVBoxLayout>
#include <app/ecs.hpp>
#include <app/generic/readonly.hpp>
#include <app/services.hpp>

namespace VTX::UI::QT::Widget::Library
{
	/**
	 * @brief Notice displayed when a read-only item is selected, with a button to duplicate it.
	 */
	class ReadOnlyNotice final : public QWidget
	{
	  public:
		explicit ReadOnlyNotice(
			QWidget * const p_parent,
			const QString & p_message = "Built-in presets are read-only\nDuplicate to edit",
			const QString & p_tip	  = "Create an editable copy of this preset"
		) : QWidget( p_parent )
		{
			auto * const layout = Helper::makeQObject<QVBoxLayout>( this );
			layout->setContentsMargins( 0, 0, 0, 0 );

			auto * const label = Helper::makeQObject<QLabel>( p_message, this );
			label->setAlignment( Qt::AlignCenter );
			label->setWordWrap( true );

			_button = Helper::makeQObject<QPushButton>( "Edit", this );
			_button->setIcon( STYLE().iconFromCodepoint( Style::Icons::COPY ) );
			_button->setToolTip( p_tip );

			layout->addWidget( label );
			layout->addWidget( _button );

			setVisible( false );
		}

		QPushButton * button() const { return _button; }

		static bool isReadOnly( const Entity p_preset )
		{ return App::REG().valid( p_preset ) && App::REG().all_of<App::Generic::ReadOnly>( p_preset ); }

	  private:
		QPointer<QPushButton> _button;
	};
} // namespace VTX::UI::QT::Widget::Library
#endif
