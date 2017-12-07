#include "trackingwindow.h"
#include "ui_trackingwindow.h"

TrackingWindow::TrackingWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TrackingWindow)
{
    ui->setupUi(this);
    resize(800, 600);

    trajWin = new TrajectoryWindow(this);
    trajWin->show();

    QWidget *mainWidget = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout();
    trackingView = new TrackingView();
    frameLabel = new CustomLabel();
    playButton = new QPushButton();
    timeSlider = new QSlider(Qt::Horizontal);
    timer = new QTimer(this);

//    connect(frameLabel, SIGNAL(mousePressed(QPoint)), this, SLOT(frameLabelMousePressed(QPoint)));
    connect(trackingView, SIGNAL(mousePressed(QPoint)), this, SLOT(frameLabelMousePressed(QPoint)));
    connect(playButton, SIGNAL(clicked()), this, SLOT(playButtonClicked()));
    connect(timeSlider, SIGNAL(valueChanged(int)), this, SLOT(timeSliderValueChanged(int)));
    connect(timer, SIGNAL(timeout()), this, SLOT(loop()));

    mainLayout->addWidget(trackingView);
    mainLayout->addWidget(playButton);
    mainLayout->addWidget(timeSlider);
    mainWidget->setLayout(mainLayout);
    this->setCentralWidget(mainWidget);

    installEventFilter(this);

    load();

    nParticles = 500;
    pf::init(particles, nParticles, lowWidth / 2, lowHeight / 2);

    timer->start();
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
    pyrLevel = 3;
    width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
    lowWidth = width / pow(2, pyrLevel);
    lowHeight = height / pow(2, pyrLevel);
    nFrames = cap.get(CV_CAP_PROP_FRAME_COUNT);
    fps = cap.get(CV_CAP_PROP_FPS);

    timeSlider->setRange(0, nFrames);
//    timer->setInterval(1000 / fps);
    timer->setInterval(1);
    framePos = 0;

    double _aspect = (double)width / height;
    double _fovy = 48.6681031196 / 180 * M_PI;
    double _far = 100.0;
   double _near = 1.0;
   double f = 1 / tan(_fovy / 2);
   P << f / _aspect, 0.0, 0.0, 0.0,
           0.0, f, 0.0, 0.0,
           0.0, 0.0, (_far + _near) / (_near - _far), 2 * _far * _near / (_near - _far),
           0.0, 0.0, -1.0, 0.0;
   Rt << 0.84080619, -0.54098684, -0.01945062, -0.06907567,
           0.13738744, 0.17849864, 0.97430122, -0.00576485,
           -0.5236122, -0.82187068, 0.22440754, -53.0946064,
           0.0, 0.0, 0.0, 1.0;

    trackingView->init(width, height, P, Rt);

    QString backgroundFileName = "/Users/tomiya/Desktop/NHK杯フィギュアスケート/image/NHK杯連続写真01/RM1_0001_0781_background.JPG";
    QString maskFileName = "/Users/tomiya/Desktop/NHK杯フィギュアスケート/image/RM1_mask.JPG";
    background = cv::imread(backgroundFileName.toStdString().c_str() );
    mask = cv::imread(maskFileName.toStdString().c_str());
    cv::resize(background, lowBackground, cv::Size(lowWidth, lowHeight));
    cv::resize(mask, lowMask, cv::Size(lowWidth, lowHeight));
}

double TrackingWindow::likelihood(int x, int y, cv::Mat image1, cv::Mat image2, cv::Mat image3){
    int width = image1.cols;
    int height = image1.rows;
    if(x >= 0 && x < width && y >= 0 && y < height){
        cv::Vec3b bgr1 = image1.at<cv::Vec3b>(y, x);
        cv::Vec3b bgr2 = image2.at<cv::Vec3b>(y, x);
        if(image3.at<cv::Vec3b>(y, x) == cv::Vec3b(0, 0, 0)) return 0.0;
        float gry1 = (float)(bgr1[0] + bgr1[1] + bgr1[2])/3;
        float gry2 = (float)(bgr2[0] + bgr2[1] + bgr2[2])/3;
        float diff = abs(gry1 - gry2);
        return diff > 0 ? diff : 0.0001;
    }
    else{
        return 0.0001;
    }
}

void TrackingWindow::frameLabelMousePressed(QPoint point){
    int x = point.x() * lowWidth / width;
    int y = point.y() * lowHeight / height;
    for(int i = 0; i < (int)particles.size(); i++){
        particles[i].x = x;
        particles[i].y = y;
    }
    running = false;
    framePos++;
    timeSlider->blockSignals(true);
    timeSlider->setValue(framePos);
    timeSlider->blockSignals(false);
    cap >> frame;
    updateTrajectory();
}

void TrackingWindow::playButtonClicked(){
    running = !running;
}

void TrackingWindow::timeSliderValueChanged(int value){
    framePos = value;
    cap.set(CV_CAP_PROP_POS_FRAMES, value);
    cap >> frame;
    loop();
}

bool TrackingWindow::eventFilter(QObject* obj, QEvent* event){
    if(event->type() == QEvent::KeyPress){
        QKeyEvent* key = static_cast<QKeyEvent*>(event);
        if(key->key() == Qt::Key_Space){
            running = !running;
        }
        else if(key->key() == Qt::Key_Right){
            running = false;
            framePos++;
            timeSlider->blockSignals(true);
            timeSlider->setValue(framePos);
            timeSlider->blockSignals(false);
            cap >> frame;
            updateTrajectory();
        }
        else if(key->key() == Qt::Key_Left){
            running = false;
            framePos = framePos > 0 ? framePos - 1 : 0;
            timeSlider->blockSignals(true);
            timeSlider->setValue(framePos);
            timeSlider->blockSignals(false);
            cap.set(CV_CAP_PROP_POS_FRAMES, framePos);
            cap >> frame;
            if(!trajectory.empty()) trajectory.pop_back();
            qDebug() << "framePos: " + QString(framePos);
        }
        else{
            return QObject::eventFilter(obj, event);
        }
        return true;
    }
    return false;
}

void TrackingWindow::updateTrajectory(){
    double cx, cy;
    pf::measure(particles, cx, cy);
    Target target;
    target.x = cx;
    target.y = cy;

    double normalizedX = (double)cx / lowWidth * 2.0 - 1.0;
    double normalizedY = 1.0 - (double)cy / lowHeight * 2.0;
    double X, Y;
    double Z = 0.5;
    Converter::convert2Dto3D(P, Rt, normalizedX, normalizedY, Z, X, Y);
    target.X = X;
    target.Y = Y;
    target.Z = Z;
    trajectory.push_back(target);
}

void TrackingWindow::drawTrajectory(cv::Mat image, int radius, int len){
    int totalLen = (int)trajectory.size();
    int begin = len < totalLen ? totalLen - len : 0;
    Scalar color;
    for(int i = begin; i <  totalLen; i++){
        int x = trajectory[i].x * pow(2, pyrLevel);
        int y = trajectory[i].y * pow(2, pyrLevel);
        int c = (double)255 * (i - begin) / len;
        color = Scalar(255 - c, c, c);
        circle(image, Point(x, y), radius, color, -1);
    }
}

void TrackingWindow::draw(cv::Mat image){
    for(int i = 0; i < particles.size(); i++){
        int x = particles[i].x * pow(2, pyrLevel);
        int y = particles[i].y * pow(2, pyrLevel);
        circle(image, Point(x, y), 6, Scalar(0, 255, 0), -1);
    }
    drawTrajectory(image, 12, 15);
}

void TrackingWindow::loop(){
    if(running){
        framePos++;
        timeSlider->blockSignals(true);
        timeSlider->setValue(framePos);
        timeSlider->blockSignals(false);
        cap >> frame;
        updateTrajectory();
    }
    if(frame.empty()){
        running = false;
    }
    else{
        cv::resize(frame, lowFrame, cv::Size(lowWidth, lowHeight));
        cv::Mat outFrame = frame.clone();

        // Update particles
        pf::resample(particles);
        pf::predict(particles, 100.0);
        pf::weight(particles, lowFrame, lowBackground, lowMask, likelihood);

        // Render
        draw(outFrame);
        trackingView->image = outFrame.data;
        trackingView->updateGL();
        trajWin->updateGL();
    }
}
