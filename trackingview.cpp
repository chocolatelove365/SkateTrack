#include "trackingview.h"

TrackingView::TrackingView(QWidget *parent) : QGLWidget(parent)
{
}

TrackingView::~TrackingView(){
}

void TrackingView::loop(){
    updateGL();
}

qreal TrackingView::windowPixelRatio() {
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    // windowHandle() might not be accessible until the window is displayed to screen.
    return windowHandle() ? windowHandle()->screen()->devicePixelRatio() : 1.0f;
#else
    return 1.0f;
#endif
}

void TrackingView::mousePressEvent(QMouseEvent *event){
    qreal pixelRatio = windowPixelRatio();
    int realX = event->x() * pixelRatio;
    int realY = event->y() * pixelRatio;
    double  normalizedX = (double)(realX - viewX) / viewWidth * 2.0 - 1.0;
    double normalizedY = 1.0 - (double)(realY - viewY) / viewHeight * 2.0;
    double xInImage =  (double)(realX - viewX) / viewWidth * _imageWidth;
    double yInImage = (double)(realY - viewY) / viewHeight * _imageHeight;
    double X, Y;
    Converter::convert2Dto3D(P, Rt, normalizedX, normalizedY, 0.0, X, Y);
    qDebug() << QString::number(xInImage) + ", " + QString::number(yInImage);
    qDebug() << QString::number(normalizedX) + ", " + QString::number(normalizedY);
    qDebug() << "X: " + QString::number(X) + ", Y: " + QString::number(Y);
}

void TrackingView::init(int imageWidth, int imageHeight){
//    QString imageFileName = "/Users/tomiya/Desktop/NHK杯フィギュアスケート/image/image_calibration.JPG";
//    cv::Mat image = cv::imread(imageFileName.toStdString().c_str() );
//    this->image = image.data;
    image = NULL;
    _imageWidth = imageWidth;
    _imageHeight = imageHeight;
    aspect = (double)_imageWidth / _imageHeight;
    fovy = 48.6681031196 / 180 * M_PI;
    far = 100.0;
   near = 1.0;
   double f = 1 / tan(fovy / 2);
   P << f / aspect, 0.0, 0.0, 0.0,
           0.0, f, 0.0, 0.0,
           0.0, 0.0, (far + near) / (near - far), 2 * far * near / (near - far),
           0.0, 0.0, -1.0, 0.0;
   Rt << 0.84080619, -0.54098684, -0.01945062, -0.06907567,
           0.13738744, 0.17849864, 0.97430122, -0.00576485,
           -0.5236122, -0.82187068, 0.22440754, -53.0946064,
           0.0, 0.0, 0.0, 1.0;
}

//void TrackingView::render(uint8_t *image){

//}

void TrackingView::updateTexture(uint8_t *image){
    if(image){
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,  _imageWidth, _imageHeight,
                        GL_BGR_EXT, GL_UNSIGNED_BYTE, image);
        glBindTexture(GL_TEXTURE_2D,0);
    }
}

void TrackingView::initializeGL(){
    glGenTextures(1, textures);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _imageWidth, _imageHeight, 0,
                 GL_BGR_EXT, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D,0);

    glClearColor(0.f, 0.f, 0.f, 1.f);
}

void TrackingView::resizeGL(int w, int h){
    qDebug() << "w: " + QString::number(w) + ", h: " + QString::number(h);
    if(w > h * aspect){
        viewWidth = (int)(h * aspect);
        viewHeight = h;
    }
    else{
        viewWidth = w;
        viewHeight = (int)(w / aspect);
    }
    viewX = (w - viewWidth) / 2;
    viewY = (h - viewHeight) / 2;
}

void TrackingView::paintGL(){
    if(!image) return;
    updateTexture(image);

    glViewport(viewX, viewY, viewWidth, viewHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, _imageWidth, _imageHeight, 0.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor4d(1.0, 1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0);
    glVertex3d(0.0, 0.0, 0.0);
    glTexCoord2d(0.0, 1.0);
    glVertex3d(0.0,_imageHeight, 0.0);
    glTexCoord2d(1.0, 1.0);
    glVertex3d(_imageWidth, _imageHeight, 0.0);
    glTexCoord2d(1.0, 0.0);
    glVertex3d(_imageWidth, 0.0, 0.0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(P.data());
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd(Rt.data());
    drawXYZAxis(20.0);
    drawRect(40, 30);
    drawCircle(0, 0, 5, 64);
    glFlush();
}

void TrackingView::drawRect(double w, double h){
    glColor4d(1.0, 0.0, 0.0, 1.0);
    glBegin(GL_LINE_LOOP);
    glVertex3d(+w/2, +h/2, 0.0);
    glVertex3d(-w/2, +h/2, 0.0);
    glVertex3d(-w/2, -h/2, 0.0);
    glVertex3d(+w/2, -h/2, 0.0);
    glEnd();
}

void TrackingView::drawCircle(double x, double y, double radius, int n){
    glColor4d(1.0, 0.0, 0.0, 1.0);
    glBegin(GL_LINE_LOOP);
    for(int i = 0; i < n; i++){
        double tmpX = radius * cos(2 * M_PI * i / n) + x;
        double tmpY = radius * sin(2 * M_PI * i / n) + y;
        glVertex3d(tmpX, tmpY, 0.0);
    }
    glEnd();
}

void TrackingView::drawXYZAxis(double len){
    glColor4d(1.0, 0.0, 0.0, 1.0);
    glBegin(GL_LINES);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(len, 0.0, 0.0);
    glEnd();
    glColor4d(0.0, 1.0, 0.0, 1.0);
    glBegin(GL_LINES);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, len, 0.0);
    glEnd();
    glColor4d(0.0, 0.0, 1.0, 1.0);
    glBegin(GL_LINES);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, 0.0, len);
    glEnd();
}
