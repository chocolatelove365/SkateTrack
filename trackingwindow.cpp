#include "trackingwindow.h"
#include "ui_trackingwindow.h"

TrackingWindow::TrackingWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TrackingWindow)
{
    ui->setupUi(this);

    QWidget *mainWidget = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout();
    frameLabel = new QLabel();
    playButton = new QPushButton();
    timeSlider = new QSlider(Qt::Horizontal);
    timer = new QTimer(this);

    connect(playButton, SIGNAL(clicked()), this, SLOT(playButtonClicked()));
    connect(timeSlider, SIGNAL(valueChanged(int)), this, SLOT(timeSliderValueChanged(int)));
    connect(timer, SIGNAL(timeout()), this, SLOT(loop()));

    mainLayout->addWidget(frameLabel);
    mainLayout->addWidget(playButton);
    mainLayout->addWidget(timeSlider);
    mainWidget->setLayout(mainLayout);
    this->setCentralWidget(mainWidget);

    load();
}

TrackingWindow::~TrackingWindow()
{
    delete ui;
}

void TrackingWindow::load(){
    QString videoFileName = "/Users/tomiya/Desktop/NHK杯フィギュアスケート/image/NHK杯連続写真01/RM1_0001_0781_2.mp4";
    cap.open(videoFileName.toStdString().c_str());
    if(!cap.isOpened()){
            qDebug() << "ERROR: failed to open video.\n";
            return;
    }
    width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
    lowWidth = width / pow(2, pyrLevel);
    lowHeight = height / pow(2, pyrLevel);
    nr_frames = cap.get(CV_CAP_PROP_FRAME_COUNT);
    fps = cap.get(CV_CAP_PROP_FPS);

    timeSlider->setRange(0, nr_frames);
//    timer->setInterval(1000 / fps);
    timer->setInterval(1);
    framePos = 0;
}

void TrackingWindow::playButtonClicked(){
    if(!timer->isActive()) {
        timer->start();
    }
    else {
        timer->stop();
    }
}

void TrackingWindow::timeSliderValueChanged(int value){
    framePos = value;
    cap.set(CV_CAP_PROP_POS_FRAMES, value);
}

void TrackingWindow::loop(){
    framePos++;
    timeSlider->blockSignals(true);
    timeSlider->setValue(framePos);
    timeSlider->blockSignals(false);
    cap >> frame;
    if(frame.empty()){
        timer->stop();
    }
    else{
        cv::resize(frame, lowFrame, cv::Size(lowWidth, lowHeight));

        qFrame = ImageFormat::Mat2QImage(lowFrame);
        frameLabel->setPixmap(QPixmap::fromImage(qFrame));
    }
}
