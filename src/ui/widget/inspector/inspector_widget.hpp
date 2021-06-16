#ifndef __VTX_UI_WIDGET_INSPECTOR__
#define __VTX_UI_WIDGET_INSPECTOR__

#include "id.hpp"
#include "model/atom.hpp"
#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "model/representation/representation.hpp"
#include "model/residue.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/custom_widget/dock_window_main_widget.hpp"
#include "ui/widget/inspector/multiple_atom_inspector_widget.hpp"
#include "ui/widget/inspector/multiple_chain_inspector_widget.hpp"
#include "ui/widget/inspector/multiple_molecule_inspector_widget.hpp"
#include "ui/widget/inspector/multiple_residue_inspector_widget.hpp"
#include "view/ui/widget/base_widget_view.hpp"
#include <QDockWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>
#include <vector>

namespace VTX::UI::Widget::Inspector
{
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
		VTX_WIDGET

	  public:
		~InspectorWidget();
		void localize() override;
		void receiveEvent( const Event::VTXEvent & p_event ) override;
		void refresh();
		void clear();

	  protected:
		InspectorWidget( QWidget * p_parent );

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		QWidget *										  _scrollWidget	  = nullptr;
		CustomWidget::DockWindowMainWidget<QScrollArea> * _scrollArea	  = nullptr;
		QVBoxLayout *									  _verticalLayout = nullptr;

		MultipleMoleculeWidget * _moleculesInspector = nullptr;
		MultipleChainWidget *	 _chainsInspector	 = nullptr;
		MultipleResidueWidget *	 _residuesInspector	 = nullptr;
		MultipleAtomWidget *	 _atomsInspector	 = nullptr;

		std::vector<ViewData> _inspectorViewsData = std::vector<ViewData>();
	};
} // namespace VTX::UI::Widget::Inspector

#endif
