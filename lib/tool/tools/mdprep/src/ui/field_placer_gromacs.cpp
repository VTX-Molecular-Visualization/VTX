#include <qformlayout.h>
#include <qspinbox.h>
#include <ui/qt/util.hpp>
//
#include "tools/mdprep/gateway/backend_gromacs.hpp"
#include "tools/mdprep/gateway/form_data.hpp"
#include "tools/mdprep/ui/shared.hpp"
//
#include "tools/mdprep/ui/specific_field_placer_gromacs.hpp"
#include "tools/mdprep/ui/md_engine_specific_field_placer.hpp"
//
#include "tools/mdprep/ui/field_placer_gromacs.hpp"
//
#include "tools/mdprep/gromacs/gromacs.hpp"

namespace VTX::Tool::Mdprep::ui
{
	using namespace VTX::Tool::Mdprep::Gateway;

	void setWidgetRowVisibility( QFormLayout * p_dest, QWidget * p_w, bool p_visibility )
	{
		int					  rowToTakeOut = 0;
		QFormLayout::ItemRole _tmp;
		p_dest->getWidgetPosition( p_w, &rowToTakeOut, &_tmp );
		if ( rowToTakeOut > -1 )
			p_dest->setRowVisible( rowToTakeOut, p_visibility );
	}

	GromacsFieldPlacer::GromacsFieldPlacer( FormLayouts p_layout ) : _layouts( std::move( p_layout ) ) {}
	void GromacsFieldPlacer::get( MdEngineSpecificFieldPlacer & p_out, const E_FIELD_SECTION & p_section ) noexcept
	{
		switch ( p_section )
		{
		case E_FIELD_SECTION::system:
			p_out = MdEngineSpecificFieldPlacer( GromacsSystemFieldPlacer( _specificData.system ) );
			break;
		case E_FIELD_SECTION::minimization: break;
		case E_FIELD_SECTION::equilibrationNvt:
			p_out = MdEngineSpecificFieldPlacer( GromacsMdStepFieldPlacer( _specificData.nvt ) );
			break;
		case E_FIELD_SECTION::equilibrationNpt:
			p_out = MdEngineSpecificFieldPlacer( GromacsMdStepFieldPlacer( _specificData.npt ) );
			break;
		case E_FIELD_SECTION::production:
			p_out = MdEngineSpecificFieldPlacer( GromacsMdStepFieldPlacer( _specificData.prod ) );
			break;
		default: break;
		}
	}

	void GromacsFieldPlacer::activate() noexcept
	{
		if ( _activatedOnce == false )
		{
			_fieldConc = new QDoubleSpinBox;
			_fieldConc->setDecimals( 2 );
			_fieldConc->setSingleStep( 0.01 );
			_fieldConc->setRange( 0., 2 );
			_fieldConc->setValue( 0.15 ); // set default here
			_activatedOnce = true;
			_layouts.baseParameters->addRow(
				VTX::UI::QT::Util::LabelWithHelper(
					"Target Salt concentration",
					"If not null, the system will be added ions until the targeted concentration is reached.",
					VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
				),
				_fieldConc
			);

			return;
		}
		setWidgetRowVisibility( _layouts.baseParameters, _fieldConc, true );
	}
	void GromacsFieldPlacer::deactivate() noexcept
	{
		setWidgetRowVisibility( _layouts.baseParameters, _fieldConc, false );
	}
	void get( const GromacsFieldPlacer &, EngineSpecificCommonInformation & p_out ) noexcept
	{
		Gateway::getGromacs( p_out );
	}
} // namespace VTX::Tool::Mdprep::ui
