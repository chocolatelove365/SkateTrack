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
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include "trajectorywindow.h"
#include "imageformat.h"
#include "particle_filter.hpp"
#include "customlabel.h"
#include "trackingview.h"
#include "trajectory.h"

#define SMOOTH_TRAJECTORY 1

namespace Ui {
class TrackingWindow;
}


class TrackingWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TrackingWindow(QWidget *parent = 0);
    ~TrackingWindow();
    vector<Target> trajectory, smoothTrajectory;
    Eigen::Matrix4d P, Rt;

private slots:
    void trackingViewMousePressed(QPoint point);
    void playButtonClicked();
    void timeSliderValueChanged(int value);
    void loop();

protected:
    bool eventFilter(QObject* obj, QEvent* event);

private:
    Ui::TrackingWindow *ui;
    TrajectoryWindow *trajWin;

    TrackingView *trackingView;
    QPushButton *playButton;
    QSlider *timeSlider;
    QTimer *timer;

    QString videoFileName, backgroundFileName, maskFileName;
    cv::VideoCapture cap;
    int framePos;
    cv::Mat frame, background, mask, lowFrame, lowBackground, lowMask;
    int pyrLevel;
    int width, height, lowWidth, lowHeight;
    int nFrames;
    int fps;

    vector<pf::Particle> particles;
    int nParticles;

    bool running;

    void loadConfig(const QString &fileName);
    void saveCSV(const QString &fileName);
    void load();
    static double likelihood(int x, int y, cv::Mat image1, cv::Mat image2, cv::Mat image3);
    static double likelihood(int x, int y, cv::Mat image);
    void updateTrajectory();
    void drawTrajectory(cv::Mat image, int radius, int len);
    void draw(cv::Mat image);
};

#endif // TRACKINGWINDOW_H
