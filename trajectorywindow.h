#ifndef TRAJECTORYWINDOW_H
#define TRAJECTORYWINDOW_H

#include <QWidget>
#include <opencv2/opencv.hpp>
#include <QVBoxLayout>
#include <QTimer>
#include <QDebug>
#include "customlabel.h"
#include "imageformat.h"

class TrajectoryWindow : public QWidget
{
    Q_OBJECT
public:
    explicit TrajectoryWindow(QWidget *parent = nullptr);

signals:

public slots:
    void loop();

private:
    CustomLabel *frameLabel;
    QImage qFrame;
};

#endif // TRAJECTORYWINDOW_H
