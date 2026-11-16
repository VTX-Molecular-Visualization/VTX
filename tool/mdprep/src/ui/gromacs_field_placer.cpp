#include "tool/mdprep/ui/gromacs_field_placer.hpp"
#include "tool/mdprep/gateway/backend_gromacs.hpp"
#include "tool/mdprep/gateway/form_data.hpp"
#include "tool/mdprep/ui/gromacs_specific_field_placer.hpp"
#include "tool/mdprep/ui/shared.hpp"
#include <memory>
#include <qformlayout.h>
#include <qspinbox.h>
#include <ui/qt/util.hpp>

namespace VTX::Tool::Mdprep::ui
{
	using namespace VTX::Tool::Mdprep::Gateway;

	namespace
	{
		void setWidgetRowVisibility( QFormLayout * p_dest, QWidget * p_label, QWidget * p_field, bool p_visibility )
		{
			if ( p_visibility )
			{
				p_dest->addRow( p_label, p_field );
				p_label->show();
				p_field->show();
				return;
			}
			int					  rowToTakeOut = 0;
			QFormLayout::ItemRole _tmp {};
			p_dest->getWidgetPosition( p_field, &rowToTakeOut, &_tmp );
			if ( rowToTakeOut > -1 )
			{
				p_dest->takeRow( rowToTakeOut );
			}
			p_field->setParent( nullptr );
			p_field->hide();
			p_label->setParent( nullptr );
			p_label->hide();
		}

	} // namespace

	GromacsFieldPlacer::GromacsFieldPlacer( Gateway::Gromacs::MdSettings & p_ ) : _specificData( &p_ )
	{
		_uiObjects._fieldConc = std::make_unique<QDoubleSpinBox>().release();
		_toDeleteObjects[ 0 ] = _uiObjects._fieldConc;
		_uiObjects._fieldConc->setDecimals( 2 );
		_uiObjects._fieldConc->setSingleStep( 0.01 );
		_uiObjects._fieldConc->setRange( 0., 2 );
		_uiObjects._fieldConc->setValue( p_.system.saltConcentration );
		_uiObjects._labelConc = VTX::UI::QT::Util::LabelWithHelper(
			"Target Salt concentration (mol/L)",
			"If the value is not null, ions will be added to the system until the targeted concentration is reached.",
			VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left );
		_toDeleteObjects[ 1 ] = _uiObjects._labelConc;
		_uiObjects._fieldConc->hide();
		_uiObjects._labelConc->hide();
	}

	void GromacsFieldPlacer::assign( FormLayouts p_layouts ) noexcept { _layouts.emplace( p_layouts ); }

	void GromacsFieldPlacer::activate()
	{
		if ( not _layouts )
		{
			return;
		}

		if ( _layouts->baseParametersIon )
		{
			setWidgetRowVisibility( _layouts->baseParametersIon, _uiObjects._labelConc, _uiObjects._fieldConc, true );
		}

		if ( _layouts->advancedParametersSystemPrepartion )
		{
			setWidgetRowVisibility(
				_layouts->advancedParametersSystemPrepartion, _uiObjects._labelConc, _uiObjects._fieldConc, true );
		}
	}

	void GromacsFieldPlacer::deactivate()
	{
		if ( not _layouts )
		{
			return;
		}

		if ( _layouts->baseParametersIon )
		{
			setWidgetRowVisibility( _layouts->baseParametersIon, _uiObjects._labelConc, _uiObjects._fieldConc, false );
		}

		if ( _layouts->advancedParametersSystemPrepartion )
		{
			setWidgetRowVisibility(
				_layouts->advancedParametersSystemPrepartion, _uiObjects._labelConc, _uiObjects._fieldConc, false );
		}
	}

	namespace
	{
		void get( const GromacsFieldPlacer &, EngineSpecificCommonInformation & p_out )
		{ Gateway::Gromacs::get( p_out ); }
	} // namespace
} // namespace VTX::Tool::Mdprep::ui
