

// Modified by Pierre Moulon
// for libmv purpose

#ifndef OPENCV_MSER_DETECTOR_H
#define OPENCV_MSER_DETECTOR_H

#include "libmv/base/vector.h"
#include "libmv/image/image.h"
#include "libmv/image/image_drawing.h"
#include "libmv/numeric/numeric.h"

#include <cmath>
#include <iostream>
#include <iterator>

//-------------------------------------------
// -- MSER From OpenCV

namespace libmv {
namespace detector {

/*!
 Maximal Stable Regions Parameters
 */
struct CvMSERParams
{
  //! delta, in the code, it compares (size_{i}-size_{i-delta})/size_{i-delta}
  int delta;
  //! prune the area which bigger than maxArea
  int maxArea;
  //! prune the area which smaller than minArea
  int minArea;
  //! prune the area have simliar size to its children
  float maxVariation;
  //! trace back to cut off mser with diversity < min_diversity
  float minDiversity;

  CvMSERParams()
  { //Default parameter from OpenCV
    delta = 5;
    minArea = 60;
    maxArea = 14400;
    maxVariation = 0.25f;
    minDiversity = 0.2f;
  }
};

struct CvPoint
{
  CvPoint(int _x = 0, int _y = 0)
  {
    x = _x;
    y = _y;
  }
  friend std::ostream & operator<<(std::ostream & os, const CvPoint & pt)
  {
    os << "(" << pt.x << "," << pt.y << ")";
    return os;
  }
  int x,y;
};

struct CvLinkedPoint
{
  struct CvLinkedPoint* prev;
  struct CvLinkedPoint* next;
  CvPoint pt;
};

// the history of region grown
struct CvMSERGrowHistory
{
  struct CvMSERGrowHistory* shortcut;
  struct CvMSERGrowHistory* child;
  int stable; // when it ever stabled before, record the size
  int val;
  int size;
};

struct CvMSERConnectedComp
{
  CvLinkedPoint* head;
  CvLinkedPoint* tail;
  CvMSERGrowHistory* history;
  unsigned long grey_level;
  int size;
  int dvar;   // the derivative of last var
  float var;  // the current variation (most time is the variation of one-step back)
};

struct CvContour
{
  vector<CvPoint> ptList;
  int color;
};

typedef vector< CvContour* > CvSeq;

// clear the connected component in stack
inline static void
    icvInitMSERComp ( CvMSERConnectedComp* comp )
{
  comp->size  = 0;
  comp->var   = 0;
  comp->dvar  = 1;
  comp->history = NULL;
}

// add history of size to a connected component
inline static void
    icvMSERNewHistory ( CvMSERConnectedComp* comp,
                        CvMSERGrowHistory* history )
{
  history->child = history;
  if ( NULL == comp->history )
  {
    history->shortcut = history;
    history->stable   = 0;
  }
  else
  {
    comp->history->child  = history;
    history->shortcut     = comp->history->shortcut;
    history->stable       = comp->history->stable;
  }
  history->val  = comp->grey_level;
  history->size = comp->size;
  comp->history = history;
}

// merging two connected component
inline static void
    icvMSERMergeComp ( CvMSERConnectedComp* comp1,
                       CvMSERConnectedComp* comp2,
                       CvMSERConnectedComp* comp,
                       CvMSERGrowHistory* history )
{
  CvLinkedPoint* head = NULL,* tail = NULL;
  comp->grey_level = comp2->grey_level;
  history->child = history;
  // select the winner by size
  if ( comp1->size < comp2->size )  {
    std::swap( comp1 , comp2);
  }

  {
    if ( NULL == comp1->history )
    {
      history->shortcut = history;
      history->stable   = 0;
    }
    else
    {
      comp1->history->child = history;
      history->shortcut     = comp1->history->shortcut;
      history->stable       = comp1->history->stable;
    }
    if ( NULL != comp2->history && comp2->history->stable > history->stable )	{
      history->stable = comp2->history->stable;
	}
    history->val = comp1->grey_level;
    history->size = comp1->size;
    // put comp1 to history
    comp->var = comp1->var;
    comp->dvar = comp1->dvar;
    if ( comp1->size > 0 && comp2->size > 0 )
    {
      comp1->tail->next = comp2->head;
      comp2->head->prev = comp1->tail;
    }
    head = ( comp1->size > 0 ) ? comp1->head : comp2->head;
    tail = ( comp2->size > 0 ) ? comp2->tail : comp1->tail;
    // always made the newly added in the last of the pixel list (comp1 ... comp2)
  }
  comp->head = head;
  comp->tail = tail;
  comp->history = history;
  comp->size = comp1->size + comp2->size;
}

inline static float
    icvMSERVariationCalc ( CvMSERConnectedComp* comp,
                           int delta )
{
  CvMSERGrowHistory* history = comp->history;
  int val = comp->grey_level;
  if ( NULL != history )
  {
    CvMSERGrowHistory* shortcut = history->shortcut;
    while ( shortcut != shortcut->shortcut && shortcut->val + delta > val )
      shortcut = shortcut->shortcut;
    CvMSERGrowHistory* child = shortcut->child;
    while ( child != child->child && child->val + delta <= val )
    {
      shortcut = child;
      child = child->child;
    }
    // get the position of history where the shortcut->val <= delta+val and shortcut->child->val >= delta+val
    history->shortcut = shortcut;
    return ( float ) ( comp->size-shortcut->size ) / ( float ) shortcut->size;
    // here is a small modification of MSER where cal ||R_{i}-R_{i-delta}||/||R_{i-delta}||
    // in standard MSER, cal ||R_{i+delta}-R_{i-delta}||/||R_{i}||
    // my calculation is simpler and much easier to implement
  }
  return 1.0f;
}

inline static bool
    icvMSERStableCheck ( CvMSERConnectedComp* comp,
                         CvMSERParams params )
{
  // tricky part: it actually check the stablity of one-step back
  if ( comp->history == NULL ||
       comp->history->size <= params.minArea ||
       comp->history->size >= params.maxArea )
    return false;
  float div = ( float ) ( comp->history->size-comp->history->stable ) / ( float ) comp->history->size;
  float var = icvMSERVariationCalc ( comp, params.delta );
  int dvar = ( comp->var < var || ( unsigned long ) ( comp->history->val + 1 ) < comp->grey_level );
  int stable = ( dvar && !comp->dvar && comp->var < params.maxVariation && div > params.minDiversity );
  comp->var = var;
  comp->dvar = dvar;
  if ( stable )
    comp->history->stable = comp->history->size;
  return stable != 0;
}

// add a pixel to the pixel list
inline static void
    icvAccumulateMSERComp ( CvMSERConnectedComp* comp,
                            CvLinkedPoint* point )
{
  if ( comp->size > 0 )
  {
    point->prev       = comp->tail;
    comp->tail->next  = point;
    point->next       = NULL;
  }
  else
  {
    point->prev = NULL;
    point->next = NULL;
    comp->head  = point;
  }
  comp->tail = point;
  comp->size++;
}

// convert the point set to CvSeq
inline static CvContour*
    icvMSERToContour ( CvMSERConnectedComp* comp)
{
  CvContour* contour = new CvContour;
  contour->ptList.resize(comp->history->size);
  //-- Fill the point list :
  CvLinkedPoint* lpt = comp->head;
  for ( int i = 0; i < comp->history->size && lpt != NULL; ++i )
  {
    CvPoint& pt = contour->ptList[i];
    pt.x = lpt->pt.x;
    pt.y = lpt->pt.y;
    lpt = lpt->next;
  }
  return contour;
}

// to preprocess src image to following format
// 32-bit image
// > 0 is available, < 0 is visited
// 17~19 bits is the direction
// 8~11 bits is the bucket it falls to (for BitScanForward)
// 0~8 bits is the color
template<class ImageUChar, class ImageInt>
static int*
icvPreprocessMSER_8UC1 ( ImageInt & img,
                         int*** heap_cur,
                         ImageUChar & src,
                         ImageUChar* mask = NULL)
{
  (void) mask;
  int srccpt    = src.Get_Step() - src.Width();
  int cpt_1     = img.Width() - src.Width()-1;
  int* imgptr   = &img(0,0);
  int* startptr = NULL;

  int level_size[256];
  memset(level_size, 0, 256*sizeof(int));

  for ( int i = 0; i < src.Width()+2; ++i )
  {
    *imgptr = -1;
    imgptr++;
  }
  imgptr += cpt_1 - 1;
  unsigned char * srcptr = &src(0,0);
  // PM : Do not handle the case of the mask
  {
    startptr = imgptr + img.Width()+1;
    for ( int i = 0; i < src.Height(); ++i )
    {
      *imgptr = -1;
      imgptr++;
      for ( int j = 0; j < src.Width(); ++j )
      {
        *srcptr = 0xff - *srcptr;
        level_size[*srcptr]++;
        *imgptr = ( ( *srcptr>>5 ) <<8 ) | ( *srcptr );
        imgptr++;
        srcptr++;
      }
      *imgptr = -1;
      imgptr += cpt_1;
      srcptr += srccpt;
    }
  }
  for ( int i = 0; i < src.Width()+2; ++i )
  {
    *imgptr = -1;
    imgptr++;
  }

  heap_cur[0][0] = 0;
  for ( int i = 1; i < 256; ++i )
  {
    heap_cur[i] = heap_cur[i-1]+level_size[i-1]+1;
    heap_cur[i][0] = 0;
  }
  return startptr;
}

static void
    icvExtractMSER_8UC1_Pass ( int* ioptr,
                               int* imgptr,
                               int*** heap_cur,
                               CvLinkedPoint* ptsptr,
                               CvMSERGrowHistory* histptr,
                               CvMSERConnectedComp* comptr,
                               int step,
                               int stepmask,
                               int stepgap,
                               const CvMSERParams & params,
                               int color,
                               CvSeq* contours)
{
  comptr->grey_level = 256;
  comptr++;
  comptr->grey_level = ( *imgptr ) &0xff;
  icvInitMSERComp ( comptr );
  *imgptr |= 0x80000000;
  heap_cur += ( *imgptr ) &0xff;
  int dir[] = { 1, step, -1, -step };

  for ( ; ; )
  {
    // take tour of all the 4 directions
    while ( ( ( *imgptr ) &0x70000 ) < 0x40000 )
    {
      // get the neighbor
      int* imgptr_nbr = imgptr+dir[ ( ( *imgptr ) &0x70000 ) >>16];
      if ( *imgptr_nbr >= 0 ) // if the neighbor is not visited yet
      {
        *imgptr_nbr |= 0x80000000; // mark it as visited
        if ( ( ( *imgptr_nbr ) &0xff ) < ( ( *imgptr ) &0xff ) )
        {
          // when the value of neighbor smaller than current
          // push current to boundary heap and make the neighbor to be the current one
          // create an empty comp
          ( *heap_cur ) ++;
          **heap_cur = imgptr;
          *imgptr += 0x10000;
          heap_cur += ( ( *imgptr_nbr ) &0xff )- ( ( *imgptr ) &0xff );

          imgptr = imgptr_nbr;
          comptr++;
          icvInitMSERComp ( comptr );
          comptr->grey_level = ( *imgptr ) &0xff;
          continue;
        }
        else
        {
          // otherwise, push the neighbor to boundary heap
          heap_cur[ ( ( *imgptr_nbr ) &0xff )- ( ( *imgptr ) &0xff ) ]++;
          *heap_cur[ ( ( *imgptr_nbr ) &0xff )- ( ( *imgptr ) &0xff ) ] = imgptr_nbr;

        }
      }
      *imgptr += 0x10000;
    }
    int i = ( int ) ( imgptr-ioptr );
    ptsptr->pt = CvPoint ( i&stepmask, i>>stepgap );
    // get the current location
    icvAccumulateMSERComp ( comptr, ptsptr );
    ptsptr++;
    // get the next pixel from boundary heap
    if ( **heap_cur )
    {
      imgptr = **heap_cur;
      ( *heap_cur )--;
    }
    else
    {
      heap_cur++;
      unsigned long pixel_val = 0;
      for ( unsigned long i = ( ( *imgptr ) &0xff ) +1; i < 256; i++ )
      {
        if ( **heap_cur )
        {
          pixel_val = i;
          break;
        }
        heap_cur++;
      }
      if ( pixel_val )
      {
        imgptr = **heap_cur;
        ( *heap_cur )--;
        if ( pixel_val < comptr[-1].grey_level )
        {
          // check the stablity and push a new history, increase the grey level
          if ( icvMSERStableCheck ( comptr, params ) )
          {
            CvContour* contour = icvMSERToContour ( comptr );
            contour->color = color;
            contours->push_back( contour );
          }
          icvMSERNewHistory ( comptr, histptr );
          comptr[0].grey_level = pixel_val;
          histptr++;
        }
        else
        {
          // keep merging top two comp in stack until the grey level >= pixel_val
          do
          {
            comptr--;
            icvMSERMergeComp ( comptr+1, comptr, comptr, histptr );
            histptr++;
          }
          while( pixel_val > comptr[0].grey_level);
        }
      }
      else
        break;
    }
  }
}

template <class ImageUChar>
static void
    icvExtractMSER_8UC1 ( ImageUChar & src,  // Image on which MSER must be extracted
                          CvSeq & contours,              // Detected MSER
                          const CvMSERParams & params )
{
  int step = 8;
  int stepgap = 3;
  while ( step < src.Get_Step()+2 )
  {
    step <<= 1;
    ++stepgap;
  }
  int stepmask = step - 1;

  // to speedup the process, make the width to be 2^N
  IntImage img(src.Height() + 2, step);
  int* ioptr = &img(0,0) + step + 1;
  int* imgptr = NULL;

  const int nbPoints = src.Width() * src.Height();

  // pre-allocate boundary heap
  int** heap = new int* [nbPoints + 256];
  int** heap_start[256];
  heap_start[0] = heap;

  // pre-allocate linked point and grow history
  CvLinkedPoint* pts = new CvLinkedPoint[nbPoints];
  CvMSERGrowHistory* history = new CvMSERGrowHistory[nbPoints];
  CvMSERConnectedComp comp[257];

  // darker to brighter (MSER-)
  imgptr = icvPreprocessMSER_8UC1 ( img, heap_start, src );
  icvExtractMSER_8UC1_Pass ( ioptr, imgptr, heap_start, pts, history, comp, step, stepmask, stepgap, params, -1, &contours );

  // brighter to darker (MSER+)
  imgptr = icvPreprocessMSER_8UC1 ( img, heap_start, src );
  icvExtractMSER_8UC1_Pass ( ioptr, imgptr, heap_start, pts, history, comp, step, stepmask, stepgap, params, 1, &contours );


  // clean up
  delete [] history;
  delete [] heap;
  delete [] pts;
}

void FitEllipse( const vector<CvPoint> & contour,
                       double & centerx,double  & centery,
                       double & width,double & height,double & angle )
{
  const int n = contour.size();

  // Critical case :
  if (n == 0)
    return;

  //-- Compute Centroid :
  centerx = centery = 0.0;

  for (int i = 0; i < n; ++i )
  {
    CvPoint p = contour[i];
    centerx += p.x;
    centery += p.y;
  }
  centerx /= n;
  centery /= n;

  //-- Compute excentricity (1st central moment) :
  double momentx = 0.0 , momenty = 0.0;
  for (int i = 0; i < n; ++i )
  {
    CvPoint p = contour[i];
    momentx += (p.x - centerx);
    momenty += (p.y - centery);
  }
  momentx /= n;
  momenty /= n;

  //-- Next, we calculate the mass distribution tensor for these pixels.
  double i11 = 0.0, i22 = 0.0, i12 = 0.0;
  for (int i = 0; i < n; ++i )
  {
    CvPoint p = contour[i];
    i11 += (p.y - centery)*(p.y - centery); //Along Y
    i22 += (p.x - centerx)*(p.x - centerx); //Along X
    i12 += (p.x - centerx)*(p.y - centery); //Along XY
  }
  i11 /= n;
  i22 /= n;
  i12 /= n;

  Mat tensor(2,2);
  tensor(0,0) = i11; tensor(0,1) = i12;
  tensor(1,0) = i12; tensor(1,1) = i22;

  Eigen::EigenSolver<Mat> es(tensor);
  typedef Eigen::EigenSolver<Mat>::EigenvectorType Matc;
  Matc eigvec = es.eigenvectors();
  typedef Eigen::EigenSolver<Mat>::EigenvectorType Matd;
  Matd eigval = es.eigenvalues();

  width   = sqrt( std::real(eigval(0)) ) *2.0;
  height  = sqrt( std::real(eigval(1)) ) *2.0;
  angle   = getCoterminalAngle( - atan2( std::real(eigvec(1,1)), std::real(eigvec(1,0)) ));

  if ( width > height )
  {
    std::swap ( width, height );
    angle = ( float ) ( M_PI/2.0 + angle );
    angle = getCoterminalAngle(angle);
  }

  bool bVerbose = false;
  if (bVerbose)
  {
    std::cout << std::endl
      << "Parameter : " << std::endl
      << "centerx = "<< centerx << std::endl
      << "centery = "<< centery << std::endl
      << "width = "<< width << std::endl
      << "height = "<< height << std::endl
      << "angle = " << angle << std::endl;
  }
}

struct sMserProperties
{
  sMserProperties(double x = 0.0, double y = 0.0,
                   double l1 = 0.0, double l2 = 0.0, double alpha = 0.0)
  {
    _x = x;
    _y = y;
    _l1 = l1;
    _l2 = l2;
    _alpha = alpha;
  }

  double _x,_y,_l1,_l2,_alpha;
};

template<class ImageUChar>
void detectPoints(const ImageUChar & image, vector<sMserProperties> & detectedEllipse)
{
  //----------> Assert that the input image is UCHAR-Gray format

  ImageUChar test( image );
  CvSeq mserSequence;
  icvExtractMSER_8UC1( test, mserSequence, CvMSERParams() );


  ImageUChar cpyIMG( image.Height(), image.Width(), 3 );
  cpyIMG.fill(0);
  bool bdisplayMserOnIndependantImage = false;
  bool bSaveImage = false;
  //-> Display detected regions and export the ellipse :
  for (int i=0; i < mserSequence.size(); ++i)
  {
    if (bSaveImage)
    {
      if (bdisplayMserOnIndependantImage)
      {
        cpyIMG = ImageUChar( image );
      }
    }
    unsigned char randomValueR = rand()%255;
    unsigned char randomValueG = rand()%255;
    unsigned char randomValueB = rand()%255;

    double centerx, centery, width, height, angle;
    FitEllipse(mserSequence[i]->ptList,centerx, centery, width, height, angle);

    detectedEllipse.push_back( sMserProperties(centerx, centery, width, height, angle));

    if (bSaveImage)
    {
      for ( int j=0; j < mserSequence[i]->ptList.size(); ++j)
      {
        unsigned char * ptrData = &cpyIMG(0,0);
        ptrData[ (mserSequence[i]->ptList[j].x + mserSequence[i]->ptList[j].y * cpyIMG.Width()) *3 ] = randomValueR;
        ptrData[ (mserSequence[i]->ptList[j].x + mserSequence[i]->ptList[j].y * cpyIMG.Width()) *3 +1] = randomValueG;
        ptrData[ (mserSequence[i]->ptList[j].x + mserSequence[i]->ptList[j].y * cpyIMG.Width()) *3 +2] = randomValueB;
      }

      DrawEllipse(centerx, centery, width, height,(unsigned char)255, &cpyIMG, angle);
      if (bdisplayMserOnIndependantImage)
      {
        char buffer[100];
        sprintf(buffer,"mser_%04d.png",i);
        WritePng(cpyIMG, buffer);
      }
    }
  }
  if (!bdisplayMserOnIndependantImage && bSaveImage)
  {
    WritePng(cpyIMG, "DetectedMser.png");
  }
}

} //namespace detector
} //namespace libmv

#endif // #define OPENCV_MSER_DETECTOR_H
