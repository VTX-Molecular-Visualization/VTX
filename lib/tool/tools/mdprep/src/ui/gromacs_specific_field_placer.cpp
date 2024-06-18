#include <qcombobox.h>
#include <qformlayout.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qvalidator.h>
#include <string>
#include <vector>
//
#include "tools/mdprep/ui/shared.hpp"
//
#include "tools/mdprep/gateway/backend_gromacs.hpp"
#include "tools/mdprep/ui/gromacs.hpp"
#include "tools/mdprep/ui/md_engine.hpp"
#include <ui/qt/util.hpp>
#include <ui/qt/validator.hpp>
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

	void GromacsSystemFieldPlacer::placeFields( QFormLayout * p_layout ) noexcept
	{
		_targetLayout				= p_layout;
		_boxFields.fieldBoxSizeMode = new QComboBox;
		{
			std::vector<std::string> values;
			Gateway::boxDimensionModeCollection( values );
			for ( auto & it_str : values )
				_boxFields.fieldBoxSizeMode->addItem( QString::fromStdString( it_str ) );

			if ( _data->boxSizeMode < _boxFields.fieldBoxSizeMode->count() )
				_boxFields.fieldBoxSizeMode->setCurrentIndex( static_cast<int>( _data->boxSizeMode ) );
		}

		_boxFields.fieldDistance = new QDoubleSpinBox;
		_boxFields.fieldDistance->setMinimum( 0. );
		_boxFields.fieldDistance->setMaximum( 100. );
		_boxFields.fieldDistance->setSingleStep( 0.1 );
		_boxFields.fieldDistance->setDecimals( 1 );
		_boxFields.fieldDistance->setValue( _data->distanceNm );

		_boxFields.fieldSizeX = new QLineEdit;
		_boxFields.fieldSizeX->setValidator( new VTX::UI::QT::QUInt64Validator( _boxFields.fieldSizeX ) );
		_boxFields.fieldSizeX->setText( QString::number( _data->boxDimensions[ 0 ] ) );
		_boxFields.fieldSizeY = new QLineEdit;
		_boxFields.fieldSizeY->setValidator( new VTX::UI::QT::QUInt64Validator( _boxFields.fieldSizeY ) );
		_boxFields.fieldSizeY->setText( QString::number( _data->boxDimensions[ 1 ] ) );
		_boxFields.fieldSizeZ = new QLineEdit;
		_boxFields.fieldSizeZ->setValidator( new VTX::UI::QT::QUInt64Validator( _boxFields.fieldSizeZ ) );
		_boxFields.fieldSizeZ->setText( QString::number( _data->boxDimensions[ 2 ] ) );
		_boxFields.fieldAngleX = new QLineEdit;
		_boxFields.fieldAngleX->setValidator( new VTX::UI::QT::QUInt64Validator( _boxFields.fieldAngleX ) );
		_boxFields.fieldAngleX->setText( QString::number( _data->boxAngles[ 0 ] ) );
		_boxFields.fieldAngleY = new QLineEdit;
		_boxFields.fieldAngleY->setValidator( new VTX::UI::QT::QUInt64Validator( _boxFields.fieldAngleY ) );
		_boxFields.fieldAngleY->setText( QString::number( _data->boxAngles[ 1 ] ) );
		_boxFields.fieldAngleZ = new QLineEdit;
		_boxFields.fieldAngleZ->setValidator( new VTX::UI::QT::QUInt64Validator( _boxFields.fieldAngleZ ) );
		_boxFields.fieldAngleZ->setText( QString::number( _data->boxAngles[ 2 ] ) );
	}
	void GromacsSystemFieldPlacer::apply() noexcept
	{
		_data->boxSizeMode		  = _boxFields.fieldBoxSizeMode->currentIndex();
		_data->distanceNm		  = _boxFields.fieldDistance->value();
		_data->boxDimensions[ 0 ] = _boxFields.fieldSizeX->text().toInt();
		_data->boxDimensions[ 1 ] = _boxFields.fieldSizeY->text().toInt();
		_data->boxDimensions[ 2 ] = _boxFields.fieldSizeZ->text().toInt();
		_data->boxAngles[ 0 ]	  = _boxFields.fieldAngleX->text().toInt();
		_data->boxAngles[ 1 ]	  = _boxFields.fieldAngleY->text().toInt();
		_data->boxAngles[ 2 ]	  = _boxFields.fieldAngleZ->text().toInt();
	}
	void GromacsSystemFieldPlacer::update( const EngineSpecificCommonInformationFieldUpdate & p_info ) noexcept
	{
		if ( p_info.field != E_ENGINE_SPECIFIC_COMMON_INFORMATION_FIELD ::boxShape )
			return;

		_removeFields();

		_currentShape = static_cast<Gateway::E_EDITCONF_BOX_SHAPE>( p_info.newValue );

		switch ( _currentShape )
		{
		case Gateway::E_EDITCONF_BOX_SHAPE::triclinic:
			_addSizeXYZ();
			_addAnglesXYZ();
			break;

		default:
			_addDimensionModeField();

			if ( static_cast<Gateway::E_EDITCONF_BOX_DIMENSION_MODE>( _boxFields.fieldBoxSizeMode->currentIndex() )
				 == Gateway::E_EDITCONF_BOX_DIMENSION_MODE::distance )
			{
				_addDistanceField();
			}
			else if ( static_cast<Gateway::E_EDITCONF_BOX_DIMENSION_MODE>( _boxFields.fieldBoxSizeMode->currentIndex() )
					  == Gateway::E_EDITCONF_BOX_DIMENSION_MODE::size )
			{
				_addSizeOnlyX();
			}
			break;
		}
	}
	void GromacsSystemFieldPlacer::get( const MdEngine *& p_out ) const noexcept {}
	void GromacsSystemFieldPlacer::_updateBoxShape( QFormLayout *, BoxFields & ) noexcept {}
	void GromacsSystemFieldPlacer::_updateBoxSizeMode( QFormLayout *, BoxFields & ) noexcept {}

	void GromacsSystemFieldPlacer::_removeFields() noexcept
	{
		int					  rowToTakeOutIndex = INT32_MAX;
		QFormLayout::ItemRole _tmp;
		_targetLayout->getWidgetPosition( _boxFields.fieldBoxSizeMode, &rowToTakeOutIndex, &_tmp );
		if ( rowToTakeOutIndex == INT32_MAX )
			return;
		_targetLayout->takeRow( rowToTakeOutIndex );
	}
	void GromacsSystemFieldPlacer::_addDimensionModeField() noexcept
	{
		_targetLayout->addRow(
			VTX::UI::QT::Util::LabelWithHelper(
				"Dimension mode",
				"How you want to tell Gromacs the size of the system.<br><br><b>distance</b> means that gromacs will "
				"center the system on the protein cartesian center, then use the farther away atom from this center, "
				"and use it as a reference. The distance value will be the length in nanometers between this reference "
				"and the system edge.<br><b>size</b> means you choose the size of the solid shape. Only one value is "
				"required for symetrical shapes.",
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			),
			_boxFields.fieldBoxSizeMode
		);
	}
	void GromacsSystemFieldPlacer::_addDistanceField() noexcept
	{
		_targetLayout->addRow(
			VTX::UI::QT::Util::LabelWithHelper(
				"Distance (nm)",
				"Distance between the edge of the box and its closest atom.",
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			),
			_boxFields.fieldDistance
		);
	}
	void GromacsSystemFieldPlacer::_addSizeOnlyX() noexcept
	{
		_targetLayout->addRow(
			VTX::UI::QT::Util::LabelWithHelper(
				"Box Size (nm)",
				"Face-to-face distance of the box.",
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			),
			_boxFields.fieldSizeX
		);
	}
	void GromacsSystemFieldPlacer::_addSizeXYZ() noexcept
	{
		_boxFields.containerSizeXYZ = new QWidget;
		QHBoxLayout * layout		= new QHBoxLayout( _boxFields.containerSizeXYZ );
		layout->addWidget( _boxFields.fieldSizeX );
		layout->addWidget( _boxFields.fieldSizeY );
		layout->addWidget( _boxFields.fieldSizeZ );
		_targetLayout->addRow(
			VTX::UI::QT::Util::LabelWithHelper(
				"Box's x, y and z dimensions",
				"Defines the size of each edge of the system's box.",
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			),
			_boxFields.containerSizeXYZ
		);
	}
	void GromacsSystemFieldPlacer::_addAnglesXYZ() noexcept
	{
		_boxFields.containerAngleXYZ = new QWidget;
		QHBoxLayout * layout		 = new QHBoxLayout( _boxFields.containerAngleXYZ );
		layout->addWidget( _boxFields.fieldAngleX );
		layout->addWidget( _boxFields.fieldAngleY );
		layout->addWidget( _boxFields.fieldAngleZ );
		_targetLayout->addRow(
			VTX::UI::QT::Util::LabelWithHelper(
				"Box's angles",
				"Defines the angles of the system's periodic box.",
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			),
			_boxFields.containerAngleXYZ
		);
	}
} // namespace VTX::Tool::Mdprep::ui
