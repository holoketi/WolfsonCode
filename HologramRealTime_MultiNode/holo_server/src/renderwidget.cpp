#include "holo_server/renderwidget.h"

RenderWidget::RenderWidget(QWidget *parent)
	: QWidget(parent)
{
    antialiased_ = true;
	
	setWindowTitle("Sever Side");
	resize(600,400);
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(false);

    setStyleSheet(QStringLiteral("background-color: rgb(0, 0, 0);"));

	pen_.setStyle(Qt::SolidLine);
	pen_.setColor(Qt::black);
	brush_.setStyle(Qt::SolidPattern);
	brush_.setColor(Qt::black);

}

RenderWidget::~RenderWidget()
{

}

void RenderWidget::keyPressEvent(QKeyEvent * event)
{
	int key = event->key();
	if (key == Qt::Key_F10)
		this->showFullScreen();
	else if (key == Qt::Key_F9){
		this->showNormal();
	}else if (key == Qt::Key_Escape)
		exit(0);
}

QSize RenderWidget::minimumSizeHint() const
{
    return QSize(600, 400);
}

QSize RenderWidget::sizeHint() const
{
    return QSize(600, 400);
}

void RenderWidget::clear()
{
	cmd_ = Noting; 

	update();

}
void RenderWidget::drawPixmap(int x, int y, int width, int height, const QPixmap& pixmap)
{
	cmd_ = Image;
	pixmap_ = pixmap;
	x_ = x;
	y_ = y;
	width_ = width;
	height_ = height;

	update();

}
void RenderWidget::drawImage(int x, int y, int width, int height, const QImage& img)
{
	cmd_ = Image;
	pixmap_ = QPixmap::fromImage(img);
	x_ = x;
	y_ = y;
	width_ = width;
	height_ = height;

	update();
}
void RenderWidget::drawRect(const QPointF& topleft, const qreal width, const qreal height, QColor color)
{
	cmd_ = Rect;
	rect_ = QRectF(topleft.x(), topleft.y(), width, height);

	pen_.setStyle(Qt::SolidLine);
	pen_.setColor(color);
	brush_.setStyle(Qt::NoBrush);

	update();

}

void RenderWidget::fillRect(const QPointF& topleft, const qreal width, const qreal height, QColor color )
{
	cmd_ = Rect;
	rect_ = QRectF(topleft.x(), topleft.y(), width, height);

	pen_.setStyle(Qt::SolidLine);
	pen_.setColor(color);
	brush_.setStyle(Qt::SolidPattern);
	brush_.setColor(color);

	update();

}

void RenderWidget::drawCircle(const QPointF& center, qreal r, QColor color)
{
	cmd_ = Circle;
	center_ = center;
	radius_ = r;

	pen_.setStyle(Qt::SolidLine);
	pen_.setColor(color);
	brush_.setStyle(Qt::NoBrush);

	update();
	
}
void RenderWidget::fillCircle(const QPointF& center, qreal r, const QColor c)
{
	cmd_ = Circle;
	center_ = center;
	radius_ = r;

	pen_.setStyle(Qt::SolidLine);
	pen_.setColor(c);
	brush_.setStyle(Qt::SolidPattern);
	brush_.setColor(c);

	update();
}

void RenderWidget::drawPolygon(const QList<QPointF> points, int pointCount , QColor color)
{
	cmd_ = Polygon;
	
	polygon_.clear();
	for(int i=0; i<pointCount; i++)
		polygon_.append(points[i]);

	pen_.setStyle(Qt::SolidLine);
	pen_.setColor(color);
	brush_.setStyle(Qt::NoBrush);

	update();

}
void RenderWidget::fillPolygon(const QList<QPointF> points, int pointCount, const QColor c)
{
	cmd_ = Polygon;
	
	polygon_.clear();
	for(int i=0; i<pointCount; i++)
		polygon_.append(points[i]);

	pen_.setStyle(Qt::SolidLine);
	pen_.setColor(c);
	brush_.setStyle(Qt::SolidPattern);
	brush_.setColor(c);

	update();

}

void RenderWidget::paintEvent(QPaintEvent * ev )
{	
	painter_.begin(this);
	painter_.setPen(pen_);
	painter_.setBrush(brush_);
	if (antialiased_)
		painter_.setRenderHint(QPainter::Antialiasing, true);
	else
		painter_.setRenderHint(QPainter::Antialiasing, false);

	switch(cmd_){
		case Rect:
			painter_.drawRect(rect_);
			break;
		case Circle:
			painter_.drawEllipse(center_, radius_, radius_);
			break;
		case Polygon:
			painter_.drawPolygon(polygon_);
			break;
		
		case Image:
			painter_.drawPixmap(x_,y_,width_,height_,pixmap_);
			break;
	}

	painter_.end();
}

void RenderWidget::setPen(const QPen &pen)
{
    this->pen_ = pen;
    update();
}

void RenderWidget::setBrush(const QBrush &brush)
{
    this->brush_ = brush;
    update();
}

void RenderWidget::setAntialiased(bool antialiased)
{
    this->antialiased_ = antialiased;
    update();
}

void RenderWidget::setPenColor(const QColor& c)
{
	pen_.setColor(c);
}
