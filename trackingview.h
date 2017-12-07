#ifndef TRACKINGVIEW_H
#define TRACKINGVIEW_H

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

class TrackingView : public QGLWidget
{
    Q_OBJECT
public:
    explicit TrackingView(QWidget *parent = nullptr);
    ~TrackingView();
    cv::Mat image;
signals:

public slots:
    void loop();

protected:
    qreal windowPixelRatio();
    void load();
    void updateTexture(uint8_t *image);

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void mousePressEvent(QMouseEvent *event);

    void drawRect(double w, double h);
    void drawCircle(double x, double y, double radius, int n);
    void drawXYZAxis(double len);

private:
//    QTimer *timer;
//    cv::Mat image;
    uint32_t textures[1];
    int imageWidth, imageHeight;
    int viewX, viewY, viewWidth, viewHeight;
    double aspect;
    double fovy;
    double near, far;
    Eigen::Matrix4d P;
    Eigen::Matrix4d Rt;
};

#endif // TRACKINGVIEW_H
