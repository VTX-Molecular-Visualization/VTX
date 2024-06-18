#include <string>
#include <vector>
//
#include "tools/mdprep/ui/shared.hpp"
//
#include "tools/mdprep/gateway/backend_gromacs.hpp"
#include "tools/mdprep/ui/gromacs.hpp"
#include "tools/mdprep/ui/md_engine.hpp"
#include <ui/qt/util.hpp>
//
#include "tools/mdprep/ui/gromacs_specific_field_placer.hpp"

namespace VTX::Tool::Mdprep::ui
{
	GromacsMdStepFieldPlacer::GromacsMdStepFieldPlacer( Gateway::GromacsStep & p_data ) : _data( &p_data ) {}
	void GromacsMdStepFieldPlacer::placeFields( QFormLayout * p_layout ) noexcept
	{
		_field_nstvout = VTX::UI::QT::Util::addUInt64Field(
			p_layout,
			"Velocity Save Interval",
			"Number of steps between each velocity save. 0 means velocities aren't saved."
		);
		_field_nstvout->setText( QString::number( _data->nstvout ) );
		_field_nstenergy = VTX::UI::QT::Util::addUInt64Field(
			p_layout, "Energy Save Interval", "Number of steps between each energy save. 0 means energies aren't saved."
		);
		_field_nstenergy->setText( QString::number( _data->nstenergy ) );
		_field_nstlog = VTX::UI::QT::Util::addUInt64Field(
			p_layout, "Log Save Interval", "Number of steps between each log write. 0 means logs aren't written."
		);
		_field_nstlog->setText( QString::number( _data->nstlog ) );
		if ( _data->nstxout_compressed.has_value() )
		{
			_field_nstxout_compressed = VTX::UI::QT::Util::addUInt64Field(
				p_layout,
				"Compressed Coordinates Save Interval",
				"Number of steps between each coordinate save in a compressed format. 0 means no compressed "
				"coordinates are written."
			);
			_field_nstxout_compressed->setText( QString::number( _data->nstxout_compressed.value() ) );
		}
	}
	void GromacsMdStepFieldPlacer::apply() noexcept
	{
		_data->nstvout	 = _field_nstvout->text().toUInt();
		_data->nstenergy = _field_nstenergy->text().toUInt();
		_data->nstlog	 = _field_nstlog->text().toUInt();
		if ( _data->nstxout_compressed.has_value() )
			_data->nstxout_compressed = _field_nstxout_compressed->text().toUInt();
	}
	void GromacsMdStepFieldPlacer::get( const MdEngine *& p_out ) const noexcept {}

	GromacsSystemFieldPlacer::GromacsSystemFieldPlacer( Gateway::GromacsSystem & p_ ) : _data( &p_ ) {}

	void GromacsSystemFieldPlacer::placeFields( QFormLayout * p_layout ) noexcept {}
	void GromacsSystemFieldPlacer::apply() noexcept {}
	void GromacsSystemFieldPlacer::update( const EngineSpecificCommonInformationFieldUpdate & p_info ) noexcept {}
	void GromacsSystemFieldPlacer::get( const MdEngine *& p_out ) const noexcept {}
	void GromacsSystemFieldPlacer::_updateBoxShape( QFormLayout *, BoxFields & ) noexcept {}
	void GromacsSystemFieldPlacer::_updateBoxSizeMode( QFormLayout *, BoxFields & ) noexcept {}
} // namespace VTX::Tool::Mdprep::ui
