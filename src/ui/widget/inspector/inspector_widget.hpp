#ifndef __VTX_UI_WIDGET_INSPECTOR__
#define __VTX_UI_WIDGET_INSPECTOR__

#include "id.hpp"
#include "inspector_item_widget.hpp"
#include "multiple_model_inspector_widget.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/custom_widget/dock_window_main_widget.hpp"
#include "view/ui/widget/base_widget_view.hpp"
#include <QDockWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>
#include <vector>

namespace VTX::UI::Widget::Inspector
{
	class MultipleMoleculeWidget;
	class MultipleChainWidget;
	class MultipleResidueWidget;
	class MultipleAtomWidget;
	class MultipleViewpointWidget;
	class MultipleMeasurmentDistanceWidget;
	class MultipleMeasurmentAngleWidget;

	struct ViewData
	{
	  public:
		ViewData( const uint p_modelID, const ID::VTX_ID & p_view, QWidget * const p_widget ) :
			_modelID( p_modelID ), _viewID( p_view ), _widget( p_widget ) {};

		const uint		 _modelID;
		const ID::VTX_ID _viewID;
		QWidget * const	 _widget;
	};

	class InspectorWidget : public BaseManualWidget<QDockWidget>
	{
		Q_OBJECT
		VTX_WIDGET

	  private:
		enum class INSPECTOR_TYPE : int
		{
			MOLECULE,
			CHAIN,
			RESIDUE,
			ATOM,
			VIEWPOINT,
			MEASURE_DISTANCE,
			MEASURE_ANGLE,

			COUNT
		};

	  public:
		~InspectorWidget();
		void localize() override;
		void receiveEvent( const Event::VTXEvent & p_event ) override;
		void refresh();
		void clear();

		bool isFreezed() const { return _isFreezed; }
		void setFreezed( const bool p_freezed );

	  protected:
		InspectorWidget( QWidget * p_parent );

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		QPushButton * _freezeButton = nullptr;

		QWidget *	  _scrollWidget	  = nullptr;
		QScrollArea * _scrollArea	  = nullptr;
		QVBoxLayout * _verticalLayout = nullptr;

		std::vector<InspectorItemWidget *> _inspectors;

		std::vector<ViewData> _inspectorViewsData = std::vector<ViewData>();
		bool				  _isFreezed		  = false;

		template<typename I, typename = std::enable_if<std::is_base_of<InspectorItemWidget, I>::value>>
		I & _getInspector( const INSPECTOR_TYPE & p_inspectorType )
		{
			return *( dynamic_cast<I *>( _inspectors[ int( p_inspectorType ) ] ) );
		}
		template<typename I, typename = std::enable_if<std::is_base_of<InspectorItemWidget, I>::value>>
		const I & _getInspector( const INSPECTOR_TYPE & p_inspectorType ) const
		{
			return *( dynamic_cast<I *>( _inspectors[ int( p_inspectorType ) ] ) );
		}

		template<typename I,
				 typename M,
				 typename = std::enable_if<std::is_base_of<MultipleModelInspectorWidget<M>, I>::value>>
		void _addTargetToInspector( const INSPECTOR_TYPE & p_inspectorType, M * const p_target )
		{
			MultipleModelInspectorWidget<M> & inspector
				= _getInspector<MultipleModelInspectorWidget<M>>( p_inspectorType );
			inspector.addTarget( p_target );
			inspector.setVisible( true );
		}

		template<typename I,
				 typename M,
				 typename = std::enable_if<std::is_base_of<MultipleModelInspectorWidget<M>, I>::value>>
		void _removeTargetToInspector( const INSPECTOR_TYPE & p_inspectorType,
									   M * const			  p_target,
									   const bool			  p_refresh = true )
		{
			MultipleModelInspectorWidget<M> & inspector
				= _getInspector<MultipleModelInspectorWidget<M>>( p_inspectorType );

			if ( inspector.hasTarget( p_target ) )
			{
				inspector.removeTarget( p_target );

				if ( inspector.getTargets().size() <= 0 )
					inspector.setVisible( false );

				if ( p_refresh )
					refresh();
			}
		}

		void _toggleFreezeState();
		void _updateFreezedFeedback();
	};
} // namespace VTX::UI::Widget::Inspector

#endif
