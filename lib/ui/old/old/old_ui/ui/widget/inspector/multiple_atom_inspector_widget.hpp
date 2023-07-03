#ifndef __VTX_UI_WIDGET_INSPECTOR_MULTIPLE_ATOM__
#define __VTX_UI_WIDGET_INSPECTOR_MULTIPLE_ATOM__

#include "ui/old_ui/ui//widget/custom_widget/qt_multi_data_field.hpp"
#include "ui/old_ui/ui/multi_data_field.hpp"
#include "ui/old_ui/ui/widget/custom_widget/collapsing_header_widget.hpp"
#include "ui/old_ui/ui/widget/custom_widget/transform_widget.hpp"
#include "ui/old_ui/ui/widget/inspector/inspector_section.hpp"
#include "ui/old_ui/ui/widget/inspector/multiple_model_inspector_widget.hpp"
#include <QLabel>
#include <QWidget>
#include "app/old/internal/math/transform.hpp"
#include <app/old/component/chemistry/atom.hpp>
#include <app/old/core/view/base_view.hpp>
#include <app/old/core/view/callback_view.hpp>
#include <unordered_set>

namespace VTX::UI::Widget::Inspector
{
	class MultipleAtomWidget : public MultipleModelInspectorWidget<App::Old::Component::Chemistry::Atom>
	{
		VTX_WIDGET

	  private:
		class MoleculeView
		{
		  public:
			MoleculeView( const App::Old::Component::Chemistry::Molecule * _molecule );
			~MoleculeView();

			void addViewer() { _counter++; }
			void removeViewer() { _counter--; };
			bool hasViewer() { return _counter > 0; };

			App::Old::Core::View::CallbackView<const App::Old::Component::Chemistry::Molecule, MultipleAtomWidget> & getView() { return *_view; }

		  private:
			App::Old::Core::View::CallbackView<const App::Old::Component::Chemistry::Molecule, MultipleAtomWidget> * _view	 = nullptr;
			uint															_counter = 1;
		};
		class MoleculeViewContainer
		{
		  public:
			MoleculeViewContainer( MultipleAtomWidget * const p_linkedInspector ) :
				_linkedInspector( p_linkedInspector )
			{
			}
			void addViewOnMolecule( const App::Old::Component::Chemistry::Molecule * p_molecule );
			void removeViewOnMolecule( const App::Old::Component::Chemistry::Molecule * p_molecule );
			App::Old::Core::View::CallbackView<const App::Old::Component::Chemistry::Molecule, MultipleAtomWidget> & getView(
				const App::Old::Component::Chemistry::Molecule * p_molecule )
			{
				return _mapMolecules[ p_molecule ]->getView();
			}
			void clear();

		  private:
			std::map<const App::Old::Component::Chemistry::Molecule *, MoleculeView *> _mapMolecules
				= std::map<const App::Old::Component::Chemistry::Molecule *, MoleculeView *>();
			MultipleAtomWidget * const _linkedInspector;
		};

	  public:
		~MultipleAtomWidget();

		void localize() override;

		virtual void clearTargets() override;
		virtual void addTarget( App::Old::Component::Chemistry::Atom * const p_target ) override;
		virtual void removeTarget( App::Old::Component::Chemistry::Atom * const p_target ) override;

	  protected:
		MultipleAtomWidget( QWidget * p_parent = nullptr );

		void		 _setupUi( const QString & p_name ) override;
		virtual void _setupSlots() override;

		void _endOfFrameRefresh( const SectionFlag & p_flag = SectionFlag ::ALL ) override;

		void _setInspectorToMolecule() const;
		void _setInspectorToChain() const;
		void _setInspectorToResidue() const;

	  private:
		InspectorSection *				 _transformSection = nullptr;
		CustomWidget::TransformWidget *	 _transformWidget  = nullptr;
		InspectorSectionVLayout *		 _infoSection	   = nullptr;
		CustomWidget::QLabelMultiField * _nameLabel		   = nullptr;
		CustomWidget::QLabelMultiField * _genericNameLabel = nullptr;
		QLabel *						 _bondInfoLabel	   = nullptr;

	 App::Old::Internal::Math::Transform _transformCache = App::Old::Internal::Math::Transform();

		MoleculeViewContainer _moleculeViewerContainer = MoleculeViewContainer( this );

		void _eventCalledOnMolecule( const VTX::App::Old::Core::Event::VTXEvent * const p_event );

		void _resetFieldStates( const SectionFlag & p_flag );
		void _fillBondData();
	};
} // namespace VTX::UI::Widget::Inspector

#endif
