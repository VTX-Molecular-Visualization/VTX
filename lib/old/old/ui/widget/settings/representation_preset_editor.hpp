#ifndef __VTX_UI_WIDGET_REPRESENTATION_PRESET_EDITOR__
#define __VTX_UI_WIDGET_REPRESENTATION_PRESET_EDITOR__

#include "color/rgb.hpp"
#include "model/representation/representation.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "view/ui/editor_view.hpp"
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QScrollArea>
#include <map>
#include <type_traits>
#include <vector>

namespace VTX::UI::Widget::Settings
{
	class RepresentationPresetEditor :
		public VTX::UI::Widget::BaseManualWidget<QScrollArea>,
		View::UI::EditorView<Model::Representation::Representation>
	{
		VTX_WIDGET

	  private:
		using Representation = Model::Representation::Representation;

		enum class PARAMETER
		{
			TYPE,
			NAME,
			SPHERE_RADIUS,
			CYLINDER_RADIUS,
			CYLINDER_COLOR_BLENDING_MODE,
			RIBBON_COLOR_MODE,
			RIBBON_COLOR_BLENDING_MODE,
			COLOR_MODE,
			COLOR,
			QUICK_ACCESS,
			SET_DEFAULT,
		};

		struct ParameterLine
		{
			ParameterLine( QLabel * const p_label, QWidget * const p_widget ) : _label( p_label ), _widget( p_widget )
			{
			}

			template<typename W, typename = std::enable_if<std::is_base_of<QWidget, W>::value>>
			W * const getWidget()
			{
				return dynamic_cast<W * const>( _widget );
			}

			void setVisible( const bool p_visible )
			{
				if ( _label != nullptr )
					_label->setVisible( p_visible );
				_widget->setVisible( p_visible );
			}

			QLabel * const	_label;
			QWidget * const _widget;
		};

	  public:
		void localize() override;
		void refresh();

		const Model::Representation::Representation * const getPreset() const { return _preset; };
		void setPreset( Model::Representation::Representation * const p_model, const bool p_updateRender = true );

	  protected:
		RepresentationPresetEditor( QWidget * const p_parent );
		~RepresentationPresetEditor();

		void _setupUi( const QString & ) override;
		void _setupSlots() override;
		void _catchModelEvent( const Event::VTXEvent * const p_event ) override;

	  private:
		int				 _itemCount = 0;
		Representation * _preset	= nullptr;

		QWidget *	  _viewport = nullptr;
		QGridLayout * _layout	= nullptr;

		std::map<PARAMETER, ParameterLine *> _parameterWidgets;

		void _addParameter( const PARAMETER & p_parameter, QWidget * const p_widget );
		void _addParameter( const PARAMETER & p_parameter, QWidget * const p_widget, const QString & p_label );
		void _addSpace( const int p_spaceSize = 10 );
		void _populateRepresentationTypeComboBox( QComboBox * const p_comboBox ) const;

		void _setParameterVisibility( const PARAMETER & p_parameter, const bool p_visible );

		template<typename W, typename = std::enable_if<std::is_base_of<QWidget, W>::value>>
		W * _getParameter( const PARAMETER & p_parameter )
		{
			assert( _parameterWidgets.find( p_parameter ) != _parameterWidgets.end() );
			_parameterWidgets[ p_parameter ]->setVisible( true );
			return _parameterWidgets[ p_parameter ]->getWidget<W>();
		}

		void _refreshRepresentationData();
		void _refreshBallAndStickRepresentation();
		void _refreshBallStickAndCartoonRepresentation();
		void _refreshStickRepresentation();
		void _refreshStickAndCartoonRepresentation();
		void _refreshVanDerWaalsRepresentation();
		void _refreshTraceRepresentation();
		void _refreshSASRepresentation();
		void _refreshCartoonRepresentation();
		void _refreshSESRepresentation();

		void _onRepresentationNameChange();
		void _onQuickAccessChange( const int p_state );
		void _onRepresentationTypeChange( const int p_newIndex );
		void _onSphereRadiusChanged( const float p_value );
		void _onCylinderRadiusChanged( const float p_value );
		void _onCylinderColorBendingModeChanged( const int p_index );
		void _onRibbonColorModeChanged( const int p_index );
		void _onRibbonColorBlendingModeChanged( const int p_index );
		void _onColorModeChanged( const int p_index );
		void _onColorChanged( const Color::Rgb & p_color );
		void _onSetDefault();
	};

} // namespace VTX::UI::Widget::Settings

#endif
