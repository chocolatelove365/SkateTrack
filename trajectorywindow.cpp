#include "trajectorywindow.h"
#include "trackingwindow.h"

TrajectoryWindow::TrajectoryWindow(QWidget *parent) : QGLWidget(parent)
{
    setWindowFlags(Qt::Window);
    resize(600, 400);
}

void TrajectoryWindow::initializeGL(){
    glClearColor(0.f, 0.f, 0.f, 1.f);
}

void TrajectoryWindow::resizeGL(int w, int h){

}

void TrajectoryWindow::paintGL(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-30.0, 30.0, -20.0, 20.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    drawRect(40, 30);
}

void TrajectoryWindow::drawRect(double w, double h){
    glColor4d(1.0, 0.0, 0.0, 1.0);
    glBegin(GL_LINE_LOOP);
    glVertex3d(+w/2, +h/2, 0.0);
    glVertex3d(-w/2, +h/2, 0.0);
    glVertex3d(-w/2, -h/2, 0.0);
    glVertex3d(+w/2, -h/2, 0.0);
    glEnd();

    std::vector<Target> trajectory = ((TrackingWindow*)(parent()))->trajectory;
    int totalLen = (int)trajectory.size();
    int len = 15;
    int begin = len < totalLen ? totalLen - len : 0;
    for(int i = begin; i <  totalLen; i++){
        double X = trajectory[i].X;
        double Y = trajectory[i].Y;
        double Z = trajectory[i].Z;
        double c = (double) (i - begin) / len;
        glColor4d(c, c, 1.0 - c, 1.0);
        double radius = 0.5;
        drawCircle(X, Y, Z, radius, 64);
    }

    glColor4d(1.0, 1.0, 1.0, 1.0);
    glBegin(GL_LINE_STRIP);
    for(int i = 0; i <  totalLen; i++){
        double X = trajectory[i].X;
        double Y = trajectory[i].Y;
        double Z = trajectory[i].Z;
        glVertex3d(X, Y, Z);
    }
    glEnd();
}

void  TrajectoryWindow::drawCircle(double X, double Y, double Z, double radius, int n){
    glBegin(GL_POLYGON);
    for(int i = 0; i < n; i++){
        double tmpX = radius * cos(2 * M_PI * i / n) + X;
        double tmpY = radius * sin(2 * M_PI * i / n) + Y;
        glVertex3d(tmpX, tmpY, Z);
    }
    glEnd();
}
