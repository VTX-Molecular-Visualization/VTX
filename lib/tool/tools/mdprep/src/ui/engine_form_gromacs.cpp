#include <qformlayout.h>
#include <qspinbox.h>
#include <ui/qt/util.hpp>
//
#include "tools/mdprep/ui/form_data.hpp"
//
#include "tools/mdprep/ui/engine_form_gromacs.hpp"
//
#include "tools/mdprep/gromacs/gromacs.hpp"

namespace VTX::Tool::Mdprep::ui
{
	void setWidgetRowVisibility( QFormLayout * p_dest, QWidget * p_w, bool p_visibility )
	{
		int					  rowToTakeOut = 0;
		QFormLayout::ItemRole _tmp;
		p_dest->getWidgetPosition( p_w, &rowToTakeOut, &_tmp );
		if ( rowToTakeOut > -1 )
			p_dest->setRowVisible( rowToTakeOut, p_visibility );
	}

	EngineFormGromacs::EngineFormGromacs( FormLayouts p_layout ) : _layouts( std::move( p_layout ) )
	{
		using namespace VTX::Tool::Mdprep::Gromacs;
		auto ffs = listForcefields( VTX::Tool::Mdprep::executableDirectory() / defaultFfDirectoryRelativePath() );
		for ( auto & it_ff : ffs )
		{
			auto ffName = it_ff.getName();
			_commonData.bioForcefieldCollection.push_back( { ffName.begin(), ffName.end() } );
		}
	}
	void EngineFormGromacs::get( const EngineSpecificCommonFormData *& p_ptr ) const noexcept { p_ptr = &_commonData; }

	void EngineFormGromacs::activate() noexcept
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
				VTX::UI::QT::Util::createLabelWithHelpTooltip(
					"Target Salt concentration",
					"If not null, the system will be added ions until the targeted concentration is reached."
				),
				_fieldConc
			);
			return;
		}
		setWidgetRowVisibility( _layouts.baseParameters, _fieldConc, true );
	}
	void EngineFormGromacs::deactivate() noexcept
	{
		setWidgetRowVisibility( _layouts.baseParameters, _fieldConc, false );
	}
} // namespace VTX::Tool::Mdprep::ui
