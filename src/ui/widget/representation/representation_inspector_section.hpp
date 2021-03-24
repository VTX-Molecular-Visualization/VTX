#ifndef __VTX_VIEW_UI_WIDGET_INSTANTIATED_REPRESENTATION_INSPECTOR_VIEW__
#define __VTX_VIEW_UI_WIDGET_INSTANTIATED_REPRESENTATION_INSPECTOR_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "color/rgb.hpp"
#include "generic/base_colorable.hpp"
#include "generic/base_representable.hpp"
#include "model/representation/instantiated_representation.hpp"
#include "ui/multi_data_field.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/representation/ball_and_stick_representation_widget.hpp"
#include "ui/widget/representation/ball_stick_and_cartoon_representation_widget.hpp"
#include "ui/widget/representation/base_representation_widget.hpp"
#include "ui/widget/representation/cartoon_representation_widget.hpp"
#include "ui/widget/representation/sas_representation_widget.hpp"
#include "ui/widget/representation/stick_and_cartoon_representation_widget.hpp"
#include "ui/widget/representation/stick_representation_widget.hpp"
#include "ui/widget/representation/trace_representation_widget.hpp"
#include "ui/widget/representation/vdw_representation_widget.hpp"
#include "view/base_view.hpp"
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <vector>

namespace VTX::UI::Widget::Representation
{
	class RepresentationInspectorSection :
		public BaseManualWidget<QWidget>,
		TMultiDataField<Model::Representation::InstantiatedRepresentation>
	{
		Q_OBJECT
		VTX_WIDGET

		using InstantiatedRepresentation = Model::Representation::InstantiatedRepresentation;

	  public:
		void localize() override;
		void refresh();

		void resetState() override;
		void updateWithNewValue( const InstantiatedRepresentation & p_representation ) override;

	  signals:
		void onRepresentationPresetChange( const int p_presetIndex );
		void onRepresentationChange( const InstantiatedRepresentation &			p_representation,
									 const Model::Representation::MEMBER_FLAG & p_flag );
		void onRepresentationColorChange( const InstantiatedRepresentation & p_representation,
										  const Color::Rgb &				 p_color,
										  const bool						 p_ssColor );
		void onRevertRepresentation();

	  protected:
		RepresentationInspectorSection( QWidget * const p_parent = nullptr );
		~RepresentationInspectorSection();

		void _setupUi( const QString & ) override;
		void _setupSlots() override;

		void _displayDifferentsDataFeedback() override;

	  private:
		CustomWidget::QPushButtonMultiField * _titleWidget				   = nullptr;
		QWidget *							  _representationWidget		   = nullptr;
		CustomWidget::QComboBoxMultiField *	  _representationPreset		   = nullptr;
		QVBoxLayout *						  _settingLayout			   = nullptr;
		BaseRepresentationWidget *			  _representationSettingWidget = nullptr;
		QPushButton *						  _revertButton				   = nullptr;

		Model::Representation::InstantiatedRepresentation * _dummyRepresentation;
		int													_baseRepresentationIndex = -1;

		void _instantiateRepresentationSettingWidget( const Generic::REPRESENTATION & p_representation );
		void _deleteRepresentationSettingWidget();

		void _toggleSettingDisplay() const;
		void _representationPresetChange( const int p_resetIndex );
		void _representationDataChange( const Model::Representation::MEMBER_FLAG & p_flagDataModified );
		void _representationColorChange( const Color::Rgb & p_color, const bool p_ssColor );
		void _revertRepresentation();

		void _populateRepresentationModeComboBox();
	};

} // namespace VTX::UI::Widget::Representation

#endif
