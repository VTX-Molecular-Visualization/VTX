#ifndef __VTX_UI_QT_WIDGET_SELECTION__
#define __VTX_UI_QT_WIDGET_SELECTION__

#include "ui/qt/widget/transform.hpp"
#include <QListWidget>
#include <QPlainTextEdit>
#include <QPointer>
#include <QVBoxLayout>
#include <QWidget>
#include <app/ecs.hpp>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::Widget
{
	/**
	 * @brief Widget displaying the current selection.
	 */
	class Selection : public QWidget
	{
	  public:
		/**
		 * @brief Constructor.
		 */
		Selection( QWidget * const p_parent = nullptr );
		~Selection();

	  private:
		/**
		 * @brief Main layout.
		 */
		QPointer<QVBoxLayout> _layout;

		/**
		 * @brief List of select items.
		 */
		QPointer<QListWidget> _list;

		/**
		 * @brief Display all full selected system transforms.
		 */
		QPointer<Widget::Transform> _transform;

		/**
		 * @brief Name edition.
		 */
		QPointer<QPlainTextEdit> _textName;

		/**
		 * @brief Refresh from app.
		 */
		Util::EventHub::Connection _connTransformChanged;
		Util::EventHub::Connection _connMetadataChanged;

		/**
		 * @brief Currently selected entities.
		 */
		std::vector<App::ECS::Entity> _entities;

		/**
		 * @brief Transform updated.
		 */
		void _transformUpdated( App::ECS::Registry &, App::ECS::Entity );

		/**
		 * @brief Metadata updated.
		 */
		void _metadataUpdated( App::ECS::Registry &, App::ECS::Entity );
	};
} // namespace VTX::UI::QT::Widget

#endif
