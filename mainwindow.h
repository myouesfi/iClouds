#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QSettings>
#include<QDesktopWidget>
#include<QRect>

#include<fstream>
#include <math.h>
#include <iostream>
#include<string>
#include<vector>

#include "cv.h"
#include "highgui.h"
#include "munkres.h"
#include "CThinPlateSpline.h"

using namespace std;
using namespace cv;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void openCloud_ButtonClick();
    void closeCloud_ButtonClick();
    void canny_ButtonClick();
    void grayImg_ButtonClick();
    void equalizeGrayImg_ButtonClick();
    void erodeCannyImg_ButtonClick();
    void dilateCannyImg_ButtonClick();
    void backgroundSubstraction_ButtonClick();
    void blueOutImg_ButtonClick();
    void logPolarImg_ButtonClick();
    void findShapeContext_ButtonClick();
    void samplingPointImg_ButtonClick();
    void samplingPointImg2_ButtonClick();
    void findSimilarImage_ButtonClick();

    CvMat* findShapeContext(QList<CvPoint2D32f> points,int nbins_r,int nbins_theta,string shapeName);

    CvMat* findCostBetween2Shapes(CvMat* shapeContext1,CvMat* shapeContext2);

    CvMat* findMatchingPoints(CvMat* costMatrix);
    CvMat* findReturnMatchingPoints(CvMat* costMatrix,
                                  QList<CvPoint2D32f> shape1,
                                  QList<CvPoint2D32f> shape2,
                                  QList<CvPoint2D32f> &matchPoints1,
                                  QList<CvPoint2D32f> &matchPoints2);

    double findShapeDistance(CvMat* costMatrix);

    void exitProgram_ButtonClick();

private:
    Ui::MainWindow *ui;

    /** Image Variable **/
    IplImage* _orgCloud;
    IplImage* _cannyImg;
    int _imgWidth,_imgHeight;

    IplImage* _logPolarImg;

    IplImage* _sampling1Img;
    IplImage* _sampling2Img;
    QList<CvPoint2D32f> _sampPoints1;
    QList<CvPoint2D32f> _sampPoints2;

    QRect _DesktopSize;

};

#endif // MAINWINDOW_H
