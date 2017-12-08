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
    void init(int imageWidth, int imageHeight, Eigen::Matrix4d P, Eigen::Matrix4d Rt);
    uint8_t * image;
    int _imageWidth, _imageHeight;
    void updateTexture(uint8_t *image);
    void mousePressEvent(QMouseEvent *event);

signals:
    void mousePressed( const QPoint& );

public slots:

protected:
    qreal windowPixelRatio();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void drawRect(double w, double h);
    void drawCircle(double x, double y, double radius, int n);
    void drawXYZAxis(double len);

private:
    uint32_t textures[1];
    int viewX, viewY, viewWidth, viewHeight;
    double _aspect;
    Eigen::Matrix4d _P;
    Eigen::Matrix4d _Rt;
};

#endif // TRACKINGVIEW_H
