#ifndef TRAJECTORYWINDOW_H
#define TRAJECTORYWINDOW_H

#include <QGLWidget>
//#include <QWidget>
#include <opencv2/opencv.hpp>
#include <QVBoxLayout>
#include <QTimer>
#include <QDebug>
#include "customlabel.h"
#include "imageformat.h"

class TrajectoryWindow : public QGLWidget
{
    Q_OBJECT
public:
    explicit TrajectoryWindow(QWidget *parent = nullptr);

signals:

public slots:
    void loop();

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void drawRect(double w, double h);

private:
    CustomLabel *frameLabel;
    QImage qFrame;
};

#endif // TRAJECTORYWINDOW_H
