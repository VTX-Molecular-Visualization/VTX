#ifndef __VTX_UI_QT_WIDGET_PRESET_SELECTOR__
#define __VTX_UI_QT_WIDGET_PRESET_SELECTOR__

#include "ui/qt/actions.hpp"
#include "ui/qt/widget/actionable_push_button.hpp"
#include <QApplication>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QPointer>
#include <QPushButton>
#include <app/library/base_preset.hpp>
// #include <app/action/library.hpp>
// #include <app/library/library_manager.hpp>
#include <app/library/base_library.hpp>
#include <app/library/library_manager.hpp>
#include <app/services.hpp>

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
	template<App::Library::ConceptPreset P>
	class PresetSelector : public BasePresetSelector
	{
	  public:
		PresetSelector( QWidget * p_parent ) :
			BasePresetSelector( p_parent ), _library( App::LIBRARY().getLibrary<P>() )
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

			/*
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
					App::ACTION().execute<App::Action::Library::DuplicatePreset<P>>(
						_comboBox->currentText().toStdString()
					);
				}
			);
			connect(
				btnDelete,
				&QPushButton::clicked,
				[ this ]()
				{
					App::ACTION().execute<App::Action::Library::DeletePreset<P>>(
						_comboBox->currentText().toStdString()
					);
				}
			);
			connect(
				lineRename,
				&QLineEdit::editingFinished,
				[ this, lineRename ]()
				{
					App::ACTION().execute<App::Action::Library::RenamePreset<P>>(
						_comboBox->currentText().toStdString(), lineRename->text().toStdString()
					);
				}
			);
			*/

			// Callbacks.
			_library->onPresetAdded += [ this ]( const std::string_view p_name )
			{
				_refreshComboBox();
				_comboBox->setCurrentText( QString::fromStdString( std::string( p_name ) ) );
			};
			_library->onPresetRenamed += [ this ]( const std::string_view p_name )
			{
				_refreshComboBox();
				_comboBox->setCurrentText( QString::fromStdString( std::string( p_name ) ) );
			};
			_library->onPresetDeleted += [ this ]( const std::string_view p_name )
			{
				_refreshComboBox();
				assert( _comboBox->count() > 0 );
				_comboBox->setCurrentIndex( 0 );
			};

			emit presetChanged( _comboBox->currentText() );
		}

		inline QString	   getCurrentPreset() const { return _comboBox->currentText(); }
		inline std::string getCurrentPresetStr() const { return _comboBox->currentText().toStdString(); }

	  private:
		App::Library::BaseLibrary<P> * const _library;
		QPointer<QComboBox>					 _comboBox;

		void _refreshComboBox()
		{
			const QSignalBlocker blocker( _comboBox );

			_comboBox->clear();
			for ( const auto & [ name, _ ] : _library->getPresets() )
			{
				_comboBox->addItem( QString::fromStdString( name ) );
			}
			_comboBox->setCurrentIndex( -1 );
		}
	};
} // namespace VTX::UI::QT::Widget::Library
#endif
