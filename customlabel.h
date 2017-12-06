#ifndef CUSTOMLABEL_H
#define CUSTOMLABEL_H

#include <QMouseEvent>
#include <QLabel>

class CustomLabel : public QLabel
{
Q_OBJECT
signals:
    void mousePressed( const QPoint& );

public:
    CustomLabel( QWidget* parent = 0, Qt::WindowFlags f = 0 );
    CustomLabel( const QString& text, QWidget* parent = 0, Qt::WindowFlags f = 0 );

    void mousePressEvent( QMouseEvent* ev );
};

#endif // CUSTOMLABEL_H
