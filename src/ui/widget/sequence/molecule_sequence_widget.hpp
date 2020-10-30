#ifndef __VTX_UI_WIDGET_MOLECULE_SEQUENCE_WIDGET__
#define __VTX_UI_WIDGET_MOLECULE_SEQUENCE_WIDGET__

#ifdef _MSC_VER
#pragma once
#endif

#include "chain_sequence_widget.hpp"
#include "model/molecule.hpp"
#include "ui/widget/view_item_widget.hpp"
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Sequence
			{
				class MoleculeSequenceWidget : public ViewItemWidget<Model::Molecule>
				{
					VTX_MANUAL_WIDGET_DECLARATION

				  public:
					void receiveEvent( const Event::VTXEvent & p_event ) override;
					void refresh() override;
					void localize() override;

				  protected:
					MoleculeSequenceWidget( QWidget * p_parent );
					void _setupUi( const QString & p_name ) override;
					void _setupSlots() override;

					void _onScrollBarValueChanged();
					void _updateLabelName( const Model::Chain & p_currentChainDisplayed );

				  private:
					QLabel *						   _sequenceLabel	  = nullptr;
					QWidget *						   _scrollAreaContent = nullptr;
					QScrollArea *					   _scrollArea		  = nullptr;
					std::vector<ChainSequenceWidget *> _chainDisplayWidgets;
					QHBoxLayout *					   _sequenceLayout = nullptr;
				};
			} // namespace Sequence
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
#endif
