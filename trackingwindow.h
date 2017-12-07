#ifndef TRACKINGWINDOW_H
#define TRACKINGWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QTimer>
#include <QDebug>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include "trajectorywindow.h"
#include "imageformat.h"
#include "particle_filter.hpp"
#include "customlabel.h"
#include "trackingview.h"
#include "trajectory.h"

namespace Ui {
class TrackingWindow;
}


class TrackingWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TrackingWindow(QWidget *parent = 0);
    ~TrackingWindow();
    vector<Target> trajectory;
    Eigen::Matrix4d P, Rt;

private slots:
    void frameLabelMousePressed(QPoint point);
    void playButtonClicked();
    void timeSliderValueChanged(int value);
    void loop();

protected:
    bool eventFilter(QObject* obj, QEvent* event);

private:
    Ui::TrackingWindow *ui;
    TrajectoryWindow *trajWin;

    TrackingView *trackingView;
    CustomLabel *frameLabel;
    QPushButton *playButton;
    QSlider *timeSlider;
    QTimer *timer;

    cv::VideoCapture cap;
    int framePos;
    cv::Mat frame, background, mask, lowFrame, lowBackground, lowMask;
    QImage qFrame;
    int pyrLevel;
    int width, height, lowWidth, lowHeight;
    int nFrames;
    int fps;

    vector<pf::Particle> particles;
    int nParticles;

    bool running;

    void load();
    static double likelihood(int x, int y, cv::Mat image1, cv::Mat image2, cv::Mat image3);
    void updateTrajectory();
    void drawTrajectory(cv::Mat image, int radius, int len);
    void draw(cv::Mat image);
};

#endif // TRACKINGWINDOW_H
