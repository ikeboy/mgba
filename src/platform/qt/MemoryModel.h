/* Copyright (c) 2013-2015 Jeffrey Pfau
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#ifndef QGBA_MEMORY_MODEL
#define QGBA_MEMORY_MODEL

#include <QAbstractScrollArea>
#include <QFont>
#include <QSize>
#include <QStaticText>
#include <QVector>

struct ARMCore;

namespace QGBA {

class GameController;

class MemoryModel : public QAbstractScrollArea {
Q_OBJECT

public:
	MemoryModel(QWidget* parent = nullptr);

	void setController(GameController* controller);

	void setRegion(uint32_t base, uint32_t size, const QString& name = QString());

	void setAlignment(int);
	int alignment() const { return m_align; }

public slots:
	void jumpToAddress(const QString& hex);
	void jumpToAddress(uint32_t);

signals:
	void selectionChanged(uint32_t start, uint32_t end);

protected:
	void resizeEvent(QResizeEvent*) override;
	void paintEvent(QPaintEvent*) override;
	void wheelEvent(QWheelEvent*) override;
	void mousePressEvent(QMouseEvent*) override;
	void mouseMoveEvent(QMouseEvent*) override;
	void keyPressEvent(QKeyEvent*) override;

private:
	void boundsCheck();

	bool isInSelection(uint32_t address);
	bool isEditing(uint32_t address);
	void drawEditingText(QPainter& painter, const QPointF& origin);

	ARMCore* m_cpu;
	QFont m_font;
	int m_cellHeight;
	int m_letterWidth;
	uint32_t m_base;
	uint32_t m_size;
	int m_top;
	int m_align;
	QMargins m_margins;
	QVector<QStaticText> m_staticNumbers;
	QVector<QStaticText> m_staticAscii;
	QStaticText m_regionName;
	QSizeF m_cellSize;
	QPair<uint32_t, uint32_t> m_selection;
	uint32_t m_selectionAnchor;
	uint32_t m_buffer;
	int m_bufferedNybbles;
};

}

#endif
