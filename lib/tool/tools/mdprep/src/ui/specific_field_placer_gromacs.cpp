#include "tools/mdprep/gateway/backend_gromacs.hpp"
#include <ui/qt/util.hpp>
//
#include "tools/mdprep/ui/specific_field_placer_gromacs.hpp"

namespace VTX::Tool::Mdprep::ui
{
	SpecificFieldPlacerGromacs::SpecificFieldPlacerGromacs( Gateway::GromacsStep & p_data ) : _data( &p_data ) {}
	void SpecificFieldPlacerGromacs::placeFields( QFormLayout * p_layout ) noexcept
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
				"coordinates "
				"are written."
			);
			_field_nstlog->setText( QString::number( _data->nstxout_compressed.value() ) );
		}
	}
	void SpecificFieldPlacerGromacs::apply() noexcept
	{
		_data->nstvout			  = _field_nstvout->text().toUInt();
		_data->nstenergy		  = _field_nstvout->text().toUInt();
		_data->nstlog			  = _field_nstvout->text().toUInt();
		_data->nstxout_compressed = _field_nstvout->text().toUInt();
	}
} // namespace VTX::Tool::Mdprep::ui
