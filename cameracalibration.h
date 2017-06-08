#ifndef CAMERACALIBRATION_H
#define CAMERACALIBRATION_H
#include <opencv2/core/mat.hpp>
//#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/flann/flann.hpp>
#include <vector>

using namespace std;
using namespace cv;

class CameraCalibration
{
public:
    CameraCalibration();
    void calcChessboardCorners(Size boardSize, float squareSize, vector<Point3f> corners);
    void saveparams(const string filename, const Mat cameraMatrix, const Mat distCoeffs, const vector<Mat> rvecs,
                    const vector<Mat> tvecs, const double RMS);
    void calibrar();
};

#endif // CAMERACALIBRATION_H
