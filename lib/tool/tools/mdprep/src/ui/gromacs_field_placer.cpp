#include <qformlayout.h>
#include <qspinbox.h>
#include <ui/qt/util.hpp>
//
#include "tools/mdprep/gateway/backend_gromacs.hpp"
#include "tools/mdprep/gateway/form_data.hpp"
#include "tools/mdprep/ui/shared.hpp"
//
#include "tools/mdprep/gromacs/gromacs.hpp"
#include "tools/mdprep/ui/gromacs_field_placer.hpp"

namespace VTX::Tool::Mdprep::ui
{
	using namespace VTX::Tool::Mdprep::Gateway;

	namespace
	{
		void setWidgetRowVisibility( QFormLayout * p_dest, QWidget * p_w, bool p_visibility )
		{
			int					  rowToTakeOut = 0;
			QFormLayout::ItemRole _tmp;
			p_dest->getWidgetPosition( p_w, &rowToTakeOut, &_tmp );
			if ( rowToTakeOut > -1 )
				p_dest->setRowVisible( rowToTakeOut, p_visibility );
		}
		void deleteWidgetRow( QFormLayout * p_dest, QWidget * p_w )
		{
			int					  rowToTakeOut = 0;
			QFormLayout::ItemRole _tmp;
			p_dest->getWidgetPosition( p_w, &rowToTakeOut, &_tmp );
			if ( rowToTakeOut > -1 )
				p_dest->removeRow( rowToTakeOut );
		}

	} // namespace

	GromacsFieldPlacer::GromacsFieldPlacer( GromacsData & p_ ) : _specificData( &p_ ) {}

	GromacsFieldPlacer::~GromacsFieldPlacer()
	{
		if ( _layouts.has_value() )
		{
			deleteWidgetRow( _layouts->baseParametersIon, _fieldConc );
			deleteWidgetRow( _layouts->baseParametersIon, _fieldBoxShape );
			deleteWidgetRow( _layouts->baseParametersIon, _fieldWaterModel );
		}
	}

	void GromacsFieldPlacer::assign( FormLayouts p_layouts ) noexcept { _layouts.emplace( std::move( p_layouts ) ); }

	void GromacsFieldPlacer::activate() noexcept
	{
		if ( not _layouts )
			return;

		if ( _activatedOnce == false )
		{
			_fieldConc = new QDoubleSpinBox;
			_fieldConc->setDecimals( 2 );
			_fieldConc->setSingleStep( 0.01 );
			_fieldConc->setRange( 0., 2 );
			_fieldConc->setValue( 0.15 ); // set default here
			_activatedOnce = true;
			_layouts->baseParametersIon->addRow(
				VTX::UI::QT::Util::LabelWithHelper(
					"Target Salt concentration",
					"If not null, the system will be added ions until the targeted concentration is reached.",
					VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
				),
				_fieldConc
			);

			return;
		}
		setWidgetRowVisibility( _layouts->baseParametersIon, _fieldConc, true );
	}
	void GromacsFieldPlacer::deactivate() noexcept
	{
		if ( not _layouts )
			return;

		setWidgetRowVisibility( _layouts->baseParametersIon, _fieldConc, false );
	}
	void GromacsFieldPlacer::get( const MdEngine *& p_out ) const noexcept {}
	void get( const GromacsFieldPlacer &, EngineSpecificCommonInformation & p_out ) noexcept
	{
		Gateway::getGromacs( p_out );
	}
} // namespace VTX::Tool::Mdprep::ui
