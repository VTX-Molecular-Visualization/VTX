#ifndef __VTX_UI_DIALOG_SET_TRAJECTORY_TARGET__
#define __VTX_UI_DIALOG_SET_TRAJECTORY_TARGET__

#include "ui/old_ui/ui/widget/base_manual_widget.hpp"
#include "ui/old_ui/ui/widget/custom_widget/model_list_widget.hpp"
#include <QAbstractButton>
#include <QButtonGroup>
#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QRadioButton>
#include <QShowEvent>
#include <vector>

namespace VTX
{
	namespace App::Core::Model
	{
		class BaseModel;
	} // namespace App::Core::Model

	namespace Model
	{
		class Molecule;
	} // namespace Model

	namespace UI::Widget::Dialog
	{
		class MoleculeListWidget : public CustomWidget::BaseModelListWidget
		{
			VTX_WIDGET

		  private:
			enum class GRID_LAYOUT_COLUMN : int
			{
				TICK  = 0,
				MODEL = 1,
			};

			class ModelFieldLine : public CustomWidget::BaseModelFieldLine
			{
			  public:
				ModelFieldLine( CustomWidget::BaseModelListWidget * const p_parent );
				~ModelFieldLine();

				App::Core::Model::BaseModel * const getModel() const;
				void								setModel( App::Core::Model::BaseModel * const p_model );

				Model::Molecule * const getMolecule() const { return _linkedMolecule; }

				QCheckBox * getCheckBox() const { return _checkBox; }
				QLabel *	getModelLabel() const { return _modelLabel; }

				bool isTicked() const;
				void setTickState( const bool p_ticked ) const;

			  private:
				QCheckBox * _checkBox	= nullptr;
				QLabel *	_modelLabel = nullptr;

				Model::Molecule * _linkedMolecule = nullptr;
			};

		  public:
			std::vector<Model::Molecule *> getTickedMolecules() const;
			void						   tickMolecule( Model::Molecule * const p_molecule, const bool p_ticked );

		  protected:
			MoleculeListWidget( QWidget * p_parent = nullptr );

			void									 _initColumns() override;
			CustomWidget::BaseModelFieldLine * const _instantiateLine();
			void _addLineInLayout( CustomWidget::BaseModelFieldLine * const p_line, const int p_row );
		};

		class SetTrajectoryTargetDialog : public BaseManualWidget<QDialog>
		{
			VTX_WIDGET

		  private:
		  public:
			static void openDialog();
			static void openDialog( const FilePath & p_trajectoryPath );

			void localize() override;
			void cancelAction();
			void openAction();

		  protected:
			SetTrajectoryTargetDialog( QWidget * p_parent );

			void showEvent( QShowEvent * p_event ) override;

			void _setupUi( const QString & p_name ) override;
			void _setupSlots() override;

			void _refresh();

		  private:
			inline static SetTrajectoryTargetDialog * _instance = nullptr;
			static SetTrajectoryTargetDialog &		  _getInstance();

			void _onTrajectoryImportModeChange( QAbstractButton * p_button, const bool p_checked );

			MoleculeListWidget * _moleculeListWidget			  = nullptr;
			QButtonGroup *		 _trajectoryImportModeButtonGroup = nullptr;
			QRadioButton *		 _linkToMoleculeRadioButton		  = nullptr;
			QRadioButton *		 _createNewMoleculeRadioButton	  = nullptr;
			QDialogButtonBox *	 _dialogButtons					  = nullptr;

			FilePath _filepath = FilePath();
		};
	} // namespace UI::Widget::Dialog
} // namespace VTX

#endif
