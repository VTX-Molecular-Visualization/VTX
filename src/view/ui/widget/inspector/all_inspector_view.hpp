#ifndef __VTX_VIEW_UI_WIDGET_ALL_INSPECTOR_VIEW__
#define __VTX_VIEW_UI_WIDGET_ALL_INSPECTOR_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/atom.hpp"
#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "model/representation/instantiated_representation.hpp"
#include "model/residue.hpp"
#include "ui/widget/inspector/inspector_section_flag.hpp"
#include "ui/widget/inspector/multiple_atom_inspector_widget.hpp"
#include "ui/widget/inspector/multiple_chain_inspector_widget.hpp"
#include "ui/widget/inspector/multiple_molecule_inspector_widget.hpp"
#include "ui/widget/inspector/multiple_residue_inspector_widget.hpp"
#include "ui/widget/representation/representation_inspector_section.hpp"
#include "view/base_view.hpp"
#include <QWidget>

namespace VTX::View::Inspector
{
	template<typename M, typename W, typename = std::enable_if<std::is_base_of<Model::BaseModel, M>::value>>
	class TInspectorView : public BaseView<M>
	{
		VTX_VIEW

	  public:
		void setLinkedInspector( W * const p_inspector ) { _linkedInspector = p_inspector; }

	  protected:
		TInspectorView( M * const p_model ) : BaseView( p_model ) {}
		~TInspectorView() {}

		bool hasLinkedInspector() { return _linkedInspector != nullptr; }
		W &	 getLinkedInspector() { return *_linkedInspector; }

	  private:
		W * _linkedInspector = nullptr;
	};

	class MoleculeInspectorView :
		public TInspectorView<Model::Molecule, VTX::UI::Widget::Inspector::MultipleMoleculeWidget>
	{
		VTX_VIEW

	  public:
		void notify( const Event::VTXEvent * const p_event ) override
		{
			if ( !hasLinkedInspector() )
				return;

			if ( p_event->name == Event::Model::DATA_CHANGE )
			{
				getLinkedInspector().refresh( VTX::UI::Widget::Inspector::SectionFlag::INFOS );
			}
			else if ( p_event->name == Event::Model::TRANSFORM_CHANGE )
			{
				getLinkedInspector().refresh( VTX::UI::Widget::Inspector::SectionFlag::TRANSFORM );
			}
			else if ( p_event->name == Event::Model::REPRESENTATION_CHANGE )
			{
				getLinkedInspector().refresh( VTX::UI::Widget::Inspector::SectionFlag::REPRESENTATION );
			}
		}

	  protected:
		MoleculeInspectorView( Model::Molecule * const p_model ) : TInspectorView( p_model ) {}
		~MoleculeInspectorView()
		{
			if ( hasLinkedInspector() )
				getLinkedInspector().removeTarget( _model );
		}
	};

	class ChainInspectorView : public TInspectorView<Model::Chain, VTX::UI::Widget::Inspector::MultipleChainWidget>
	{
		VTX_VIEW

	  public:
		void notify( const Event::VTXEvent * const p_event ) override
		{
			if ( !hasLinkedInspector() )
				return;

			if ( p_event->name == Event::Model::DATA_CHANGE )
			{
				getLinkedInspector().refresh( VTX::UI::Widget::Inspector::SectionFlag::INFOS );
			}
			else if ( p_event->name == Event::Model::REPRESENTATION_CHANGE )
			{
				getLinkedInspector().refresh( VTX::UI::Widget::Inspector::SectionFlag::REPRESENTATION );
			}
		}

	  protected:
		ChainInspectorView( Model::Chain * const p_model ) : TInspectorView( p_model ) {}
		~ChainInspectorView()
		{
			if ( hasLinkedInspector() )
				getLinkedInspector().removeTarget( _model );
		}
	};

	class ResidueInspectorView :
		public TInspectorView<Model::Residue, VTX::UI::Widget::Inspector::MultipleResidueWidget>
	{
		VTX_VIEW

	  public:
		void notify( const Event::VTXEvent * const p_event ) override
		{
			if ( !hasLinkedInspector() )
				return;

			if ( p_event->name == Event::Model::DATA_CHANGE )
			{
				getLinkedInspector().refresh( VTX::UI::Widget::Inspector::SectionFlag::INFOS );
			}
			else if ( p_event->name == Event::Model::REPRESENTATION_CHANGE )
			{
				getLinkedInspector().refresh( VTX::UI::Widget::Inspector::SectionFlag::REPRESENTATION );
			}
		}

	  protected:
		ResidueInspectorView( Model::Residue * const p_model ) : TInspectorView( p_model ) {}
		~ResidueInspectorView()
		{
			if ( hasLinkedInspector() )
				getLinkedInspector().removeTarget( _model, false );
		}
	};

	class AtomInspectorView : public TInspectorView<Model::Atom, VTX::UI::Widget::Inspector::MultipleAtomWidget>
	{
		VTX_VIEW

	  public:
		void notify( const Event::VTXEvent * const p_event ) override
		{
			if ( !hasLinkedInspector() )
				return;

			if ( p_event->name == Event::Model::DATA_CHANGE )
			{
				getLinkedInspector().refresh( VTX::UI::Widget::Inspector::SectionFlag::ALL );
			}
		}

	  protected:
		AtomInspectorView( Model::Atom * const p_model ) : TInspectorView( p_model ) {}
		~AtomInspectorView()
		{
			if ( hasLinkedInspector() )
				getLinkedInspector().removeTarget( _model );
		}
	};

	class InstantiatedRepresentationView : public BaseView<Model::Representation::InstantiatedRepresentation>
	{
		VTX_VIEW

	  public:
		void setLinkedInspector( VTX::UI::Widget::Representation::RepresentationInspectorSection * const p_inspector )
		{
			_linkedInspector = p_inspector;
		}
		void notify( const Event::VTXEvent * const p_event ) override
		{
			if ( _linkedInspector == nullptr )
				return;

			_linkedInspector->refresh();
		}

	  protected:
		InstantiatedRepresentationView( Model::Representation::InstantiatedRepresentation * const p_model ) :
			BaseView( p_model )
		{
		}
		~InstantiatedRepresentationView() {}

	  private:
		VTX::UI::Widget::Representation::RepresentationInspectorSection * _linkedInspector = nullptr;
	};

} // namespace VTX::View::Inspector

#endif
