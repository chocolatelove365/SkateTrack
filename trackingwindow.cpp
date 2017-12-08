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
    playButton = new QPushButton();
    timeSlider = new QSlider(Qt::Horizontal);
    timer = new QTimer(this);

    connect(trackingView, SIGNAL(mousePressed(QPoint)), this, SLOT(trackingViewMousePressed(QPoint)));
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

    nParticles = 1000;
    pf::init(particles, nParticles, lowWidth / 2, lowHeight / 2);

    timer->start();
}

TrackingWindow::~TrackingWindow()
{
    delete ui;
}

void TrackingWindow::loadConfig(const QString &fileName){
    QFile loadFile(fileName);
    if (!loadFile.open(QIODevice::ReadOnly))
    {
        qWarning("Couldn't open save.json");
        return;
    }
    QByteArray loadData = loadFile.readAll();
    QJsonObject json(QJsonDocument::fromJson(loadData).object());
    videoFileName =  json["videoFileName"].toString();
    backgroundFileName = json["backgroundFileName"].toString();
    maskFileName = json["maskFileName"].toString();
    pyrLevel =  json["pyrLevel"].toInt();
    QJsonValue value = json.value("Rt");
    Rt << 0.84080619, -0.54098684, -0.01945062, -0.06907567,
            0.13738744, 0.17849864, 0.97430122, -0.00576485,
            -0.5236122, -0.82187068, 0.22440754, -53.0946064,
            0.0, 0.0, 0.0, 1.0;
}

void TrackingWindow::saveCSV(const QString &fileName){
    stringstream ss;
    for(int i = 0; i < trajectory.size(); i++)
    {
        ss << trajectory[i].X << "," << trajectory[i].Y << endl;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::information(this, tr("Unable to open file"),
                                 file.errorString());
        return;
    }
    QTextStream out(&file);
    out << ss.str().c_str();
}

void TrackingWindow::load(){
    loadConfig("/Users/tomiya/Desktop/SkateTrack/config.json");

    cap.open(videoFileName.toStdString().c_str());
    if(!cap.isOpened()){
            qDebug() << "ERROR: failed to open video.\n";
            return;
    }
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

    trackingView->init(width, height, P, Rt);

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

double TrackingWindow::likelihood(int x, int y, cv::Mat image){
    int width = image.cols;
    int height = image.rows;
    if(x >= 0 && x < width && y >= 0 && y < height){
        if(image.at<uchar>(y, x) == 0) return 0.0001;
        else{
            return (double)image.at<uchar>(y, x);
        }
    }
    else{
        return 0.0001;
    }
}

void TrackingWindow::trackingViewMousePressed(QPoint point){
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
//    trajectory.erase(trajectory.begin() + value, trajectory.begin() + trajectory.size()+1);
//    cap >> frame;
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
        }
        else if(key->key() == Qt::Key_S){
            qDebug() << "Save CSV";
            saveCSV("/Users/tomiya/Desktop/SkateTrack/trajectory.csv");
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
    double Z = 0.0;
    Converter::convert2Dto3D(P, Rt, normalizedX, normalizedY, Z, X, Y);
    target.X = X;
    target.Y = Y;
    target.Z = Z;
    trajectory.push_back(target);

#if SMOOTH_TRAJECTORY
    int n = trajectory.size();
    int m = 3;
    if(n >= m){
        double sx, sy, sX, sY, sZ;
        sx = sy = sX = sY = sZ = 0;
        for(int i = 1; i < m+1; i++){
            sx += trajectory[n-i].x;
            sy += trajectory[n-i].y;
            sX += trajectory[n-i].X;
            sY += trajectory[n-i].Y;
            sZ += trajectory[n-i].Z;
        }
        sx /= m; sy /= m; sX /= m; sY /= m; sZ /= m;
        Target target;
        target.x = sx; target.y = sy; target.X = sX; target.Y = sY; target.Z = sZ;
        smoothTrajectory.push_back(target);
    }

#endif
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
    static int count = 0;
    if(running && count >= 2){
        framePos++;
        timeSlider->blockSignals(true);
        timeSlider->setValue(framePos);
        timeSlider->blockSignals(false);
        cap >> frame;
        updateTrajectory();
        count = 0;
    }
    else count++;
    if(frame.empty()){
        running = false;
    }
    else{
        cv::resize(frame, lowFrame, cv::Size(lowWidth, lowHeight));
        cv::Mat outFrame = frame.clone();

        // Update particles
        pf::resample(particles);
#if 0
        double var = 100.0;
#else
        Eigen::Vector4d worldPos, cameraPos;
        worldPos << trajectory.back().X, trajectory.back().Y, trajectory.back().Z, 1.0;
        cameraPos = Rt * worldPos;
//        qDebug() << "cameraPos: X = " + QString::number(cameraPos[0]) + ", Y = " + QString::number(cameraPos[1]) + ", Z = " + QString::number(cameraPos[2]);
        double var = 100.0 / (cameraPos[0]*cameraPos[0]+cameraPos[1]*cameraPos[1]+cameraPos[2]*cameraPos[2]) * 40 * 40;
#endif

#if 1
        cv::Mat lowFrameGray, lowBackgroundGray, lowMaskGray, diff, diffWithMask;
        cv::cvtColor(lowFrame, lowFrameGray, CV_BGR2GRAY);
        cv::cvtColor(lowBackground, lowBackgroundGray, CV_BGR2GRAY);
        cv::cvtColor(lowMask, lowMaskGray, CV_BGR2GRAY);
        cv::absdiff(lowFrameGray, lowBackgroundGray, diff);
        cv::threshold(diff, diff, 0, 255, CV_THRESH_BINARY|CV_THRESH_OTSU);
        diff.copyTo(diffWithMask, lowMaskGray);
        cv::dilate(diffWithMask, diffWithMask, Mat(), Point(-1, -1), 3);
        cv::erode(diffWithMask, diffWithMask, Mat(), Point(-1, -1), 3);

        std::vector< std::vector< cv::Point > > contours, contours_subset;
        cv::findContours(diffWithMask, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
        for(int i = 0; i < contours.size(); i++){
            double area = contourArea(contours.at(i));
            if(area > 150.0){
                contours_subset.push_back(contours.at(i));
            }
        }
        drawContours(diffWithMask,contours_subset,-1,0,-1);

//        double var = 100.0;
        pf::predict(particles, var);
        pf::weight(particles, diffWithMask, likelihood);

        cv::cvtColor(diffWithMask, diffWithMask, CV_GRAY2BGR);
        cv::Mat tmp;
        cv::resize(diffWithMask, tmp, cv::Size(width, height));

        // Render
        draw(outFrame);

        trackingView->image = outFrame.data;
//        trackingView->image = tmp.data;
#else
        pf::predict(particles, var);
        pf::weight(particles, lowFrame, lowBackground, lowMask, likelihood);
        // Render
        draw(outFrame);
        trackingView->image = outFrame.data;
#endif
        trackingView->updateGL();
        trajWin->updateGL();
    }
}
