#include <qcombobox.h>
#include <qformlayout.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qvalidator.h>
#include <string>
#include <thread>
#include <vector>
//
#include "tool/mdprep/gateway/form_data.hpp"
#include "tool/mdprep/gateway/shared.hpp"
//
#include "tool/mdprep/ui/shared.hpp"
//
#include "tool/mdprep/gateway/backend_gromacs.hpp"
#include "tool/mdprep/ui/gromacs.hpp"
#include "tool/mdprep/ui/md_engine.hpp"
#include <ui/qt/util.hpp>
#include <ui/qt/validator.hpp>
//
#include "tool/mdprep/ui/gromacs_specific_field_placer.hpp"

namespace VTX::Tool::Mdprep::ui
{
	GromacsMdStepFieldPlacer::GromacsMdStepFieldPlacer( Gateway::Gromacs::MdStep & p_data ) : _data( &p_data ) {}
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

	GromacsSystemFieldPlacer::GromacsSystemFieldPlacer( Gateway::Gromacs::SystemSettings & p_ ) : _data( &p_ ) {}
	namespace
	{
		inline void createInt64HiddenField( QWidget * p_parent, QLineEdit *& p_field, const double & p_default )
		{
			p_field = new QLineEdit( p_parent );
			p_field->setValidator( new VTX::UI::QT::QUInt64Validator( p_field ) );
			p_field->setText( QString::number( p_default ) );
			p_field->hide();
		}
	} // namespace
	void GromacsSystemFieldPlacer::placeFields( QFormLayout * p_layout ) noexcept
	{
		_targetLayout = p_layout;

		_boxFields.fieldBoxSizeMode = new QComboBox( _targetLayout->parentWidget() );
		_boxFields.fieldBoxSizeMode->hide();
		{
			std::vector<std::string> values;
			Gateway::Gromacs::boxDimensionModeCollection( values );
			for ( auto & it_str : values )
				_boxFields.fieldBoxSizeMode->addItem( QString::fromStdString( it_str ) );

			if ( _data->boxSizeMode < static_cast<size_t>( _boxFields.fieldBoxSizeMode->count() ) )
				_boxFields.fieldBoxSizeMode->setCurrentIndex( static_cast<int>( _data->boxSizeMode ) );
		}
		QObject::connect(
			_boxFields.fieldBoxSizeMode, &QComboBox::currentIndexChanged, [ & ] { this->_updateBoxSizeMode(); }
		);

		_boxFields.fieldDistance = new QDoubleSpinBox( _targetLayout->parentWidget() );
		_boxFields.fieldDistance->setMinimum( 0. );
		_boxFields.fieldDistance->setMaximum( 100. );
		_boxFields.fieldDistance->setSingleStep( 0.1 );
		_boxFields.fieldDistance->setDecimals( 1 );
		_boxFields.fieldDistance->setValue( _data->distanceNm );
		_boxFields.fieldDistance->hide();

		createInt64HiddenField( _targetLayout->parentWidget(), _boxFields.fieldSizeX, _data->boxDimensions[ 0 ] );
		createInt64HiddenField( _targetLayout->parentWidget(), _boxFields.fieldSizeY, _data->boxDimensions[ 1 ] );
		createInt64HiddenField( _targetLayout->parentWidget(), _boxFields.fieldSizeZ, _data->boxDimensions[ 2 ] );
		createInt64HiddenField( _targetLayout->parentWidget(), _boxFields.fieldAngleX, _data->boxAngles[ 0 ] );
		createInt64HiddenField( _targetLayout->parentWidget(), _boxFields.fieldAngleY, _data->boxAngles[ 1 ] );
		createInt64HiddenField( _targetLayout->parentWidget(), _boxFields.fieldAngleZ, _data->boxAngles[ 2 ] );
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
		if ( p_info.field != E_ENGINE_SPECIFIC_COMMON_INFORMATION_FIELD ::boxShape
			 && p_info.field != E_ENGINE_SPECIFIC_COMMON_INFORMATION_FIELD ::boxDimensionMode )
			return;

		_removeFields();

		_currentShape = static_cast<Gateway::Gromacs::E_BOX_SHAPE>( p_info.newValue );

		switch ( _currentShape )
		{
		case Gateway::Gromacs::E_BOX_SHAPE::triclinic:
			_addSizeXYZ();
			_addAnglesXYZ();
			break;

		default:
			_addDimensionModeField();

			if ( static_cast<Gateway::Gromacs::E_BOX_DIMENSION_MODE>( _boxFields.fieldBoxSizeMode->currentIndex() )
				 == Gateway::Gromacs::E_BOX_DIMENSION_MODE::distance )
			{
				_addDistanceField();
			}
			else if ( static_cast<Gateway::Gromacs::E_BOX_DIMENSION_MODE>( _boxFields.fieldBoxSizeMode->currentIndex() )
					  == Gateway::Gromacs::E_BOX_DIMENSION_MODE::size )
			{
				_addSizeOnlyX();
			}
			break;
		}
	}
	void GromacsSystemFieldPlacer::_updateBoxSizeMode() noexcept
	{
		update( EngineSpecificCommonInformationFieldUpdate {
			.field	  = E_ENGINE_SPECIFIC_COMMON_INFORMATION_FIELD ::boxDimensionMode,
			.newValue = static_cast<size_t>( _boxFields.fieldBoxSizeMode->currentIndex() ) } );
	}
	namespace
	{
		void removeField( QFormLayout * p_from, QWidget * p_what ) noexcept
		{
			int					  rowToTakeOutIndex = INT32_MAX;
			QFormLayout::ItemRole _tmp;
			p_from->getWidgetPosition( p_what, &rowToTakeOutIndex, &_tmp );
			if ( rowToTakeOutIndex == INT32_MAX || rowToTakeOutIndex == -1 )
				return;
			auto stuffTakenOut = p_from->takeRow( rowToTakeOutIndex );
			stuffTakenOut.fieldItem->widget()->hide();
			stuffTakenOut.labelItem->widget()->hide();
		}
	} // namespace
	void GromacsSystemFieldPlacer::_removeFields() noexcept
	{
		removeField( _targetLayout, _boxFields.fieldBoxSizeMode );
		removeField( _targetLayout, _boxFields.fieldDistance );
		removeField( _targetLayout, _boxFields.containerSizeXYZ );
		removeField( _targetLayout, _boxFields.fieldSizeX );
		removeField( _targetLayout, _boxFields.fieldSizeY );
		removeField( _targetLayout, _boxFields.fieldSizeZ );
		removeField( _targetLayout, _boxFields.containerAngleXYZ );
		removeField( _targetLayout, _boxFields.fieldAngleX );
		removeField( _targetLayout, _boxFields.fieldAngleY );
		removeField( _targetLayout, _boxFields.fieldAngleZ );
	}
	void GromacsSystemFieldPlacer::_addDimensionModeField() noexcept
	{
		_boxFields.fieldBoxSizeMode->show();
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
		_boxFields.fieldDistance->show();
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
		_boxFields.fieldSizeX->show();
		_targetLayout->addRow(
			VTX::UI::QT::Util::LabelWithHelper(
				"Box Size (nm)",
				"Face-to-face distance of the box in nanometers.",
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			),
			_boxFields.fieldSizeX
		);
	}
	void GromacsSystemFieldPlacer::_addSizeXYZ() noexcept
	{
		_boxFields.containerSizeXYZ = new QWidget;
		QHBoxLayout * layout		= new QHBoxLayout( _boxFields.containerSizeXYZ );
		layout->setContentsMargins( { 0, 0, 0, 0 } );
		layout->addWidget( _boxFields.fieldSizeX );
		layout->addWidget( _boxFields.fieldSizeY );
		layout->addWidget( _boxFields.fieldSizeZ );
		_boxFields.fieldSizeX->show();
		_boxFields.fieldSizeY->show();
		_boxFields.fieldSizeZ->show();
		_targetLayout->addRow(
			VTX::UI::QT::Util::LabelWithHelper(
				"Box's x, y and z dimensions",
				"Defines the size of each edge of the system's box in nanometers.",
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			),
			_boxFields.containerSizeXYZ
		);
	}
	void GromacsSystemFieldPlacer::_addAnglesXYZ() noexcept
	{
		_boxFields.containerAngleXYZ = new QWidget;
		QHBoxLayout * layout		 = new QHBoxLayout( _boxFields.containerAngleXYZ );
		layout->setContentsMargins( { 0, 0, 0, 0 } );
		layout->addWidget( _boxFields.fieldAngleX );
		layout->addWidget( _boxFields.fieldAngleY );
		layout->addWidget( _boxFields.fieldAngleZ );
		_boxFields.fieldAngleX->show();
		_boxFields.fieldAngleY->show();
		_boxFields.fieldAngleZ->show();
		_targetLayout->addRow(
			VTX::UI::QT::Util::LabelWithHelper(
				"Box's angles",
				"Defines the angles of the system's periodic box in degrees.",
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			),
			_boxFields.containerAngleXYZ
		);
	}
} // namespace VTX::Tool::Mdprep::ui
