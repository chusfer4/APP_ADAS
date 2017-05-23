#ifndef FINDLINES_H
#define FINDLINES_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

#define PI 3.1415926

class LineFinder {

  private:

      // original image
      Mat img;

      // vector containing the end points
      // of the detected lines
      vector<Vec4i> lines;

      // accumulator resolution parameters
      double deltaRho;
      double deltaTheta;

      // minimum number of votes that a line
      // must receive before being considered
      int minVote;

      // min length for a line
      double minLength;

      // max allowed gap along the line
      double maxGap;

      // distance to shift the drawn lines down when using a ROI
      int shift;

  public:

      // Default accumulator resolution is 1 pixel by 1 degree
      // no gap, no mimimum length
      LineFinder() : deltaRho(1), deltaTheta(PI/180), minVote(10), minLength(0.), maxGap(0.) {}

      // Set the resolution of the accumulator
      void setAccResolution(double dRho, double dTheta)
      {
          deltaRho= dRho;
          deltaTheta= dTheta;
      }

      // Set the minimum number of votes
      void setMinVote(int minv)
      {
          minVote= minv;
      }

      // Set line length and gap
      void setLineLengthAndGap(double length, double gap)
      {
          minLength= length;
          maxGap= gap;
      }

      // set image shift
      void setShift(int imgShift)
      {
          shift = imgShift;
      }

      // Apply probabilistic Hough Transform
      vector<Vec4i> findLines(Mat& binary)
      {
          lines.clear();
          HoughLinesP(binary,lines,deltaRho,deltaTheta,minVote, minLength, maxGap);

          return lines;
      }

      // Draw the detected lines on an image
      void drawDetectedLines(Mat &image, Scalar color=Scalar(255))
      {
          // Draw the lines
          vector<Vec4i>::const_iterator it2= lines.begin();

          while (it2!=lines.end())
          {
              Point pt1((*it2)[0],(*it2)[1]+shift);
              Point pt2((*it2)[2],(*it2)[3]+shift);

              line( image, pt1, pt2, color , 6 );
              cout << " HoughP line: ("<< pt1 <<"," << pt2 << ")\n";
              ++it2;
          }
      }


      // Eliminates lines that do not have an orientation equals to
      // the ones specified in the input matrix of orientations
      // At least the given percentage of pixels on the line must
      // be within plus or minus delta of the corresponding orientation
      vector<Vec4i> removeLinesOfInconsistentOrientations(
          const Mat &orientations, double percentage, double delta) {

              vector<Vec4i>::iterator it= lines.begin();

              // check all lines
              while (it!=lines.end())
              {
                  // end points
                  int x1= (*it)[0];
                  int y1= (*it)[1];
                  int x2= (*it)[2];
                  int y2= (*it)[3];

                  // line orientation + 90o to get the parallel line
                  double ori1= atan2(static_cast<double>(y1-y2),static_cast<double>(x1-x2))+PI/2;
                  if (ori1>PI) ori1= ori1-2*PI;

                  double ori2= atan2(static_cast<double>(y2-y1),static_cast<double>(x2-x1))+PI/2;
                  if (ori2>PI) ori2= ori2-2*PI;

                  // for all points on the line
                  LineIterator lit(orientations, Point(x1,y1), Point(x2,y2));
                  int i,count=0;
                  for(i = 0, count=0; i < lit.count; i++, ++lit)
                  {
                      float ori= *(reinterpret_cast<float *>(*lit));

                      // is line orientation similar to gradient orientation ?
                      if (std::min(fabs(ori-ori1),fabs(ori-ori2))<delta)
                          count++;
                  }

                  double consistency= count/static_cast<double>(i);

                  // set to zero lines of inconsistent orientation
                  if (consistency < percentage)
                  {
                      (*it)[0]=(*it)[1]=(*it)[2]=(*it)[3]=0;
                  }

                  ++it;
              }

              return lines;
      }
};


#endif // FINDLINES_H
