#ifndef __VTX_UI_WIDGET_CHAIN_SEQUENCE_WIDGET__
#define __VTX_UI_WIDGET_CHAIN_SEQUENCE_WIDGET__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"
#include "sequence_display_widget.hpp"
#include "ui/widget/view_item_widget.hpp"
#include <QHBoxLayout>
#include <QLabel>
#include <QPoint>
#include <QString>
#include <vector>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Sequence
			{
				class ChainSequenceWidget : public ViewItemWidget<Model::Chain>
				{
					VTX_MANUAL_WIDGET_DECLARATION

				  public:
					void refresh() override;
					void localize() override;

					Model::Residue & getResidueAtPos( const QPoint & p_pos ) const;
					Model::Residue & getFirstResidue() const { return _model->getMoleculePtr()->getResidue( _model->getIndexFirstResidue() ); };
					Model::Residue & getLastResidue() const { return _model->getMoleculePtr()->getResidue( _model->getIndexLastResidue() ); };
					QPoint			 getResiduePos( const Model::Residue & p_residue, const QWidget * const p_widgetSpace ) const
					{
						return _sequenceDisplayWidget->getResiduePos( p_residue, p_widgetSpace );
					}

					void updateSelection( const std::vector<Model::Residue *> & p_selection ) { _sequenceDisplayWidget->updateSelection( p_selection ); };

				  protected:
					ChainSequenceWidget( QWidget * p_parent );
					void _setupUi( const QString & p_name ) override;
					void _setupSlots() override;

				  private:
					QLabel *				_scaleWidget		   = nullptr;
					SequenceDisplayWidget * _sequenceDisplayWidget = nullptr;
					QVBoxLayout *			_layout				   = nullptr;

					void _refreshScale();
					uint _findSecondIndex( const int firstResidueIndex, const int firstIndexStrSize );
				};
			} // namespace Sequence
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
#endif
