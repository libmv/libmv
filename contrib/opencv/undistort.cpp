// Remove radial distortion from images.
// Author: keir mierle, 2009.
//
// Use with the YAML calibration file created by "calibration.cpp".

#include "cv.h"
#include "highgui.h"

#include <stdio.h>
#include <vector>
#include <string>

void read_camera_params( const char* in_filename,
                         CvMat** camera_matrix,
                         CvMat** dist_coeffs)
{
    CvFileStorage* fs = cvOpenFileStorage( in_filename, 0, CV_STORAGE_READ );
    *camera_matrix = (CvMat *) cvRead (fs, cvGetFileNodeByName (fs, NULL, "camera_matrix"));
    *dist_coeffs = (CvMat *) cvRead (fs, cvGetFileNodeByName (fs, NULL, "distortion_coefficients"));
    cvReleaseFileStorage (&fs);
}

// so sue me
using namespace std;

int main( int argc, char** argv )
{
    const char* in_filename = 0;
    CvMat *camera_matrix;
    CvMat *dist_coeffs;

    if( argc < 2 )
    {
        printf( "remove radial distortion (undistort) images\n"
            "Usage: undistort\n"
            "     -c <out_camera_params>   # the input filename for intrinsic parameters\n"
            "     image1 image2...         # images to undistort\n"
            "\n"
            "Example:\n"
            "\n"
            "     undistort -c out_camera_data.yml img_1119.jpg img_1120.jpg\n"
            "\n" );
        return 0;
    }

    vector<string> images;
    for( int i = 1; i < argc; i++ )
    {
        const char* s = argv[i];
        if( strcmp( s, "-c" ) == 0 )
        {
            in_filename = argv[++i];
        }
        else if( s[0] != '-' )
        {
          images.push_back(s);
        }
        else
        {
            return fprintf( stderr, "Unknown option %s", s ), -1;
        }
    }

    if(!in_filename)
        return fprintf( stderr, "Must specify -c calib_parameters.yml\n" );

    read_camera_params(in_filename, &camera_matrix, &dist_coeffs);

    for (size_t i = 0; i < images.size(); ++i)
    {
        IplImage *view = cvLoadImage( images[i].c_str(), 1 );

        if( !view )
        {
            fprintf(stderr, "Couldn't read: %s\n", images[i].c_str());
            break;
        }

        IplImage* undistorted = cvCloneImage( view );
        cvUndistort2( view, undistorted, camera_matrix, dist_coeffs );

        fprintf(stderr, "Undistorting: %s -> %s.u.jpg\n", images[i].c_str());

        string new_file = images[i] + ".u.jpg";
        cvSaveImage( new_file.c_str(), undistorted );

        cvReleaseImage( &undistorted );
        cvReleaseImage( &view );
    }
    return 0;
}
