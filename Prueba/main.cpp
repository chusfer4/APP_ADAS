#include <iostream>
#include "opencv2/opencv.hpp"

//using namespace cv;

int main(int, char**)
{
cv::VideoCapture cap(0); // open the default camera
//Video Capture cap(path_to_video); // open the video file
if(!cap.isOpened())  // check if we succeeded
    return -1;

cv::namedWindow("Video",1);
for(;;)
{
    cv::Mat frame;
    cap >> frame; // get a new frame from camera
    imshow("Video", frame);
    if(cv::waitKey(30) >= 0) break;
}
// the camera will be deinitialized automatically in VideoCapture destructor
return 0;
}
