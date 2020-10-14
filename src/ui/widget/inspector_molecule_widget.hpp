#ifndef __VTX_UI_WIDGET_INSPECTOR_MOLECULE__
#define __VTX_UI_WIDGET_INSPECTOR_MOLECULE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_manual_widget.hpp"
#include "model/molecule.hpp"
#include "ui/widget_factory.hpp"
#include <QBoxLayout>
#include <QCheckBox>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class InspectorMoleculeWidget : public BaseManualWidget<QWidget>
			{
				VTX_MANUAL_WIDGET_DECLARATION

			  public:
				void		refresh();
				inline void localize() override {};

				inline void setModel( Model::Molecule * const p_model )
				{
					_model = p_model;
					refresh();
				};
				inline Model::Molecule * getModel() { return _model; };

			  protected:
				InspectorMoleculeWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {};

				void		 _setupUi( const QString & p_name ) override;
				virtual void _setupSlots() override;

				inline void toggleFoldState() { setFoldState( !_folded ); }
				inline bool getFoldState() { return _folded; }
				void		setFoldState( bool foldState );

			  private:
				QVBoxLayout * _verticalLayout = nullptr;

				// Header
				QWidget *	  _headerWidget = nullptr;
				QHBoxLayout * _headerLayout = nullptr;
				QPushButton * _foldState	= nullptr;
				QCheckBox *	  _enableWidget = nullptr;
				QLabel *	  _symbolWidget = nullptr;
				QLabel *	  _nameWidget	= nullptr;

				// Content
				QWidget *	  _content		 = nullptr;
				QLabel *	  _info			 = nullptr;
				QVBoxLayout * _contentLayout = nullptr;

				bool			  _folded = false;
				Model::Molecule * _model  = nullptr;

				void setModelEnableFromCheckBox( int checkboxState ) { _model->setEnable( checkboxState > 0 ); }
			};

		} // namespace Widget
	}	  // namespace UI
} // namespace VTX

#endif
