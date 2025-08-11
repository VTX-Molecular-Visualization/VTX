#ifndef __VTX_UI_QT_WIDGET_PRESET_SELECTOR__
#define __VTX_UI_QT_WIDGET_PRESET_SELECTOR__

#include "ui/qt/actions.hpp"
#include "ui/qt/core/widget/actionable_push_button.hpp"
#include <QApplication>
#include <QComboBox>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <app/action/library.hpp>
#include <app/core/library/library_system.hpp>

namespace VTX::UI::QT::Widget::Library
{
	/**
	 * @brief Base class for preset selectors to permit signals (not possible on templated class).
	 */
	class BasePresetSelector : public QGroupBox
	{
		Q_OBJECT

	  public:
		BasePresetSelector( QWidget * p_parent ) : QGroupBox( p_parent ) {}
		virtual ~BasePresetSelector() = default;

	  signals:
		void presetChanged( const QString & p_name );
	};

	/**
	 * @brief Class responsible for displaying a combo box to select a preset, add and remove.
	 */
	template<App::Core::Library::ConceptPreset P>
	class PresetSelector : public BasePresetSelector
	{
	  public:
		PresetSelector( QWidget * p_parent ) : BasePresetSelector( p_parent )
		{
			// auto * groupBox = new QGroupBox( "Presets" );
			// auto * layout	= new QVBoxLayout( groupBox );
			setTitle( "Presets" );

			auto * layout = new QGridLayout( this );
			// layout->setContentsMargins( 0, 0, 0, 0 );

			_comboBox = new QComboBox( this );
			_refreshComboBox();
			_comboBox->setCurrentIndex( 0 );
			layout->addWidget( _comboBox, 0, 0, 1, 3 );

			using namespace Core::Widget;
			using namespace Action;

			auto * btnNew		 = new ActionablePushButton( Factory::get<Preset::Add<P>>(), this );
			auto * btnDupplicate = new ActionablePushButton( Factory::get<Preset::Duplicate<P>>(), this );
			auto * btnDelete	 = new ActionablePushButton( Factory::get<Preset::Delete<P>>(), this );

			layout->addWidget( btnNew, 1, 0 );
			layout->addWidget( btnDupplicate, 1, 1 );
			layout->addWidget( btnDelete, 1, 2 );

			auto * lineRename = new QLineEdit( this );
			layout->addWidget( lineRename, 2, 0, 1, 3 );
			lineRename->setText( _comboBox->currentText() );

			connect(
				_comboBox,
				&QComboBox::currentTextChanged,
				this,
				[ this, lineRename ]()
				{
					lineRename->setText( _comboBox->currentText() );
					emit presetChanged( _comboBox->currentText() );
				}
			);

			connect(
				btnDupplicate,
				&QPushButton::clicked,
				[ this ]()
				{
					App::ACTION_SYSTEM().execute<App::Action::Library::DuplicatePreset<P>>(
						_comboBox->currentText().toStdString()
					);
				}
			);
			connect(
				btnDelete,
				&QPushButton::clicked,
				[ this ]()
				{
					App::ACTION_SYSTEM().execute<App::Action::Library::DeletePreset<P>>(
						_comboBox->currentText().toStdString()
					);
				}
			);
			connect(
				lineRename,
				&QLineEdit::editingFinished,
				[ this, lineRename ]()
				{
					App::ACTION_SYSTEM().execute<App::Action::Library::RenamePreset<P>>(
						_comboBox->currentText().toStdString(), lineRename->text().toStdString()
					);
				}
			);

			// Callbacks.
			_library->onPresetAdded += [ this ]( std::string_view p_name )
			{
				_refreshComboBox();
				_comboBox->setCurrentText( QString::fromStdString( std::string( p_name ) ) );
			};
			_library->onPresetRenamed += [ this ]( std::string_view p_name )
			{
				_refreshComboBox();
				_comboBox->setCurrentText( QString::fromStdString( std::string( p_name ) ) );
			};
			_library->onPresetDeleted += [ this ]( std::string_view p_name )
			{
				_refreshComboBox();
				assert( _comboBox->count() > 0 );
				// TOFIX: signal blocked (???)
				_comboBox->setCurrentText( _comboBox->itemText( 0 ) );
			};
		}

		inline std::string getCurrentPreset() const { return _comboBox->currentText().toStdString(); }

	  private:
		App::Core::Library::Library<P> * const _library = App::LIBRARY_SYSTEM().getLibrary<P>();
		QPointer<QComboBox>					   _comboBox;

		void _refreshComboBox()
		{
			_comboBox->blockSignals( true );

			_comboBox->clear();
			for ( const auto & [ name, _ ] : _library->getPresets() )
			{
				_comboBox->addItem( QString::fromStdString( name ) );
			}

			_comboBox->blockSignals( false );
		}
	};
} // namespace VTX::UI::QT::Widget::Library
#endif
