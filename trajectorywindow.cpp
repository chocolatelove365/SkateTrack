#include "trajectorywindow.h"

TrajectoryWindow::TrajectoryWindow(QWidget *parent) : QGLWidget(parent)
{
    setWindowFlags(Qt::Window);
    resize(600, 400);
//    QVBoxLayout *mainLayout = new QVBoxLayout();
//    frameLabel = new CustomLabel();

//    mainLayout->addWidget(frameLabel);
//    this->setLayout(mainLayout);
}

void TrajectoryWindow::loop(){
//    cv::Mat outFrame(400, 600, CV_8UC3, cv::Scalar(255, 255, 255));
//    qFrame = ImageFormat::Mat2QImage(outFrame);
//    frameLabel->setPixmap(QPixmap::fromImage(qFrame));
    updateGL();
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
}
