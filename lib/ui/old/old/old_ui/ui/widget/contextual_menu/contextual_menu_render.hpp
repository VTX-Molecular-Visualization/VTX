#ifndef __VTX_UI_WIDGET_CONTEXTUAL_MENU_RENDER__
#define __VTX_UI_WIDGET_CONTEXTUAL_MENU_RENDER__

#include "contextual_menu_template.hpp"
#include <QAction>
#include <QMenu>
#include <QString>

namespace VTX::UI::Widget::ContextualMenu
{
	class ContextualMenuRender : public BaseContextualMenu
	{
		VTX_WIDGET

	  private:
		inline static const int SHOW_ALL_OVERLAYS_DATA_VALUE = -1;
		inline static const int HIDE_ALL_OVERLAYS_DATA_VALUE = -2;

	  public:
		~ContextualMenuRender();
		void localize() override;
		void receiveEvent( const VTX::App::Old::Core::Event::VTXEvent & p_event ) override;

	  protected:
		ContextualMenuRender( QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _loadMoleculeAction() const;
		void _downloadMoleculeAction() const;

		void _setPickerToSelection() const;

		void _setSelectionGranularityAction( QAction * p_action ) const;
		void _setMeasurementMode( QAction * p_action ) const;
		void _showAllMoleculesAction() const;
		void _resetCameraAction() const;

	  private:
		QAction * _selectionModeAction;
		QAction * _measurementModeAction;

		QMenu * _projectionMenu			  = nullptr;
		QMenu * _backgroundColorMenu	  = nullptr;
		QMenu * _renderSettingPreset	  = nullptr;
		QMenu * _selectionGranularityMenu = nullptr;
		QMenu * _measurementModeMenu	  = nullptr;

		QMenu * _overlaysMenu = nullptr;

		void _addSelectionGranularityActionInMenu( const int p_granularity, const QString & p_name ) const;
		void _addMeasurementModeActionInMenu( const int p_mode, const QString & p_name ) const;

		void _refreshPickerMode() const;
		void _refreshSelectionGranularityMenu() const;
		void _refreshMeasurementModeMenu() const;
		void _refreshCameraProjection() const;
		void _refreshAppliedRenderSettingPreset() const;
		void _refreshOverlayVisibilityMenu() const;

		void _changeProjectionAction( QAction * const p_action );
		void _setBackgroundColorAction( QAction * const p_action );
		void _applyRenderEffectPresetAction( QAction * const p_action );
		void _setOverlayVisibilityAction( QAction * const p_action );
		void _takeSnapshotAction();
		void _exportImageAction();
	};

} // namespace VTX::UI::Widget::ContextualMenu

#endif
