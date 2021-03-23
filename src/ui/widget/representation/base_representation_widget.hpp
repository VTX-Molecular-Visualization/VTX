#ifndef __VTX_UI_WIDGET_BASE_REPRESENTATION_WIDGET__
#define __VTX_UI_WIDGET_BASE_REPRESENTATION_WIDGET__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/representation/instantiated_representation.hpp"
#include "ui/multi_data_field.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/custom_widget/colormode_field_widget.hpp"
#include "ui/widget/custom_widget/float_field_slider_widget.hpp"
#include "ui/widget/custom_widget/secondary_structure_colormode_field_widget.hpp"
#include <QWidget>
#include <unordered_set>

namespace VTX::View::UI::Widget::Representation
{
	class InstantiatedRepresentationView;
}

namespace VTX::UI::Widget::Representation
{
	class BaseRepresentationWidget :
		public VTX::UI::Widget::BaseManualWidget<QWidget>,
		public TMultiDataField<Model::Representation::InstantiatedRepresentation>
	{
		Q_OBJECT
		VTX_WIDGET

	  public:
		using InstantiatedRepresentation	 = Model::Representation::InstantiatedRepresentation;
		using InstantiatedRepresentationView = View::UI::Widget::Representation::InstantiatedRepresentationView;

		~BaseRepresentationWidget();
		void refresh();

		void setRepresentation( InstantiatedRepresentation * const p_representation );
		void notifyInstantiatedRepresentationViewDeleted();

		// MultiDataField Implementation //////////////////////////////
		// TODO : Remove => BaseRepresentationWidget is virtual and children override this function
		virtual void updateWithNewValue( const Model::Representation::InstantiatedRepresentation & p_value ) override;
		void		 resetState() override;
		//////////////////////////////////////////////////////////////

	  signals:
		void onDataChange( const Model::Representation::MEMBER_FLAG & p_flag );
		void onColorChange( const Color::Rgb & p_color, const bool p_ssColor );

	  protected:
		BaseRepresentationWidget( QWidget * p_parent = nullptr );

		InstantiatedRepresentation *	   _instantiatedRepresentation = nullptr;
		Model::Representation::MEMBER_FLAG _sphereFlag = Model::Representation::MEMBER_FLAG::SPHERE_RADIUS_FIXED;

		void		 _setupUi( const QString & p_name ) override;
		void		 _setupSlots();
		virtual void _refresh() {};

		// MultiDataField Implementation //////////////////////////////
		void _displayDifferentsDataFeedback() override;
		//////////////////////////////////////////////////////////////

		void _addSphereWidgetInLayout( const QString &							p_label,
									   const float								p_min,
									   const float								p_max,
									   const Model::Representation::MEMBER_FLAG p_sphereFlag );
		void _addCylinderWidgetInLayout( const QString & p_label, const float p_min, const float p_max );
		void _addColorModeInLayout( const QString & p_label );
		void _addSSColorModeInLayout( const QString & p_label );

		void _setSphereValue( const float p_value );
		void _addSphereValue( const float p_value );
		void _setCylinderValue( const float p_value );
		void _addCylinderValue( const float p_value );

		void _refreshColorModeWidget();
		void _refreshSSColorModeWidget();
		void _refreshColorModeWidget( const InstantiatedRepresentation & p_representation );
		void _refreshSSColorModeWidget( const InstantiatedRepresentation & p_representation );
		void _addColorModeValue( const InstantiatedRepresentation & p_representation );
		void _addSSColorModeValue( const InstantiatedRepresentation & p_representation );

		virtual void _onSphereRadiusChange( const float p_newRadius );
		virtual void _onCylinderRadiusChange( const float p_newRadius );
		virtual void _colorModeChanged( const Generic::COLOR_MODE & p_colorMode );
		virtual void _ssColorModeChanged( const Generic::SECONDARY_STRUCTURE_COLOR_MODE & p_colorMode );
		void		 _colorChanged( const Color::Rgb & p_color );
		void		 _ssColorChanged( const Color::Rgb & p_color );

		CustomWidget::FloatFieldSliderWidget *				   _sphereWidget	  = nullptr;
		CustomWidget::FloatFieldSliderWidget *				   _cylinderWidget	  = nullptr;
		CustomWidget::ColorModeFieldWidget *				   _colorModeWidget	  = nullptr;
		CustomWidget::SecondaryStructureColorModeFieldWidget * _ssColorModeWidget = nullptr;

	  private:
		View::UI::Widget::Representation::InstantiatedRepresentationView * _representationView = nullptr;

		QGridLayout *										   _layout = nullptr;
		std::unordered_set<const Generic::BaseRepresentable *> _targets
			= std::unordered_set<const Generic::BaseRepresentable *>();
	};
} // namespace VTX::UI::Widget::Representation
#endif
