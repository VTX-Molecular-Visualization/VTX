#ifndef __VTX_UI_WIDGET_INSPECTOR_MULTIPLE_ATOM__
#define __VTX_UI_WIDGET_INSPECTOR_MULTIPLE_ATOM__

#include "math/transform.hpp"
#include "model/atom.hpp"
#include "ui//widget/custom_widget/qt_multi_data_field.hpp"
#include "ui/multi_data_field.hpp"
#include "ui/widget/custom_widget/collapsing_header_widget.hpp"
#include "ui/widget/custom_widget/transform_widget.hpp"
#include "ui/widget/inspector/inspector_section.hpp"
#include "ui/widget/inspector/multiple_model_inspector_widget.hpp"
#include "view/base_view.hpp"
#include "view/callback_view.hpp"
#include <QLabel>
#include <QWidget>
#include <unordered_set>

namespace VTX::UI::Widget::Inspector
{
	class MultipleAtomWidget : public MultipleModelInspectorWidget<Model::Atom>
	{
		VTX_WIDGET

	  private:
		class MoleculeView
		{
		  public:
			MoleculeView( const Model::Molecule * _molecule );
			~MoleculeView();

			void addViewer() { _counter++; }
			void removeViewer() { _counter--; };
			bool hasViewer() { return _counter > 0; };

			View::CallbackView<const Model::Molecule, MultipleAtomWidget> & getView() { return *_view; }

		  private:
			View::CallbackView<const Model::Molecule, MultipleAtomWidget> * _view	 = nullptr;
			uint															_counter = 1;
		};
		class MoleculeViewContainer
		{
		  public:
			MoleculeViewContainer( MultipleAtomWidget * const p_linkedInspector ) :
				_linkedInspector( p_linkedInspector )
			{
			}
			void addViewOnMolecule( const Model::Molecule * p_molecule );
			void removeViewOnMolecule( const Model::Molecule * p_molecule );
			View::CallbackView<const Model::Molecule, MultipleAtomWidget> & getView(
				const Model::Molecule * p_molecule )
			{
				return _mapMolecules[ p_molecule ]->getView();
			}
			void clear();

		  private:
			std::map<const Model::Molecule *, MoleculeView *> _mapMolecules
				= std::map<const Model::Molecule *, MoleculeView *>();
			MultipleAtomWidget * const _linkedInspector;
		};

	  public:
		~MultipleAtomWidget();

		void localize() override;

		virtual void clearTargets() override;
		virtual void addTarget( Model::Atom * const p_target ) override;
		virtual void removeTarget( Model::Atom * const p_target ) override;

	  protected:
		MultipleAtomWidget( QWidget * p_parent = nullptr );

		void		 _setupUi( const QString & p_name ) override;
		virtual void _setupSlots() override;

		void _endOfFrameRefresh( const SectionFlag & p_flag = SectionFlag ::ALL ) override;

	  private:
		InspectorSection *				 _transformSection = nullptr;
		CustomWidget::TransformWidget *	 _transformWidget  = nullptr;
		InspectorSectionVLayout *		 _infoSection	   = nullptr;
		CustomWidget::QLabelMultiField * _nameLabel		   = nullptr;
		CustomWidget::QLabelMultiField * _genericNameLabel = nullptr;
		QLabel *						 _bondInfoLabel	   = nullptr;

		Math::Transform _transformCache = Math::Transform();

		MoleculeViewContainer _moleculeViewerContainer = MoleculeViewContainer( this );

		void _eventCalledOnMolecule( const Event::VTXEvent * const p_event );

		void _resetFieldStates( const SectionFlag & p_flag );
		void _fillBondData();
	};
} // namespace VTX::UI::Widget::Inspector

#endif
