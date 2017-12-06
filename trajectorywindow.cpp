#include "trajectorywindow.h"

TrajectoryWindow::TrajectoryWindow(QWidget *parent) : QWidget(parent)
{
    setWindowFlags(Qt::Window);
    QVBoxLayout *mainLayout = new QVBoxLayout();
    frameLabel = new CustomLabel();

    mainLayout->addWidget(frameLabel);
    this->setLayout(mainLayout);
}

void TrajectoryWindow::loop(){
    cv::Mat outFrame(400, 600, CV_8UC3, cv::Scalar(255, 255, 255));
    qFrame = ImageFormat::Mat2QImage(outFrame);
    frameLabel->setPixmap(QPixmap::fromImage(qFrame));
}
