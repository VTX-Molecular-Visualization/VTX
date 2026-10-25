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
		Selection( QWidget * p_parent = nullptr );

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
		 * @brief Currently selected entities.
		 */
		std::vector<Entity> _entities;

		/**
		 * @brief Transform updated.
		 */
		void _transformUpdated( Registry &, Entity );

		/**
		 * @brief Metadata updated.
		 */
		void _metadataUpdated( Registry &, Entity );

		/**
		 * @brief Refresh from app.
		 */
		VTX::Util::EventHub::ScopedConnection _connTransformChanged;
		VTX::Util::EventHub::ScopedConnection _connMetadataChanged;
	};
} // namespace VTX::UI::QT::Widget

#endif
