#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
// Required for screen DPI information
#include <QScreen>
#include <QWindow>
#endif

#include <opencv2/opencv.hpp>
#include <QDebug>
#include <QTimer>
#include <QMouseEvent>
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <math.h>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include "converter.h"

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent = nullptr);
    ~GLWidget();

signals:

public slots:
    void loop();

protected:
    qreal windowPixelRatio();
    void load();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void mousePressEvent(QMouseEvent *event);

    void drawRect(double w, double h);
    void drawCircle(double x, double y, double radius, int n);
    void drawXYZAxis(double len);

private:
    QTimer *timer;
    cv::VideoCapture cap;
    cv::Mat image;
    int imageWidth, imageHeight;
    int viewX, viewY, viewWidth, viewHeight;
    double aspect;
    double fovy;
    double near, far;
    Eigen::Matrix4d P;
    Eigen::Matrix4d Rt;
};

#endif // GLWIDGET_H
