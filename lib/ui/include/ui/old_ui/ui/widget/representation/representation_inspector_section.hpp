#ifndef __VTX_VIEW_UI_WIDGET_INSTANTIATED_REPRESENTATION_INSPECTOR_VIEW__
#define __VTX_VIEW_UI_WIDGET_INSTANTIATED_REPRESENTATION_INSPECTOR_VIEW__

#include "ui/old_ui/ui/multi_data_field.hpp"
#include "ui/old_ui/ui/widget/base_manual_widget.hpp"
#include "ui/old_ui/ui/widget/representation/ball_and_stick_representation_widget.hpp"
#include "ui/old_ui/ui/widget/representation/ball_stick_and_cartoon_representation_widget.hpp"
#include "ui/old_ui/ui/widget/representation/base_representation_widget.hpp"
#include "ui/old_ui/ui/widget/representation/cartoon_representation_widget.hpp"
#include "ui/old_ui/ui/widget/representation/representation_library_combo_box.hpp"
#include "ui/old_ui/ui/widget/representation/sas_representation_widget.hpp"
#include "ui/old_ui/ui/widget/representation/ses_representation_widget.hpp"
#include "ui/old_ui/ui/widget/representation/stick_and_cartoon_representation_widget.hpp"
#include "ui/old_ui/ui/widget/representation/stick_representation_widget.hpp"
#include "ui/old_ui/ui/widget/representation/trace_representation_widget.hpp"
#include "ui/old_ui/ui/widget/representation/vdw_representation_widget.hpp"
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <app/old_app/color/rgba.hpp>
#include <app/core/event/vtx_event.hpp>
#include <app/old_app/generic/base_colorable.hpp>
#include <app/old_app/generic/base_representable.hpp>
#include <app/model/representation/instantiated_representation.hpp>
#include <app/core/view/base_view.hpp>
#include <vector>

namespace VTX::UI::Widget::Representation
{
	class RepresentationInspectorSection :
		public BaseManualWidget<QWidget>,
		public TMultiDataField<const Model::Representation::InstantiatedRepresentation>
	{
		Q_OBJECT
		VTX_WIDGET

	  public:
		enum ActionButtons
		{
			ApplyToChildren = 1 << 0,
			Revert			= 1 << 1,

			All	 = 0xFFFF,
			None = 0,
		};

		using InstantiatedRepresentation = Model::Representation::InstantiatedRepresentation;

	  public:
		void localize() override;
		void receiveEvent( const VTX::App::Core::Event::VTXEvent & p_event ) override;
		void refresh();

		void resetState() override;
		void resetState( const bool p_deleteViews, const bool p_deleteDataWidget );

		void updateWithNewValue( const InstantiatedRepresentation & p_representation ) override;
		void updateWithNewValue( const InstantiatedRepresentation & p_representation, const bool p_instantiateViews );

		void setActionButtonVisibility( const ActionButtons & p_buttons );
		void setDirty();

	  signals:
		void onRepresentationPresetChange( const int p_presetIndex );
		void onRepresentationChange( const InstantiatedRepresentation &			p_representation,
									 const Model::Representation::MEMBER_FLAG & p_flag );
		void onRepresentationColorChange( const InstantiatedRepresentation & p_representation,
										  const Color::Rgba &				 p_color,
										  const bool						 p_ssColor );
		void onRevertRepresentation();
		void onApplyRepresentationToChildren();

	  protected:
		RepresentationInspectorSection( QWidget * const p_parent = nullptr );
		~RepresentationInspectorSection();

		void _setupUi( const QString & ) override;
		void _setupSlots() override;

		void _displayDifferentsDataFeedback() override;

	  private:
		CustomWidget::QPushButtonMultiField * _titleWidget				   = nullptr;
		QWidget *							  _representationWidget		   = nullptr;
		RepresentationLibraryComboBox *		  _representationPreset		   = nullptr;
		QVBoxLayout *						  _settingLayout			   = nullptr;
		BaseRepresentationWidget *			  _representationSettingWidget = nullptr;
		QPushButton *						  _applyToChildrenButton	   = nullptr;
		QPushButton *						  _revertButton				   = nullptr;

		std::unordered_set<App::Core::Model::ID> _representationIDs = std::unordered_set<App::Core::Model::ID>();

		Model::Representation::InstantiatedRepresentation * _dummyRepresentation	 = nullptr;
		int													_baseRepresentationIndex = -1;
		bool												_isDirty				 = false;

		void _instantiateRepresentationSettingWidget( const Generic::REPRESENTATION & p_representation );
		void _deleteRepresentationSettingWidget();

		void _toggleSettingDisplay() const;
		void _setSettingDisplay( const bool p_expand ) const;
		void _representationPresetChange( const int p_resetIndex );
		void _representationDataChange( const Model::Representation::MEMBER_FLAG & p_flagDataModified );
		void _representationColorChange( const Color::Rgba & p_color, const bool p_ssColor );
		void _revertRepresentation();
		void _applyRepresentationToChildren();

		void _onTargetedRepresentationChange( const VTX::App::Core::Event::VTXEvent * const p_event );
		void _recomputeUi();

		void _onDummyChange( const VTX::App::Core::Event::VTXEvent * const p_event );
	};

} // namespace VTX::UI::Widget::Representation

#endif
