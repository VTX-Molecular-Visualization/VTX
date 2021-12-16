#ifndef __VTX_UI_WIDGET_INSPECTOR__
#define __VTX_UI_WIDGET_INSPECTOR__

#include "id.hpp"
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

		MultipleMoleculeWidget *  _moleculesInspector  = nullptr;
		MultipleChainWidget *	  _chainsInspector	   = nullptr;
		MultipleResidueWidget *	  _residuesInspector   = nullptr;
		MultipleAtomWidget *	  _atomsInspector	   = nullptr;
		MultipleViewpointWidget * _viewpointsInspector = nullptr;

		std::vector<ViewData> _inspectorViewsData = std::vector<ViewData>();
		bool				  _isFreezed		  = false;

		void _toggleFreezeState();
		void _updateFreezedFeedback();
	};
} // namespace VTX::UI::Widget::Inspector

#endif
