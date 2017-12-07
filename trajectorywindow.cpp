#include "trajectorywindow.h"
#include "trackingwindow.h"

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
//    updateGL();
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

    int len = 15;
    int totalLen = (int)trajectory.size();
    int begin = len < totalLen ? totalLen - len : 0;
//    Scalar color;
    for(int i = begin; i <  totalLen; i++){
//        int x = trajectory[i].x * pow(2, pyrLevel);
//        int y = trajectory[i].y * pow(2, pyrLevel);
//        int c = (double)255 * (i - begin) / len;
//        color = Scalar(255 - c, c, c);
//        circle(image, Point(x, y), radius, color, -1);
        double X = trajectory[i].X;
        double Y = trajectory[i].Y;
        drawCircle(X, Y, 1.0, 64);
    }
}

void  TrajectoryWindow::drawCircle(double x, double y, double radius, int n){
    glColor4d(1.0, 0.0, 0.0, 1.0);
    glBegin(GL_LINE_LOOP);
    for(int i = 0; i < n; i++){
        double tmpX = radius * cos(2 * M_PI * i / n) + x;
        double tmpY = radius * sin(2 * M_PI * i / n) + y;
        glVertex3d(tmpX, tmpY, 0.0);
    }
    glEnd();
}
