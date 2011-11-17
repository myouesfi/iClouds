#include "mainwindow.h"
#include "ui_mainwindow.h"


#define PI 3.141592653589

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //QSettings settings("ParNurZeal","iClouds");
    move(QPoint(600,500));
    _orgCloud=NULL;
    _sampling1Img=NULL;
    _sampling2Img=NULL;
    QDesktopWidget *desktop = QApplication::desktop();
    _DesktopSize = desktop->screenGeometry(desktop->primaryScreen());

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::openCloud_ButtonClick(){

    QString fileName = QFileDialog::getOpenFileName(
                this,
                tr("Open Cloud Image"),
                "C:\\Users\\theeraphol\\Pictures\\Cloud Pics",
                tr("Image Files (*.png *.jpg *.bmp)"));
    if(fileName==NULL) return;
    _orgCloud= cvLoadImage(fileName.toStdString().c_str());
    cvNamedWindow("Original Cloud",CV_WINDOW_NORMAL);
    if(_orgCloud->width<1000&&_orgCloud->height<1000){
        _imgWidth=_orgCloud->width;_imgHeight=_orgCloud->height;
    }else{
        _imgWidth=_orgCloud->width/4;_imgHeight=_orgCloud->height/4;
    }
    cvResizeWindow("Original Cloud",_imgWidth,_imgHeight);
    cvMoveWindow("Original Cloud",0,0);
    cvShowImage("Original Cloud",_orgCloud);


    return;
}

void MainWindow::grayImg_ButtonClick(){
    if(_orgCloud==NULL)return;
    _cannyImg = cvCreateImage(cvGetSize(_orgCloud),IPL_DEPTH_8U,1);
    cvCvtColor(_orgCloud,_cannyImg,CV_RGB2GRAY);
    cvNamedWindow("Canny Edge Detector",CV_WINDOW_NORMAL);
    cvResizeWindow("Canny Edge Detector",_imgWidth,_imgHeight);
    cvShowImage("Canny Edge Detector",_cannyImg);
    cvMoveWindow("Canny Edge Detector",_imgWidth+20,0);
    return;
}

void MainWindow::equalizeGrayImg_ButtonClick(){
    cvEqualizeHist(_cannyImg,_cannyImg);
    cvShowImage("Canny Edge Detector",_cannyImg);
}

void MainWindow::erodeCannyImg_ButtonClick(){
    cvErode(_cannyImg,_cannyImg);
    cvShowImage("Canny Edge Detector",_cannyImg);
}
void MainWindow::dilateCannyImg_ButtonClick(){
    cvDilate(_cannyImg,_cannyImg);
    cvShowImage("Canny Edge Detector",_cannyImg);
}
void MainWindow::canny_ButtonClick(){
    cvCanny(_cannyImg,_cannyImg,1,255,3);
    cvShowImage("Canny Edge Detector",_cannyImg);
}
void MainWindow::logPolarImg_ButtonClick(){
    if(_orgCloud==NULL)return;
    _logPolarImg = cvCreateImage(cvGetSize(_orgCloud),IPL_DEPTH_8U,3);
    cvLogPolar(_orgCloud,_logPolarImg,
               cvPoint2D32f(_orgCloud->width/2,_orgCloud->height/2),
               40,
               CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS);
    cvNamedWindow("Log Polar",CV_WINDOW_NORMAL);
    cvResizeWindow("Log Polar",_imgWidth,_imgHeight);
    cvShowImage("Log Polar",_logPolarImg);
    cvMoveWindow("Log Polar",_imgWidth+10,0);

}

void MainWindow::backgroundSubstraction_ButtonClick(){
}
void MainWindow::blueOutImg_ButtonClick(){
}

void MainWindow::samplingPointImg_ButtonClick(){
    if(_cannyImg==NULL) return;
    QList<CvPoint2D32f> sampPoints;
    for(int y=0;y<_cannyImg->height;y++){
        uchar* ptr=(uchar*)(_cannyImg->imageData+y*_cannyImg->widthStep);
        for(int x=0;x<_cannyImg->width;x++){
            if(ptr[x]==255){
                sampPoints.append(cvPoint2D32f(x,y));
            }
        }
    }
    _sampling1Img= cvCreateImage(cvGetSize(_orgCloud),IPL_DEPTH_8U,1);
    cvZero(_sampling1Img);
    _sampPoints1.clear();
    long count =0;
    for(int i=0;i<sampPoints.size();i++){
        if(count++%5==0){
            uchar* ptr = (uchar*)(_sampling1Img->imageData+((int)sampPoints[i].y)*_sampling1Img->widthStep);
            ptr[(int)sampPoints[i].x]=255;
            _sampPoints1.append(cvPoint2D32f(sampPoints[i].x,sampPoints[i].y));
        }
    }
    cvNamedWindow("Sampling Image 1",CV_WINDOW_NORMAL);
    cvResizeWindow("Sampling Image 1",_imgWidth,_imgHeight);
    cvShowImage("Sampling Image 1",_sampling1Img);
    cvMoveWindow("Sampling Image 1",_DesktopSize.width()-(_imgWidth+20)*2,0);
}
void MainWindow::samplingPointImg2_ButtonClick(){
    if(_cannyImg==NULL) return;
    QList<CvPoint2D32f> sampPoints;
    for(int y=0;y<_cannyImg->height;y++){
        uchar* ptr=(uchar*)(_cannyImg->imageData+y*_cannyImg->widthStep);
        for(int x=0;x<_cannyImg->width;x++){
            if(ptr[x]==255){
                sampPoints.append(cvPoint2D32f(x,y));
            }
        }
    }
    _sampling2Img= cvCreateImage(cvGetSize(_orgCloud),IPL_DEPTH_8U,1);
    cvZero(_sampling2Img);
    _sampPoints2.clear();
    long count =0;
    for(int i=0;i<sampPoints.size();i++){
        if(count++%5==0){
            uchar* ptr = (uchar*)(_sampling2Img->imageData+((int)sampPoints[i].y)*_sampling2Img->widthStep);
            ptr[(int)sampPoints[i].x]=255;
            _sampPoints2.append(cvPoint2D32f(sampPoints[i].x,sampPoints[i].y));
        }
    }
    cvNamedWindow("Sampling Image 2",CV_WINDOW_NORMAL);
    cvResizeWindow("Sampling Image 2",_imgWidth,_imgHeight);
    cvShowImage("Sampling Image 2",_sampling2Img);
    cvMoveWindow("Sampling Image 2",_DesktopSize.width()-_imgWidth-20,0);
}


double dist2(CvPoint2D32f p1,CvPoint2D32f p2){
    return sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
}

double angle2(CvPoint2D32f p1,CvPoint2D32f p2){
    double angle = atan2((p2.y-p1.y),(p2.x-p1.x));
    if(angle<0) angle += 2*PI;
    return angle;
}

void printMatrixToFile(CvMat *matrix,string filename){
    FILE *file;
    file = fopen(filename.c_str(),"w");
    for(int row=0;row<matrix->height;row++){
        for(int col=0;col<matrix->width;col++){
            fprintf(file,"%7.4f ",cvGetReal2D(matrix,row,col));
        }
        fprintf(file,"\n");
    }
    fclose(file);
}
CvMat* MainWindow::findShapeContext(QList<CvPoint2D32f> points, int nbins_r, int nbins_theta,string shapeName){
    /** create distance matrix **/
    CvMat *matrix = cvCreateMat(points.size(),points.size(),CV_32F);
    for(int row=0;row<points.size();row++){
        for(int col=0;col<points.size();col++){
            cvSetReal2D(matrix,row,col,dist2(points[row],points[col]));
        }
    }
    printMatrixToFile(matrix,shapeName+"_1_distMatrix.txt");
    /** normalize by mean **/
    CvScalar mean = cvAvg(matrix);
    for(int row=0;row<points.size();row++){
        for(int col=0;col<points.size();col++){
            double value = cvGetReal2D(matrix,row,col);
            cvSetReal2D(matrix,row,col,value/mean.val[0]);
        }
    }
    printMatrixToFile(matrix,shapeName+"_2_normDistMatrix.txt");

    /** create dist histogram **/
    CvMat *distHist = cvCreateMat(points.size(),points.size(),CV_8U);
    cvZero(distHist);
    // create log step;
    QList<double> logScale;
    logScale.append(0.1250);logScale.append(0.250);logScale.append(0.5);logScale.append(1);logScale.append(2);
    for(int i=0;i<logScale.size();i++){
        for(int row=0;row<points.size();row++){
            for(int col=0;col<points.size();col++){
                double value = cvGetReal2D(matrix,row,col);
                if(value<logScale[i]){
                    double histVal = cvGetReal2D(distHist,row,col);
                    cvSetReal2D(distHist,row,col,++histVal);
                }
            }
        }
    }
    printMatrixToFile(distHist,shapeName+"_3_distHistMatrix.txt");

    /** create angle matrix **/
    CvMat *angMatrix = cvCreateMat(points.size(),points.size(),CV_32F);
    for(int row=0;row<points.size();row++){
        for(int col=0;col<points.size();col++){
            cvSetReal2D(angMatrix,row,col,angle2(points[row],points[col]));
        }
    }
    printMatrixToFile(angMatrix,shapeName+"_4_angleMatrix.txt");
    /** quantized angle matrix**/
    for(int row=0;row<points.size();row++){
        for(int col=0;col<points.size();col++){
            double value = cvGetReal2D(angMatrix,row,col);
            value = 1+floor(value/(2*PI/nbins_theta));
            cvSetReal2D(angMatrix,row,col,value);
        }
    }
    printMatrixToFile(angMatrix,shapeName+"_5_quantizedAngMatrix.txt");

    /** combine 2 histogram into BinHistogram **/
    CvMat *flagDistMatrix = cvCreateMat(points.size(),points.size(),CV_8U);
    cvZero(flagDistMatrix);
    for(int row=0;row<points.size();row++){
        for(int col=0;col<points.size();col++){
            double value = cvGetReal2D(matrix,row,col);
            if(value>0)
                cvSetReal2D(flagDistMatrix,row,col,1);
        }
    }

    int nbins=nbins_r*nbins_theta;
    CvMat *binHistMatrix = cvCreateMat(points.size(),nbins,CV_8U);
    cvZero(binHistMatrix);
    for(int row=0;row<points.size();row++){
        for(int col=0;col<points.size();col++){
            int distH = cvGetReal2D(distHist,row,col);
            int angH = cvGetReal2D(angMatrix,row,col);
            if(distH==0||angH==0) continue;
            int oldVal = cvGetReal2D(binHistMatrix,row,(angH-1)*nbins_r+(distH-1));
            cvSetReal2D(binHistMatrix,row,(angH-1)*nbins_r+(distH-1),++oldVal);
        }
    }
    printMatrixToFile(binHistMatrix,shapeName+"_6_BinHistogram.txt");
    return binHistMatrix;
}

CvMat* MainWindow::findCostBetween2Shapes(CvMat *shapeContext1, CvMat *shapeContext2){
    /** cal Cost Matrix **/
    CvMat *costMatrix = cvCreateMat(shapeContext1->rows,shapeContext2->rows,CV_32F);
    cvZero(costMatrix);
    for(int row=0;row<costMatrix->rows;row++){
        for(int col=0;col<costMatrix->cols;col++){
            double cost=0;
            for(int index=0;index<shapeContext1->cols;index++){
                int pHist=cvGetReal2D(shapeContext1,row,index);
                int qHist=cvGetReal2D(shapeContext2,col,index);
                if(pHist+qHist==0)continue;
                double newVal=pow((double)(pHist-qHist),2);
                newVal/=(pHist+qHist);
                cost+=newVal;
            }
            cost=0.5*cost;
            cvSetReal2D(costMatrix,row,col,cost);
        }
    }
    return costMatrix;
}

CvMat* MainWindow::findMatchingPoints(CvMat *costMatrix){
    Matrix<double> hungMatrix(costMatrix->rows,costMatrix->cols);
    //initialze matrix with values;
    for(int row=0;row<costMatrix->rows;row++){
        for(int col=0;col<costMatrix->cols;col++){
            hungMatrix(row,col)=cvGetReal2D(costMatrix,row,col);
        }
    }
    Munkres hungAlgo;
    hungAlgo.solve(hungMatrix);
    CvMat *resultHungMatrix=cvCreateMat(hungMatrix.rows(),hungMatrix.columns(),CV_32F);
    cvZero(resultHungMatrix);
    for(int row=0;row<hungMatrix.rows();row++){
        for(int col=0;col<hungMatrix.columns();col++){
            cvSetReal2D(resultHungMatrix,row,col,hungMatrix(row,col));
        }
    }
    return resultHungMatrix;
}
CvMat* MainWindow::findReturnMatchingPoints(CvMat *costMatrix,
                                          QList<CvPoint2D32f> shape1,
                                          QList<CvPoint2D32f> shape2,
                                          QList<CvPoint2D32f> &matchPoints1,
                                          QList<CvPoint2D32f> &matchPoints2){
    CvMat *resultHungMatrix=findMatchingPoints(costMatrix);
    // extract all matched pair points
    for(int row=0;row<resultHungMatrix->rows;row++){
        for(int col=0;col<resultHungMatrix->cols;col++){
            double value = cvGetReal2D(resultHungMatrix,row,col);
            if(value==0){
                matchPoints1.append(cvPoint2D32f(shape1.at(row).x,shape1.at(row).y));
                matchPoints2.append(cvPoint2D32f(shape2.at(col).x,shape2.at(col).y));
            }
        }
    }
    return resultHungMatrix;
}

double MainWindow::findShapeDistance(CvMat *costMatrix){
    // find shape distance from cost matrix
    double Dsc_term1=0;
    for(int row=0;row<costMatrix->rows;row++){
        double min=INFINITE;
        for(int col=0;col<costMatrix->cols;col++){
            double value = cvGetReal2D(costMatrix,row,col);
            if(value<min){
                min=value;
            }
        }
        Dsc_term1+=min;
    }
    Dsc_term1=(1.0/costMatrix->rows)*Dsc_term1;
    double Dsc_term2=0;
    for(int col=0;col<costMatrix->cols;col++){
        double min=INFINITE;
        for(int row=0;row<costMatrix->rows;row++){
            double value = cvGetReal2D(costMatrix,row,col);
            if(value<min){
                min=value;
            }
        }
        Dsc_term2+=min;
    }
    Dsc_term2=(1.0/costMatrix->cols)*Dsc_term2;
    return Dsc_term1+Dsc_term2;
}

void MainWindow::findShapeContext_ButtonClick(){
    QList<CvPoint2D32f> shape1,shape2;
    /*shape1.append(cvPoint2D32f(20,50));shape1.append(cvPoint2D32f(40,50));
    shape1.append(cvPoint2D32f(30,40));shape1.append(cvPoint2D32f(15,30));
    shape1.append(cvPoint2D32f(30,20));shape1.append(cvPoint2D32f(45,30));*/
    /*shape2.append(cvPoint2D32f(30,60));shape2.append(cvPoint2D32f(50,60));
    shape2.append(cvPoint2D32f(40,50));shape2.append(cvPoint2D32f(25,40));
    shape2.append(cvPoint2D32f(40,30));//shape2.append(cvPoint2D32f(0.45,0.3));*/
    /** assign shape point lists from some points in 2 canny images **/
    shape1 =_sampPoints1;
    shape2 =_sampPoints2;

    // before transform
    Mat prototypeImg(cvGetSize(_orgCloud),CV_8UC1,Scalar(0));
    Mat dst(cvGetSize(_orgCloud),CV_8UC1,Scalar(0));
    for(int i=0;i<shape1.size();i++){
        prototypeImg.at<unsigned char>(shape1[i].y,shape1[i].x)=255;
    }
    cv::namedWindow("Before Transform",CV_WINDOW_NORMAL);
    cvResizeWindow("Before Transform",_imgWidth,_imgHeight);
    cvMoveWindow("Before Transform",0,_imgHeight+40);
    cv::imshow("Before Transform",prototypeImg);

    int nbins_r=5;
    int nbins_theta=6;
    /** cal shape context of each shape **/
    CvMat *binHistMatrix = findShapeContext(shape1,nbins_r,nbins_theta,"1");
    CvMat *binHistMatrix2= findShapeContext(shape2,nbins_r,nbins_theta,"2");

    /** cal Cost Matrix **/
    CvMat *costMatrix = findCostBetween2Shapes(binHistMatrix,binHistMatrix2);
    printMatrixToFile(costMatrix,"costMatrix.txt");
    qDebug()<<"Shape Distant::"<<findShapeDistance(costMatrix);
    if(cvWaitKey(0)=='q'){
        qDebug()<<"Quit & Not Tranform";
        return;
    }

    /** matching points of 2 shapes using Hungarian Algo.**/
    QList<CvPoint2D32f> matchPoints1,matchPoints2;
    CvMat *resultHungMatrix=findReturnMatchingPoints(costMatrix,shape1,shape2,matchPoints1,matchPoints2);
    printMatrixToFile(resultHungMatrix,"hungarianMatrix.txt");

    /** show the correspondences between the untransformed images **/
    IplImage *untranImg = cvCreateImage(cvGetSize(_orgCloud),IPL_DEPTH_8U,1);
    for(int i=0;i<matchPoints1.size();i++){
        cvLine(untranImg,
               cvPoint(matchPoints1[i].x,matchPoints1[i].y),
               cvPoint(matchPoints2[i].x,matchPoints2[i].y),cvScalar(0),2);
    }
    cvNamedWindow("Untranformed Correspondences",CV_WINDOW_NORMAL);
    cvResizeWindow("Untranformed Correspondences",_imgWidth,_imgHeight);
    cvMoveWindow("Untranformed Correspondences",_imgWidth+20,_imgHeight+40);
    cvShowImage("Untranformed Correspondences",untranImg);

    /** Regularized TPS **/
    // create cv::Point
    vector<Point> iP,iiP;
    for(int i=0;i<matchPoints1.size();i++){
        iP.push_back(Point(matchPoints1[i].x,matchPoints1[i].y));
    }
    for(int i=0;i<matchPoints2.size();i++){
        iiP.push_back(Point(matchPoints2[i].x ,matchPoints2[i].y ));
    }
    CThinPlateSpline tps(iP,iiP);
    tps.warpImage(prototypeImg,dst,0.001,INTER_CUBIC,BACK_WARP);

    cv::namedWindow("After Transform",CV_WINDOW_NORMAL);
    cvResizeWindow("After Transform",_imgWidth,_imgHeight);
    cvMoveWindow("After Transform",(_imgWidth+20)*2,_imgHeight+40);
    cv::imshow("After Transform",dst);
    // find new points set.
    QList<CvPoint2D32f> transformShape;
    for(int row=0;row<dst.rows;row++){
        for(int col=0;col<dst.cols;col++){
            if(dst.at<unsigned char>(row,col)==255){
                transformShape.append(cvPoint2D32f(col,row));
            }
        }
    }

    /** Compute the shape distance **/
    CvMat *binHistTransformShape= findShapeContext(transformShape,nbins_r,nbins_theta,"3");
    CvMat *lastCostMatrix = findCostBetween2Shapes(binHistTransformShape,binHistMatrix2);
    printMatrixToFile(lastCostMatrix,"lastCostMatrix.txt");
    // find shape distance from cost matrix
    double Dsc = findShapeDistance(lastCostMatrix);
    qDebug()<<"Shape Distance [Tranformed]:"<<Dsc;

}

void MainWindow::findSimilarImage_ButtonClick(){
    /** find sampling points for the cloud **/
    if(_cannyImg==NULL) return;
    QList<CvPoint2D32f> myCloud;
    myCloud=_sampPoints1;
    /** find the best match prototype **/
    string bestMatchFileName;
    double bestShapeDistance = INFINITE;
    string folderName="C:\\Users\\theeraphol\\Pictures\\Cloud Pics\\Database\\";
    for(int i=0;i<9;i++){
        char number[10];
        itoa(i+1,number,10);
        string fileName = folderName+"prototype ("+number+").jpg";
        /** load prototype image & sampling point from prototype **/
        IplImage* prototypeImg = cvLoadImage(fileName.c_str());
        IplImage* prototypeGrayImg= cvCreateImage(cvGetSize(prototypeImg),IPL_DEPTH_8U,1);
        cvCvtColor(prototypeImg,prototypeGrayImg,CV_RGB2GRAY);
        cvCanny(prototypeGrayImg,prototypeGrayImg,1,255,3);
        QList<CvPoint2D32f> prototype;
        QList<CvPoint2D32f> sampPoints;
        for(int y=0;y<prototypeGrayImg->height;y++){
            uchar* ptr=(uchar*)(prototypeGrayImg->imageData+y*prototypeGrayImg->widthStep);
            for(int x=0;x<prototypeGrayImg->width;x++){
                if(ptr[x]==255){
                    sampPoints.append(cvPoint2D32f(x,y));
                }
            }
        }
        prototype.clear();
        long count =0;
        for(int i=0;i<sampPoints.size();i++){
            if(count++%5==0){
                prototype.append(cvPoint2D32f(sampPoints[i].x,sampPoints[i].y));
            }
        }

        int nbins_r=5;
        int nbins_theta=6;
        /** cal shape context of each shape **/
        CvMat *binHistMatrix = findShapeContext(myCloud,nbins_r,nbins_theta,"myCloud");
        CvMat *binHistMatrix2= findShapeContext(prototype,nbins_r,nbins_theta,"prototype");
        /** cal Cost Matrix **/
        CvMat *costMatrix = findCostBetween2Shapes(binHistMatrix,binHistMatrix2);
        printMatrixToFile(costMatrix,"costMatrix.txt");
        double newShapeDist = findShapeDistance(costMatrix);
        qDebug()<<"Shape Distant with "<<i+1<<"::"<<newShapeDist;
        if(newShapeDist<bestShapeDistance){
            bestShapeDistance=newShapeDist;
            bestMatchFileName=fileName;
        }
    }
    IplImage* bestMatchImg = cvLoadImage(bestMatchFileName.c_str());
    cvNamedWindow("Best Match");
    cvShowImage("Best Match",bestMatchImg);
    qDebug()<<"[END PROCESS]";
}

void MainWindow::closeCloud_ButtonClick(){
    if(cvGetWindowHandle("Original Cloud")==NULL)
        return;
    cvReleaseImage(&_orgCloud);
    cvDestroyWindow("Original Cloud");
    cvDestroyWindow("Canny Edge Detector");
    cvDestroyWindow("Log Polar");
}

void MainWindow::exitProgram_ButtonClick(){
    exit(1);
}
