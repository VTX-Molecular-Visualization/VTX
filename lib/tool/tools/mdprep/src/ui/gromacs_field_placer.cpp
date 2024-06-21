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
		void setWidgetRowVisibility( QFormLayout * p_dest, QWidget * p_label, QWidget * p_field, bool p_visibility )
		{
			if ( p_visibility == true )
			{
				p_dest->addRow( p_label, p_field );
				p_label->show();
				p_field->show();
				return;
			}
			int					  rowToTakeOut = 0;
			QFormLayout::ItemRole _tmp;
			p_dest->getWidgetPosition( p_field, &rowToTakeOut, &_tmp );
			if ( rowToTakeOut > -1 )
				p_dest->takeRow( rowToTakeOut );
			p_field->setParent( nullptr );
			p_field->hide();
			p_label->setParent( nullptr );
			p_label->hide();
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

	GromacsFieldPlacer::GromacsFieldPlacer( GromacsData & p_ ) : _specificData( &p_ )
	{
		_fieldConc = new QDoubleSpinBox;
		_fieldConc->setDecimals( 2 );
		_fieldConc->setSingleStep( 0.01 );
		_fieldConc->setRange( 0., 2 );
		_fieldConc->setValue( p_.saltConcentration );
		_labelConc = VTX::UI::QT::Util::LabelWithHelper(
			"Target Salt concentration",
			"If not null, the system will be added ions until the targeted concentration is reached.",
			VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
		);
		_fieldConc->hide();
		_labelConc->hide();
	}

	GromacsFieldPlacer::~GromacsFieldPlacer()
	{
		if ( _layouts.has_value() )
		{
			_fieldConc->deleteLater();
			_labelConc->deleteLater();
			if ( _layouts->baseParametersIon )
				deleteWidgetRow( _layouts->baseParametersIon, _fieldConc );
		}
	}

	void GromacsFieldPlacer::assign( FormLayouts p_layouts ) noexcept { _layouts.emplace( std::move( p_layouts ) ); }

	void GromacsFieldPlacer::activate() noexcept
	{
		if ( not _layouts )
			return;

		if ( _layouts->baseParametersIon )
			setWidgetRowVisibility( _layouts->baseParametersIon, _labelConc, _fieldConc, true );
	}
	void GromacsFieldPlacer::deactivate() noexcept
	{
		if ( not _layouts )
			return;

		if ( _layouts->baseParametersIon )
			setWidgetRowVisibility( _layouts->baseParametersIon, _labelConc, _fieldConc, false );
	}
	void GromacsFieldPlacer::get( const MdEngine *& p_out ) const noexcept {}
	void get( const GromacsFieldPlacer &, EngineSpecificCommonInformation & p_out ) noexcept
	{
		Gateway::getGromacs( p_out );
	}
} // namespace VTX::Tool::Mdprep::ui
