#ifndef __VTX_UI_QT_WIDGET_PRESET_SELECTOR__
#define __VTX_UI_QT_WIDGET_PRESET_SELECTOR__

#include "ui/qt/actions.hpp"
#include "ui/qt/application.hpp"
#include "ui/qt/widget/actionable_push_button.hpp"
#include <QApplication>
#include <QComboBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QPointer>
#include <QPushButton>
#include <QToolBar>
#include <QVBoxLayout>
#include <app/action/preset.hpp>
#include <app/ecs.hpp>
#include <app/preset/name.hpp>
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
		void presetChanged( const App::ECS::Entity );
	};

	/**
	 * @brief Class responsible for displaying a combo box to select a preset, add and remove.
	 */
	template<typename P>
	class PresetSelector : public BasePresetSelector
	{
	  public:
		PresetSelector( QWidget * p_parent ) : BasePresetSelector( p_parent )
		{
			auto * layout = new QVBoxLayout( this );
			setTitle( "Presets" );

			// layout->setContentsMargins( 0, 0, 0, 0 );

			_comboBox = new QComboBox( this );
			layout->addWidget( _comboBox );

			using namespace Action;

			auto * toolbar = new QToolBar( this );
			toolbar->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
			toolbar->setIconSize( QSize( 18, 18 ) );
			auto * aNew		  = Application::getAction<Preset::Add<P>>();
			auto * aDuplicate = Application::getAction<Preset::Duplicate<P>>();
			auto * aDelete	  = Application::getAction<Preset::Delete<P>>();
			toolbar->addAction( aNew );
			toolbar->addAction( aDuplicate );
			toolbar->addAction( aDelete );
			layout->addWidget( toolbar );

			auto * lineRename = new QLineEdit( this );
			lineRename->setText( _comboBox->currentText() );
			layout->addWidget( lineRename );

			connect(
				_comboBox,
				&QComboBox::currentTextChanged,
				this,
				[ this, lineRename ]()
				{
					lineRename->setText( _comboBox->currentText() );
					emit presetChanged( getCurrentPreset() );
				}
			);

			connect( aNew, &QAction::triggered, [ this ]() { App::ACTION().execute<App::Action::Preset::Add<P>>(); } );

			connect(
				aDuplicate,
				&QAction::triggered,
				[ this ]() { App::ACTION().execute<App::Action::Preset::Duplicate<P>>( getCurrentPreset() ); }
			);

			connect(
				aDelete,
				&QAction::triggered,
				[ this ]() { App::ACTION().execute<App::Action::Preset::Delete<P>>( getCurrentPreset() ); }
			);

			connect(
				lineRename,
				&QLineEdit::editingFinished,
				[ this, lineRename ]()
				{
					App::ACTION().execute<App::Action::Preset::Rename<P>>(
						getCurrentPreset(), lineRename->text().toStdString()
					);
				}
			);

			// Callbacks.
			auto & reg = App::REG();

			reg.on_construct<P>().connect<&PresetSelector::_refreshComboBox>( this );
			reg.on_update<P>().connect<&PresetSelector::_refreshComboBox>( this );
			reg.on_destroy<P>().connect<&PresetSelector::_refreshComboBox>( this );

			/*
			_library.onPresetAdded += [ this ]( const std::string_view p_name )
			{
				_refreshComboBox();
				_comboBox->setCurrentText( QString::fromStdString( std::string( p_name ) ) );
			};
			_library.onPresetRenamed += [ this ]( const std::string_view p_name )
			{
				_refreshComboBox();
				_comboBox->setCurrentText( QString::fromStdString( std::string( p_name ) ) );
			};
			_library.onPresetDeleted += [ this ]( const std::string_view p_name )
			{
				_refreshComboBox();
				assert( _comboBox->count() > 0 );
				_comboBox->setCurrentIndex( 0 );
			};
			**/

			// emit presetChanged( _comboBox->currentData().value<App::ECS::Entity>() );
		}

		inline App::ECS::Entity getCurrentPreset() const
		{
			return _comboBox->currentData().value<App::ECS::Entity>();
			;
		}

	  private:
		QPointer<QComboBox> _comboBox;

		void _refreshComboBox( App::ECS::Registry & p_r, App::ECS::Entity p_e )
		{
			using namespace App;
			const QSignalBlocker blocker( _comboBox );

			int index = _comboBox->currentIndex();

			_comboBox->clear();

			auto view = REG().view<Preset::Name, P>();
			for ( const ECS::Entity entity : view )
			{
				const auto & presetName = view.get<Preset::Name>( entity ).name;
				_comboBox->addItem( QString::fromStdString( presetName ), QVariant::fromValue<ECS::Entity>( entity ) );
			}

			int newIndex = std::min( index, _comboBox->count() - 1 );
			_comboBox->setCurrentIndex( newIndex );
		}
	};
} // namespace VTX::UI::QT::Widget::Library
#endif
