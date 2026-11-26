#ifndef VTX_UI_QT_DIALOG_TRAJECTORY_ASSOCIATION_HPP
#define VTX_UI_QT_DIALOG_TRAJECTORY_ASSOCIATION_HPP

#include "ui/qt/dialog/base_dialog.hpp"
#include <QComboBox>
#include <QPointer>
#include <QRadioButton>
#include <util/filesystem.hpp>

namespace VTX::UI::QT::Dialog
{

	class TrajectoryAssociation : public BaseDialog<TrajectoryAssociation>
	{
	  public:
		TrajectoryAssociation( FilePath );

	  private:
		FilePath			_path;
		QPointer<QComboBox> _cbSystem;
	};
} // namespace VTX::UI::QT::Dialog
#endif
