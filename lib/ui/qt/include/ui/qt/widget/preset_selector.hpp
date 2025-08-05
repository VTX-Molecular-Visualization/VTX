#ifndef __VTX_UI_QT_WIDGET_PRESET_SELECTOR__
#define __VTX_UI_QT_WIDGET_PRESET_SELECTOR__

#include "ui/qt/actions.hpp"
#include "ui/qt/core/widget/actionable_push_button.hpp"
#include <QApplication>
#include <QComboBox>
#include <QGridLayout>
#include <QPushButton>
#include <app/action/library.hpp>
#include <app/core/library/library_system.hpp>

namespace VTX::UI::QT::Widget
{
	/**
	 * @brief Class responsible for displaying a combo box to select a preset, add and remove.
	 */
	template<typename T>
	class PresetSelector : public QWidget
	{
	  public:
		PresetSelector( QWidget * p_parent ) : QWidget( p_parent )
		{
			auto * layout	= new QGridLayout( this );
			auto * comboBox = new QComboBox( this );
			layout->addWidget( comboBox, 0, 0, 1, 3 );
			layout->setContentsMargins( 0, 0, 0, 0 );

			for ( const auto & [ name, preset ] : _library->getItems() )
			{
				comboBox->addItem( QString::fromStdString( name ) );
			}

			using namespace Core::Widget;
			using namespace Action;

			auto * actionAdd = Factory::get<Preset::Add<T>>();

			auto * btnNew		 = new ActionablePushButton( actionAdd, this );
			auto * btnDupplicate = new ActionablePushButton( Factory::get<Preset::Duplicate<T>>(), this );
			auto * btnDelete	 = new ActionablePushButton( Factory::get<Preset::Delete<T>>(), this );

			layout->addWidget( btnNew, 1, 0 );
			layout->addWidget( btnDupplicate, 1, 1 );
			layout->addWidget( btnDelete, 1, 2 );
		}

	  private:
		App::Core::Library::Library<T> * const _library = App::LIBRARY_SYSTEM().getLibrary<T>();
	};
} // namespace VTX::UI::QT::Widget
#endif
