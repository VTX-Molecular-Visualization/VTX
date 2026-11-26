#ifndef VTX_UI_QT_WIDGET_LIBRARY_BASE_PRESET_WIDGET_HPP
#define VTX_UI_QT_WIDGET_LIBRARY_BASE_PRESET_WIDGET_HPP

#include "preset_selector.hpp"
#include "read_only_notice.hpp"
#include "ui/qt/helper.hpp"
#include "ui/qt/selection_manager.hpp"
#include "ui/qt/services.hpp"
#include <QGroupBox>
#include <QVBoxLayout>
#include <app/ecs.hpp>

namespace VTX::UI::QT::Widget::Library
{

	template<typename P>
	class BasePresetWidget : public QWidget
	{
	  public:
		BasePresetWidget( QWidget * const p_parent ) :
			QWidget( p_parent ), _layout( Helper::makeQObject<QVBoxLayout>( this ) ),
			_presetSelector( Helper::makeQObject<PresetSelector<P>>( this ) ),
			_readOnlyNotice( Helper::makeQObject<ReadOnlyNotice>( this ) ),
			_groupboxPreset( Helper::makeQObject<QGroupBox>( this ) )
		{
			_groupboxPreset->setLayout( Helper::makeQObject<QVBoxLayout>( _groupboxPreset ) );

			setLayout( _layout );
			_layout->setContentsMargins( 0, 0, 0, 0 );

			_layout->addWidget( _presetSelector );
			_layout->addWidget( _readOnlyNotice );
			_layout->addWidget( _groupboxPreset );

			// Connect preset selector to update function.
			connect( _presetSelector, &PresetSelector<P>::currentPresetUpdated, this, &BasePresetWidget::_update );
			connect( _presetSelector, &PresetSelector<P>::presetChanged, this, &BasePresetWidget::_onPresetChanged );
			connect(
				_presetSelector,
				&PresetSelector<P>::presetSelected,
				this,
				[]( const Entity p_preset ) { SELECTION().selectPreset( SELECTION_GROUP<P>, p_preset ); }
			);
			connect(
				_readOnlyNotice->button(),
				&QPushButton::clicked,
				this,
				[ this ]() { _presetSelector->duplicateCurrentPreset(); }
			);
		}

		Entity currentPreset() const { return _presetSelector->getCurrentPreset(); }

		QGroupBox * presetGroupBox() const { return _groupboxPreset; }

		void addWidget( QWidget * const p_widget ) { _groupboxPreset->layout()->addWidget( p_widget ); }

		void addWidgetBelow( QWidget * const p_widget ) { _layout->addWidget( p_widget ); }

		void setTitle( const QString & p_title ) { _groupboxPreset->setTitle( p_title ); }

		void refresh() { _presetSelector->refresh(); }

		void setCurrentPreset( const Entity p_preset ) { _presetSelector->setCurrentPreset( p_preset ); }

		void setScope( const E_PRESET_SCOPE p_scope ) { _presetSelector->setScope( p_scope ); }

	  protected:
		/**
		 * @brief Update the widget when the preset is updated.
		 */
		virtual void _update( Entity ) = 0;

	  private:
		void _onPresetChanged( const Entity p_preset )
		{
			const bool valid	= App::REG().valid( p_preset ) && App::REG().all_of<P>( p_preset );
			const bool readOnly = ReadOnlyNotice::isReadOnly( p_preset );
			_readOnlyNotice->setVisible( readOnly );
			_groupboxPreset->setVisible( valid && not readOnly );
			if ( valid )
			{
				_update( p_preset );
			}
		}

		QPointer<QVBoxLayout>						 _layout;
		QPointer<Widget::Library::PresetSelector<P>> _presetSelector;
		QPointer<ReadOnlyNotice>					 _readOnlyNotice;
		QPointer<QGroupBox>							 _groupboxPreset;
	};

} // namespace VTX::UI::QT::Widget::Library
#endif
