
/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2008, Willow Garage Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of Intel Corporation may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

// Modified by Pierre Moulon
// for libmv purpose

#ifndef OPENCV_STAR_DETECTOR_H
#define OPENCV_STAR_DETECTOR_H

#include "libmv/image/image.h"
#include "libmv/base/vector.h"
#include <cmath>

namespace libmv {
namespace detector {

template<class Image, class Image1>
static void             // uchar, int, int , int
icvComputeIntegralImages( const Image & _I, //uchar 1 channel
                          Image1 & _S, // int
                          Image1 & _T, // int
                          Image1 & _FT)// int
{
    int x, y, rows = _I.rows(), cols = _I.cols();
    const unsigned char * I = &_I(0,0);
    int *S = &_S(0,0), *T = &_T(0,0), *FT = &_FT(0,0);
    int istep = _I.cols(), step = _S.cols();

    assert( (rows+1) == _S.rows()     && (cols+1) == _S.cols()
            && _S.rows() == _T.rows() && _T.rows() == _FT.rows()
            && _S.cols() == _T.cols() && _T.cols() == _FT.cols());

    for( x = 0; x <= cols; ++x )
        S[x] = T[x] = FT[x] = 0;

    S += step; T += step; FT += step;
    S[0] = T[0] = 0;
    FT[0] = I[0];
    for( x = 1; x < cols; ++x )
    {
        S[x] = S[x-1] + I[x-1];
        T[x] = I[x-1];
        FT[x] = I[x] + I[x-1];
    }
    S[cols] = S[cols-1] + I[cols-1];
    T[cols] = FT[cols] = I[cols-1];

    for( y = 2; y <= rows; ++y )
    {
        I += istep, S += step, T += step, FT += step;

        S[0] = S[-step]; S[1] = S[-step+1] + I[0];
        T[0] = T[-step + 1];
        T[1] = FT[0] = T[-step + 2] + I[-istep] + I[0];
        FT[1] = FT[-step + 2] + I[-istep] + I[1] + I[0];

        for( x = 2; x < cols; ++x )
        {
            S[x] = S[x - 1] + S[-step + x] - S[-step + x - 1] + I[x - 1];
            T[x] = T[-step + x - 1] + T[-step + x + 1] - T[-step*2 + x] + I[-istep + x - 1] + I[x - 1];
            FT[x] = FT[-step + x - 1] + FT[-step + x + 1] - FT[-step*2 + x] + I[x] + I[x-1];
        }

        S[cols] = S[cols - 1] + S[-step + cols] - S[-step + cols - 1] + I[cols - 1];
        T[cols] = FT[cols] = T[-step + cols - 1] + I[-istep + cols - 1] + I[cols - 1];
    }
}

struct CvStarFeature
{
    int area;
    int* p[8];
};

template<class Image, class Image1, class Image2>
static int
icvStarDetectorComputeResponses( const Image & img, // uchar image type
                                  Image1 * responses, // float image type
                                  Image2* sizes, // short image type
                                 int maxSize ) //maximum scale explored
{
    const int MAX_PATTERN = 17;
    static const int sizes0[] = {1, 2, 3, 4, 6, 8, 11, 12, 16, 22, 23, 32, 45, 46, 64, 90, 128, -1};
    static const int pairs[][2] = {{1, 0}, {3, 1}, {4, 2}, {5, 3}, {7, 4}, {8, 5}, {9, 6},
                                   {11, 8}, {13, 10}, {14, 11}, {15, 12}, {16, 14}, {-1, -1}};
    float invSizes[MAX_PATTERN][2];
    int sizes1[MAX_PATTERN];

    CvStarFeature f[MAX_PATTERN];

    int y=0, i=0, rows = img.Height(), cols = img.Width();
    int border, npatterns=0, maxIdx=0;

    for(; pairs[i][0] >= 0; ++i ) {
        if( sizes0[pairs[i][0]] >= maxSize )
            break;
    }
    npatterns = i;
    npatterns += (pairs[npatterns-1][0] >= 0);
    maxIdx = pairs[npatterns-1][0];

    IntImage sum( rows + 1, cols + 1);
    IntImage tilted( rows + 1, cols + 1);
    IntImage flatTilted( rows + 1, cols + 1);
    int step = sum.Width();

    icvComputeIntegralImages( img, sum, tilted, flatTilted );

    for( i = 0; i <= maxIdx; ++i )  {
        int ur_size = sizes0[i], t_size = sizes0[i] + sizes0[i]/2;
        int ur_area = (2*ur_size + 1)*(2*ur_size + 1);
        int t_area = t_size*t_size + (t_size + 1)*(t_size + 1);

        int * ptrSUM = &sum(0,0);
        // data.i (access matrix as integer
        f[i].p[0] = ptrSUM + (ur_size + 1)*step + ur_size + 1;
        f[i].p[1] = ptrSUM - ur_size*step + ur_size + 1;
        f[i].p[2] = ptrSUM + (ur_size + 1)*step - ur_size;
        f[i].p[3] = ptrSUM - ur_size*step - ur_size;

        int * ptrTILTED = &tilted(0,0);
        int * ptrFLATTILTED = &flatTilted(0,0);
        f[i].p[4] = ptrTILTED + (t_size + 1)*step + 1;
        f[i].p[5] = ptrFLATTILTED - t_size;
        f[i].p[6] = ptrFLATTILTED + t_size + 1;
        f[i].p[7] = ptrTILTED - t_size*step + 1;

        f[i].area = ur_area + t_area;
        sizes1[i] = sizes0[i];
    }
    // negate end points of the size range
    // for a faster rejection of very small or very large features in non-maxima suppression.
    sizes1[0] = -sizes1[0];
    sizes1[1] = -sizes1[1];
    sizes1[maxIdx] = -sizes1[maxIdx];
    border = sizes0[maxIdx] + sizes0[maxIdx]/2;

    for( i = 0; i < npatterns; ++i )  {
        int innerArea = f[pairs[i][1]].area;
        int outerArea = f[pairs[i][0]].area - innerArea;
        invSizes[i][0] = 1.f/outerArea;
        invSizes[i][1] = 1.f/innerArea;
    }

    for( y = 0; y < border; ++y ) {
      memset( &((*responses)(y,0)), 0, cols * sizeof(float));
      memset( &((*responses)(rows - 1 - y, 0)), 0, cols * sizeof(float));

      memset( &((*sizes)(y,0)), 0, cols * sizeof(short));
      memset( &((*sizes)(rows - 1 - y, 0)), 0, cols * sizeof(short));
    }

    for( y = border; y < rows - border; ++y ) {
        int x = border;
        float* r_ptr = (float*)(&(*responses)(0,0) + responses->Width()*y);
        short* s_ptr = (short*)(&(*sizes)(0,0) + sizes->Width()*y);

        memset( r_ptr, 0, border * sizeof(float));
        memset( s_ptr, 0, cols * sizeof(short));

        memset( &r_ptr[cols - 1 - border], 0, border * sizeof(float));
        memset( &s_ptr[cols - 1 - border], 0, border * sizeof(short));

        for( ; x < cols - border; ++x ) {
            int ofs = y*step + x;
            int vals[MAX_PATTERN];
            float bestResponse = 0.0f;
            int bestSize = 0;

            for(int i = 0; i <= maxIdx; ++i ) {
                const int** p = (const int**)&f[i].p[0];
                vals[i] = p[0][ofs] - p[1][ofs] - p[2][ofs] + p[3][ofs] +
                    p[4][ofs] - p[5][ofs] - p[6][ofs] + p[7][ofs];
            }
            for(int i = 0; i < npatterns; ++i ) {
                int inner_sum = vals[pairs[i][1]];
                int outer_sum = vals[pairs[i][0]] - inner_sum;
                float response = inner_sum*invSizes[i][1] - outer_sum*invSizes[i][0];
                if( fabs(response) > fabs(bestResponse) ) {
                    bestResponse = response;
                    bestSize = sizes1[pairs[i][0]];
                }
            }

            r_ptr[x] = bestResponse;
            s_ptr[x] = (short)bestSize;
        }
    }

    return border;
}

template<class Image, class Image1>
static bool
icvStarDetectorSuppressLines( const Image & responses, // image in float
                              const Image1& sizes, // image in short data type
                              int ptx, int pty, // analyzed point coordinates
                              // Threshold to reject point
                              int ilineThresholdProjected = 10,
                              int ilineThresholdBinarized = 8)
{
    const float* r_ptr = &responses(0,0);
    const short* s_ptr = &sizes(0,0);
    int rstep = responses.cols();
    int sstep = sizes.cols();

    int sz = s_ptr[pty*sstep + ptx];
    int x, y, delta = sz/4, radius = delta*4;
    float Lxx = 0.0f, Lyy = 0.0f, Lxy = 0.0f;
    int Lxxb = 0, Lyyb = 0, Lxyb = 0;

    for( y = pty - radius; y <= pty + radius; y += delta )  {
        for( x = ptx - radius; x <= ptx + radius; x += delta )  {
            float Lx = r_ptr[y*rstep + x + 1] - r_ptr[y*rstep + x - 1];
            float Ly = r_ptr[(y+1)*rstep + x] - r_ptr[(y-1)*rstep + x];
            Lxx += Lx*Lx;
            Lyy += Ly*Ly;
            Lxy += Lx*Ly;
        }
    }

    if( (Lxx + Lyy)*(Lxx + Lyy) >= ilineThresholdProjected*(Lxx*Lyy - Lxy*Lxy) )
        return true;

    for( y = pty - radius; y <= pty + radius; y += delta )  {
        for( x = ptx - radius; x <= ptx + radius; x += delta )  {
            int Lxb = (s_ptr[y*sstep + x + 1] == sz) - (s_ptr[y*sstep + x - 1] == sz);
            int Lyb = (s_ptr[(y+1)*sstep + x] == sz) - (s_ptr[(y-1)*sstep + x] == sz);
            Lxxb += Lxb * Lxb;
            Lyyb += Lyb * Lyb;
            Lxyb += Lxb * Lyb;
        }
    }

    if( (Lxxb + Lyyb)*(Lxxb + Lyyb) >= ilineThresholdBinarized*(Lxxb*Lyyb - Lxyb*Lxyb) )
        return true;

    return false;
}

struct CvPoint
{
  int x,y;
  CvPoint(int _x,int _y)  {
    x= _x; y = _y;
  }
};

template<class Image, class Image1>
static void
icvStarDetectorSuppressNonmax( const Image& responses, // image in float
                               const Image1& sizes,    // image in short
                               vector<Feature *> *keypoints, // detected feature
                               int iBorder, // max number of scale expored
                               int iSuppressNonMaxSize = 5, // x*y*z non max suppression
                               float fResponseThreshold = 30.0f, // blob response filtering
                               // threshold factors
                               int ilineThresholdProjected = 10,
                               int ilineThresholdBinarized = 8)
{
    int x, y, x1, y1, delta = iSuppressNonMaxSize/2;
    int rows = responses.rows(), cols = responses.cols();
    const float* r_ptr = &responses(0,0);
    const short* s_ptr = &sizes(0,0);
    int rstep = responses.cols();
    int sstep = sizes.cols();
    short featureSize = 0;

    for( y = iBorder; y < rows - iBorder; y += delta+1 )  {
        for( x = iBorder; x < cols - iBorder; x += delta+1 )
        {
            float maxResponse = fResponseThreshold;
            float minResponse = (float)-fResponseThreshold;
            CvPoint maxPt(-1,-1), minPt(-1,-1);
            int tileEndY = std::min(y + delta, rows - iBorder - 1);
            int tileEndX = std::min(x + delta, cols - iBorder - 1);

            for( y1 = y; y1 <= tileEndY; y1++ ) {
                for( x1 = x; x1 <= tileEndX; x1++ ) {
                    float val = r_ptr[y1*rstep + x1];
                    if( maxResponse < val ) {
                        maxResponse = val;
                        maxPt = CvPoint(x1, y1);
                    }
                    else if( minResponse > val )  {
                        minResponse = val;
                        minPt = CvPoint(x1, y1);
                    }
                }
            }

            if( maxPt.x >= 0 )  {
                for( y1 = maxPt.y - delta; y1 <= maxPt.y + delta; y1++ )  {
                    for( x1 = maxPt.x - delta; x1 <= maxPt.x + delta; x1++ )  {
                        float val = r_ptr[y1*rstep + x1];
                        if( val >= maxResponse && (y1 != maxPt.y || x1 != maxPt.x))
                            goto skip_max;
                    }
                }

                if( (featureSize = s_ptr[maxPt.y*sstep + maxPt.x]) >= 4 &&
                    !icvStarDetectorSuppressLines( responses, sizes, maxPt.x, maxPt.y, ilineThresholdProjected, ilineThresholdBinarized ))
                {
                  PointFeature *f = new PointFeature(maxPt.x, maxPt.y);
                  f->scale = featureSize;
                  f->orientation = 0.0;
                  keypoints->push_back(f);
                }
            }
        skip_max:
            if( minPt.x >= 0 )  {
                for( y1 = minPt.y - delta; y1 <= minPt.y + delta; y1++ )  {
                    for( x1 = minPt.x - delta; x1 <= minPt.x + delta; x1++ )  {
                        float val = r_ptr[y1*rstep + x1];
                        if( val <= minResponse && (y1 != minPt.y || x1 != minPt.x))
                            goto skip_min;
                    }
                }

                if( (featureSize = s_ptr[minPt.y*sstep + minPt.x]) >= 4 &&
                    !icvStarDetectorSuppressLines( responses, sizes, minPt.x, minPt.y, ilineThresholdProjected, ilineThresholdBinarized ))
                {
                    PointFeature *f = new PointFeature(minPt.x, minPt.y);
                    f->scale = featureSize;
                    f->orientation = 0.0;
                    keypoints->push_back(f);
                }
            }
        skip_min:
            ;
        }
    }
}

} //namespace detector
} //namespace libmv

#endif // #define OPENCV_STAR_DETECTOR_H

