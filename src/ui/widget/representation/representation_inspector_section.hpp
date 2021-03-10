#ifndef __VTX_VIEW_UI_WIDGET_INSTANTIATED_REPRESENTATION_INSPECTOR_VIEW__
#define __VTX_VIEW_UI_WIDGET_INSTANTIATED_REPRESENTATION_INSPECTOR_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "color/rgb.hpp"
#include "generic/base_colorable.hpp"
#include "generic/base_representable.hpp"
#include "model/representation/instantiated_representation.hpp"
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
	class RepresentationInspectorSection : public VTX::UI::Widget::BaseManualWidget<QWidget>
	{
		VTX_WIDGET
		VTX_VIEW

		using InstantiatedRepresentation = Model::Representation::InstantiatedRepresentation;
		using BaseRepresentationWidget	 = VTX::UI::Widget::Representation::BaseRepresentationWidget;

		using BallAndStickRepresentationWidget = VTX::UI::Widget::Representation::BallAndStickRepresentationWidget;
		using BallStickAndCartoonRepresentationWidget
			= VTX::UI::Widget::Representation::BallStickAndCartoonRepresentationWidget;
		using CartoonRepresentationWidget = VTX::UI::Widget::Representation::CartoonRepresentationWidget;
		using SasRepresentationWidget	  = VTX::UI::Widget::Representation::SasRepresentationWidget;
		using StickRepresentationWidget	  = VTX::UI::Widget::Representation::StickRepresentationWidget;
		using StickAndCartoonRepresentationWidget
			= VTX::UI::Widget::Representation::StickAndCartoonRepresentationWidget;
		using TraceRepresentationWidget = VTX::UI::Widget::Representation::TraceRepresentationWidget;
		using VdwRepresentationWidget	= VTX::UI::Widget::Representation::VdwRepresentationWidget;

	  public:
		void localize() override;

		template<typename T,
				 typename = std::enable_if<std::is_base_of<Model::BaseModel, T>::value>,
				 typename = std::enable_if<std::is_base_of<Generic::BaseRepresentable, T>::value>>
		void setTarget( T * const p_target )
		{
			_setTarget( p_target, p_target );
		}

	  protected:
		RepresentationInspectorSection( QWidget * const p_parent );
		~RepresentationInspectorSection();

		void _setupUi( const QString & ) override;
		void _setupSlots() override;

		void _setTarget( Model::BaseModel * const p_model, Generic::BaseRepresentable * const p_representable );

		void _refresh();

	  private:
		Generic::BaseRepresentable *						_target		 = nullptr;
		Model::BaseModel *									_targetModel = nullptr;
		int													_representationIndex;
		Model::Representation::InstantiatedRepresentation * _representation = nullptr;

		bool _representationHasBeenModified = false;

		QPushButton *			   _titleWidget					= nullptr;
		QWidget *				   _representationWidget		= nullptr;
		QComboBox *				   _representationPreset		= nullptr;
		QVBoxLayout *			   _settingLayout				= nullptr;
		BaseRepresentationWidget * _representationSettingWidget = nullptr;

		BaseRepresentationWidget * _instantiateRepresentationSettingWidget(
			const Generic::REPRESENTATION & p_representation );

		void _toggleSettingDisplay();
		void _representationPresetChange( const int p_resetIndex );
		void _representationDataChange();

		void _populateRepresentationModeComboBox();

		void _setRepresentationIfNeeded();
	};

} // namespace VTX::UI::Widget::Representation

#endif
