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
	 * @brief Base class for preset selectors to permit signals (not possible on templated class).
	 */
	class BasePresetSelector : public QWidget
	{
		Q_OBJECT

	  public:
		BasePresetSelector( QWidget * p_parent ) : QWidget( p_parent ) {}
		virtual ~BasePresetSelector() = default;

	  signals:
		void presetChanged( const QString & p_name );
	};

	/**
	 * @brief Class responsible for displaying a combo box to select a preset, add and remove.
	 */
	template<typename T>
	class PresetSelector : public BasePresetSelector
	{
	  public:
		PresetSelector( QWidget * p_parent ) : BasePresetSelector( p_parent )
		{
			auto * layout = new QGridLayout( this );
			layout->setContentsMargins( 0, 0, 0, 0 );

			_comboBox = new QComboBox( this );
			_refreshComboBox();
			_comboBox->setCurrentIndex( 0 );
			layout->addWidget( _comboBox, 0, 0, 1, 3 );
			connect( _comboBox, &QComboBox::currentTextChanged, this, &PresetSelector::presetChanged );

			using namespace Core::Widget;
			using namespace Action;

			auto * btnNew		 = new ActionablePushButton( Factory::get<Preset::Add<T>>(), this );
			auto * btnDupplicate = new ActionablePushButton( Factory::get<Preset::Duplicate<T>>(), this );
			auto * btnDelete	 = new ActionablePushButton( Factory::get<Preset::Delete<T>>(), this );

			layout->addWidget( btnNew, 1, 0 );
			layout->addWidget( btnDupplicate, 1, 1 );
			layout->addWidget( btnDelete, 1, 2 );

			connect(
				btnDupplicate,
				&QPushButton::clicked,
				[ this ]()
				{
					App::ACTION_SYSTEM().execute<App::Action::Library::DuplicatePreset<T>>(
						_comboBox->currentText().toStdString()
					);
				}
			);
			connect(
				btnDelete,
				&QPushButton::clicked,
				[ this ]()
				{
					App::ACTION_SYSTEM().execute<App::Action::Library::DeletePreset<T>>(
						_comboBox->currentText().toStdString()
					);
				}
			);

			_library->onPresetAdded += [ this ]( std::string_view p_name )
			{
				_refreshComboBox();
				_comboBox->setCurrentText( QString::fromStdString( std::string( p_name ) ) );
			};
			_library->onPresetRenamed += [ this ]( std::string_view )
			{
				QString selectedName = _comboBox->currentText();
				_refreshComboBox();
				_comboBox->setCurrentText( selectedName );
			};
			_library->onPresetDeleted += [ this ]( std::string_view )
			{
				_refreshComboBox();
				assert( _comboBox->count() > 0 );
				_comboBox->setCurrentIndex( 0 );
				// Emit the signal to notify that the preset has changed.
				emit presetChanged( _comboBox->currentText() );
			};
		}

	  private:
		App::Core::Library::Library<T> * const _library = App::LIBRARY_SYSTEM().getLibrary<T>();
		QPointer<QComboBox>					   _comboBox;

		void _refreshComboBox()
		{
			_comboBox->blockSignals( true );

			_comboBox->clear();
			for ( const auto & [ name, _ ] : _library->getItems() )
			{
				_comboBox->addItem( QString::fromStdString( name ) );
			}

			_comboBox->blockSignals( false );
		}

	  public:
	};
} // namespace VTX::UI::QT::Widget
#endif
