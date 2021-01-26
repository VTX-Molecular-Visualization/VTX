#ifndef __VTX_UI_WIDGET_CHAIN_SEQUENCE_WIDGET__
#define __VTX_UI_WIDGET_CHAIN_SEQUENCE_WIDGET__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"
#include "sequence_chain_data.hpp"
#include "sequence_display_widget.hpp"
#include "ui/widget/view_item_widget.hpp"
#include <QHBoxLayout>
#include <QLabel>
#include <QPoint>
#include <QString>

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
					VTX_WIDGET

				  public:
					~ChainSequenceWidget()
					{
						if ( _chainData != nullptr )
							delete _chainData;
					};

					void refresh() override;
					void localize() override;

					Model::Residue * const getResidueAtPos( const QPoint & p_pos ) const;
					Model::Residue * const getFirstResidue() const
					{
						return _model->getMoleculePtr()->getResidue( _model->getIndexFirstResidue() );
					};
					Model::Residue * const getLastResidue() const
					{
						return _model->getMoleculePtr()->getResidue( _model->getIndexLastResidue() );
					};
					QPoint getResiduePos( const Model::Residue & p_residue, const QWidget * const p_widgetSpace ) const
					{
						return _sequenceDisplayWidget->getResiduePos( p_residue, p_widgetSpace );
					}

					Model::Residue & getClosestResidueFromPos( const QPoint & p_pos, const bool p_takeForward ) const;
					int				 getSequenceXmin() const { return pos().x() + _sequenceDisplayWidget->pos().x(); };
					int				 getSequenceXmax() const
					{
						return pos().x() + _sequenceDisplayWidget->pos().x() + _sequenceDisplayWidget->getSize() - 1;
					};

					void repaintSelection() const { _sequenceDisplayWidget->repaint(); };

				  protected:
					ChainSequenceWidget( QWidget * p_parent );
					void _setupUi( const QString & p_name ) override;
					void _setupSlots() override;

				  private:
					QLabel *				_scaleWidget		   = nullptr;
					SequenceDisplayWidget * _sequenceDisplayWidget = nullptr;
					QVBoxLayout *			_layout				   = nullptr;

					SequenceChainData * _chainData = nullptr;
				};
			} // namespace Sequence
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
#endif
