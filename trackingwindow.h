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
#include "imageformat.h"

namespace Ui {
class TrackingWindow;
}

class TrackingWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TrackingWindow(QWidget *parent = 0);
    ~TrackingWindow();

private slots:
    void playButtonClicked();
    void timeSliderValueChanged(int value);
    void loop();

private:
    Ui::TrackingWindow *ui;

    QLabel *frameLabel;
    QPushButton *playButton;
    QSlider *timeSlider;
    QTimer *timer;

    cv::VideoCapture cap;
    int framePos;
    cv::Mat frame, lowFrame;
    QImage qFrame;
    int pyrLevel = 3;
    int width, height, lowWidth, lowHeight;
    int nr_frames;
    int fps;

    void load();
};

#endif // TRACKINGWINDOW_H
