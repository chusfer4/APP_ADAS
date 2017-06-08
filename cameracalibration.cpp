#include "cameracalibration.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/flann/flann.hpp>
#include <vector>

using namespace std;
using namespace cv;

CameraCalibration::CameraCalibration()
{

}

void CameraCalibration::calcChessboardCorners(Size boardSize, float squareSize, vector<Point3f> corners)
{
    corners.clear();
    for (int i = 0; i < boardSize.height; i++)
    {
        for(int j = 0; j < boardSize.width; j++)
        {
            corners.push_back(Point3f(float(j*squareSize), float(i*squareSize), 0));
        }
    }
}

void CameraCalibration::saveparams(const string filename, const Mat cameraMatrix, const Mat distCoeffs, const vector<Mat> rvecs,  const vector<Mat> tvecs, const double RMS)
{
    FileStorage fs( filename, FileStorage::WRITE);
    fs << "Calibrate_accuracy" << RMS;
    fs << "Camera_Matrix" << cameraMatrix;
    fs << "Distorsion_Coefficients" << distCoeffs;
    fs << "Rotation_Vector" << rvecs;
    fs << "Translation_Vector" << tvecs;

    if ( !rvecs.empty() && !tvecs.empty() )
    {
        CV_ASSERT(rvecs[0].type() == tvecs[0].type());
        Mat bigmat((int)rvecs.size(), 6, rvecs[0].type());
        for( int i = 0; i < (int)rvecs.size(); i++)
        {
            Mat r = bigmat(Range(i, i+1), Range(0,3));
            Mat t = bigmat(Range(i, i+1), Range(3,6));

            CV_Assert(rvecs[i].rows == 3 && rvecs[i].cols == 1);
            CV_Assert(tvecs[i].rows == 3 && tvecs[i].cols == 1);

            r = rvecs[i].t();
            t = tvecs[i].t();
        }

        cvWriteComment( *fs, "Rotation vector + Translation vector", 0);
        fs << "extrinsic_parameters" << bigmat;
    }
    fs.release();
}

void CameraCalibration::calibrar()
{
    Size patternsize = Size(9,6); //Esquinas interiores horizontal-vertical
    vector<Point3f> corners3D;
    vector<Point2f> corners2D; //findChessboardCorners guarda los puntos del tablero aqui
    vector<vector<Point2f>> coord2D; // Esquinas detectadas de la imagen
    vector<vector<Point3f>> coord3D; // Ubicacion real de los puntos 3D

    string path = "D:/WORKSPACE/LineDetector_QT_Isa/camera_cal/calibration";
    stringstream imgs;

    calcChessboardCorners(patternsize, 25, corners3D); // 25 = medida del cuadrado

    Mat img, imgGray;
    bool found;

    for (int i = 1; i <= 20; i++) //Numero de imagenes
    {
        imgs << path << i << ".jpg";
        img = imread(imgs.str().c_str());
        cvtColor(img, imgGray, COLOR_BGR2GRAY);
        imgs = stringstream();

        found = findChessboardCorners(imgGray, patternsize, corners2D, CALIB_CB_ADAPTIVE_THRESH +
                                                                        CALIB_CB_NORMALIZE_IMAGE +
                                                                        CALIB_CB_FAST_CHECK);
        if (found)
        {
            cornerSubPix(imgGray, corners2D, Size(11,11), Size(-1,-1), TermCriteria(
                                                                        TermCriteria::EPS +
                                                                        TermCriteria::COUNT, 30, 0.1));
            drawChessboardCorners(img, patternsize, Mat(corners2D), found);
            coord2D.push_back(corners2D);
            coord3D.push_back(corners3D);
        }
        namedWindow("image", WINDOW_AUTOSIZE);
        imshow("image", img);
    }

    cvDestroyWindow("image");

    Mat cameraMatrix = Mat::eye(3, 3, CV_64F); //Matriz 3x3
    Mat distCoeffs = Mat::zeros(8, 1, CV_64F);
    vector<Mat> rvecs;
    vector<Mat> tvecs;
    // Encontrando esos valores de la camara:
    double rms = calibrateCamera(coord3D, coord2D, img.size(), cameraMatrix,
                                 distCoeffs, rvecs, tvecs, CALIB_FIX_PRINCIPAL_POINT +
                                 CALIB_FIX_ASPECT_RATIO + CALIB_ZERO_TANGENT_DIST,
                                 TermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 30, 2.22e-16));

    cout << "RMS: " << rms << endl; // si es cercano a cero significa buena calibracion
    cout << "Camera matrix: " << cameraMatrix << endl;
    cout << "Distortion Coefficients: " << distCoeffs << endl;

    //saveparams("D:/WORKSPACE/LineDetector_QT_Isa/camera_cal/Params.txt", cameraMatrix, distCoeffs, rvecs, tvecs, rms);


}
