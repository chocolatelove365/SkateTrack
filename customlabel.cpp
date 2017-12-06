#include "customlabel.h"

void CustomLabel::mousePressEvent( QMouseEvent* ev )
{
    const QPoint p = ev->pos();
    emit mousePressed( p );
}

CustomLabel::CustomLabel( QWidget * parent, Qt::WindowFlags f )
    : QLabel( parent, f ) {}

CustomLabel::CustomLabel( const QString& text, QWidget* parent, Qt::WindowFlags f )
    : QLabel( text, parent, f ) {}
