#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QtWidgets/qwidget.h>
#include <QtGui/qpen.h>
#include <QtGui/qbrush.h>
#include <QtGui/qpixmap.h>
#include <QtGui/qfont.h>
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>

class RenderWidget : public QWidget
{
	Q_OBJECT

public:

	enum DrawCmd { Rect, Circle, Polygon, Image, Noting };

	RenderWidget(QWidget *parent = 0);
	~RenderWidget();

	QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setAntialiased(bool antialiased);

	void setPenColor(const QColor& c);

	void clear();
	
	void drawPixmap(int x, int y, int width, int height, const QPixmap& pixmap);
	void drawImage(int x, int y, int width, int height, const QImage& img);

	void drawRect(const QPointF& topleft, const qreal width, const qreal height, QColor color = QColor(0,0,0));
	void fillRect(const QPointF& topleft, const qreal width, const qreal height, QColor color = QColor(0,0,0));

	void drawCircle(const QPointF& center, qreal r, QColor color = QColor(0,0,0));
	void fillCircle(const QPointF& center, qreal r, const QColor c = QColor(0,0,0));

	void drawPolygon(const QList<QPointF> points, int pointCount, QColor color = QColor(0,0,0) );
	void fillPolygon(const QList<QPointF> points, int pointCount, const QColor c = QColor(0,0,0));
	

protected:

    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
	void keyPressEvent(QKeyEvent * event) Q_DECL_OVERRIDE;

private:

	QPainter	painter_;
	QPen		pen_;
    QBrush		brush_;

    bool		antialiased_;
	DrawCmd		cmd_;

	// for drawing circle & ellipse
	QPointF		center_;
	qreal		radius_;

	// for drawing image
	QPixmap	    pixmap_;
	int			x_;
	int			y_;
	int			width_;
	int			height_;

	// for drawing Rectangle
	QRectF		rect_;

	// for drawing Polygon
	QPolygonF	polygon_;

		
};

#endif // RENDERWIDGET_H
