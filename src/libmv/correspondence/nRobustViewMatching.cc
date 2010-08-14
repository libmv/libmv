// Copyright (c) 2010 libmv authors.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#include "libmv/base/scoped_ptr.h"
#include "libmv/base/vector_utils.h"
#include "libmv/correspondence/feature.h"
#include "libmv/correspondence/feature_matching.h"
#include "libmv/correspondence/nRobustViewMatching.h"
#include "libmv/descriptor/descriptor.h"
#include "libmv/descriptor/daisy_descriptor.h"
#include "libmv/descriptor/dipole_descriptor.h"
#include "libmv/descriptor/simpliest_descriptor.h"
#include "libmv/descriptor/surf_descriptor.h"
#include "libmv/descriptor/vector_descriptor.h"
#include "libmv/detector/detector.h"
#include "libmv/detector/fast_detector.h"
#include "libmv/detector/mser_detector.h"
#include "libmv/detector/star_detector.h"
#include "libmv/detector/surf_detector.h"
#include "libmv/image/image.h"
#include "libmv/image/image_io.h"
#include "libmv/image/image_converter.h"
#include "libmv/multiview/robust_fundamental.h"

using namespace libmv;
using namespace correspondence;
using namespace std;

/**
 * Compute the data and store it in the class map<string,T>
 *
 * \param[in] filename   The file from which the data will be extracted.
 *
 * \return True if success.
 */
bool nRobustViewMatching::computeData(const string & filename)
{
  Array3Du imageA;
  if (!ReadImage(filename.c_str(), &imageA)) {
    LOG(FATAL) << "Failed loading image: " << filename;
    return false;
  }
  else
  {
    Array3Du imageTemp;
    Rgb2Gray( imageA, &imageTemp);
    Image im( new Array3Du(imageTemp) );

    libmv::vector<libmv::Feature *> features;
    //scoped_ptr<detector::Detector> detector(detector::CreateFastDetector(9, 30,true));
    //scoped_ptr<detector::Detector> detector(detector::CreateStarDetector(true));
    //scoped_ptr<detector::Detector> detector(detector::CreateFastDetectorLimited(30,true, 1024));
    scoped_ptr<detector::Detector> detector(detector::CreateMserDetector());
    detector->Detect( im, &features, NULL);

    libmv::vector<descriptor::Descriptor *> descriptors;
    scoped_ptr<descriptor::Describer>
      //describer(descriptor::CreateSimpliestDescriber());
      describer(descriptor::CreateSurfDescriber());
      //describer(descriptor::CreateDaisyDescriber());
      //describer(descriptor::CreateDipoleDescriber());
    describer->Describe(features, im, NULL, &descriptors);

    // Copy data.
    m_ViewData.insert( make_pair(filename,FeatureSet()) );
    FeatureSet & KeypointData = m_ViewData[filename];
    KeypointData.features.resize(descriptors.size());
    for(int i = 0;i < descriptors.size(); ++i)
    {
      KeypointFeature & feat = KeypointData.features[i];
      feat.descriptor = *(descriptor::VecfDescriptor*)descriptors[i];
      *(PointFeature*)(&feat) = *(PointFeature*)features[i];
    }

    DeleteElements(&features);
    DeleteElements(&descriptors);

    return true;
  }
}

/**
* Compute the putative match between data computed from element A and B
*  Store the match data internally in the class
*  map< <string, string> , MatchObject >
*
* \param[in] The name of the filename A (use computed data for this element)
* \param[in] The name of the filename B (use computed data for this element)
*
* \return True if success.
*/
bool nRobustViewMatching::MatchData(const string & dataA, const string & dataB)
{
  // Check input data
  if ( find(m_vec_InputNames.begin(), m_vec_InputNames.end(), dataA)
          == m_vec_InputNames.end() ||
         find(m_vec_InputNames.begin(), m_vec_InputNames.end(), dataB)
          == m_vec_InputNames.end())
  {
    LOG(INFO) << "[nViewMatching::MatchData] "
              << "Could not identify one of the input name.";
    return false;
  }
  if (m_ViewData.find(dataA) == m_ViewData.end() ||
      m_ViewData.find(dataB) == m_ViewData.end())
  {
    LOG(INFO) << "[nViewMatching::MatchData] "
              << "Could not identify data for one of the input name.";
    return false;
  }

  // Computed data exist for the given name
  int iDataA = find(m_vec_InputNames.begin(), m_vec_InputNames.end(), dataA)
                - m_vec_InputNames.begin();
  int iDataB = find(m_vec_InputNames.begin(), m_vec_InputNames.end(), dataB)
                - m_vec_InputNames.begin();

  Matches matches;
  //TODO(pmoulon) make FindCandidatesMatches a parameter.
  FindCandidateMatches(m_ViewData[dataA],
                       m_ViewData[dataB],
                       &matches);
  /*FindCandidateMatches_Ratio(m_ViewData[dataA],
                       m_ViewData[dataB],
                       &matches,eMATCH_KDTREE_FLANN , 0.6f);*/
  Matches consistent_matches;
  if (computeConstrainMatches(matches,iDataA,iDataB,&consistent_matches))
  {
    matches = consistent_matches;
  }
  if (matches.NumTracks() > 0)
  {
    m_sharedData.insert(
      make_pair(
        make_pair(m_vec_InputNames[iDataA],m_vec_InputNames[iDataB]),
        matches)
      );
  }

  return true;
}

/**
* From a series of element it compute the cross putative match list.
*
* \param[in] vec_data The data on which we want compute cross matches.
*
* \return True if success (and any matches was found).
*/
bool nRobustViewMatching::computeCrossMatch( const libmv::vector<string> & vec_data)
{
  m_vec_InputNames = vec_data;
  bool bRes = true;
  for (int i=0; i < vec_data.size(); ++i)
  {
    bRes &= computeData(vec_data[i]);
  }

  bool bRes2 = true;
  for (int i=0; i < vec_data.size(); ++i) {
    for (int j=0; j < i; ++j)
    {
      if (m_ViewData.find(vec_data[i]) != m_ViewData.end() &&
        m_ViewData.find(vec_data[j]) != m_ViewData.end())
      {
        bRes2 &= this->MatchData( vec_data[i], vec_data[j]);
      }
    }
  }
  return bRes2;
}

/**
* Give the posibility to constrain the matches list.
*
* \param[in] matchIn The input match data between indexA and indexB.
* \param[in] dataAindex The reference index for element A.
* \param[in] dataBindex The reference index for element B.
* \param[out] matchesOut The output match that satisfy the internal constraint.
*
* \return True if success.
*/
bool nRobustViewMatching::computeConstrainMatches(const Matches & matchIn,
                             int dataAindex,
                             int dataBindex,
                             Matches * matchesOut)
{
  if (matchesOut == NULL)
  {
    LOG(INFO) << "[nViewMatching::computeConstrainMatches]"
              << " Could not export constrained matches.";
    return false;
  }
  libmv::vector<Mat> x;
  libmv::vector<int> tracks, images;
  images.push_back(0);
  images.push_back(1);
  PointMatchMatrices(matchIn, images, &tracks, &x);

  libmv::vector<int> inliers;
  Mat3 H;
  // TODO(pmoulon) Make the Correspondence filter a parameter.
  //HomographyFromCorrespondences2PointRobust(x[0], x[1], 0.3, &H, &inliers);
  //HomographyFromCorrespondences4PointRobust(x[0], x[1], 0.3, &H, &inliers);
  //AffineFromCorrespondences2PointRobust(x[0], x[1], 1, &H, &inliers);
  FundamentalFromCorrespondences7PointRobust(x[0], x[1], 1.0, &H, &inliers);

  //TODO(pmoulon) insert an optimization phase.
  // Rerun Robust correspondance on the inliers.
  // it will allow to compute a better model and filter ugly fitting.

  //-- Assert that the output of the model is consistent :
  // As much as the minimal points are inliers.
  if (inliers.size() > 4 * 2) {
    // If tracks table is empty initialize it
    if (m_featureToTrackTable.size() == 0)  {
      // Build new correspondence graph containing only inliers.
      for (int l = 0; l < inliers.size(); ++l)  {
        const int k = inliers[l];
        m_featureToTrackTable[matchIn.Get(0, tracks[k])] = l;
        m_featureToTrackTable[matchIn.Get(1, tracks[k])] = l;
        m_tracks.Insert(dataAindex, l,
            matchIn.Get(dataBindex, tracks[k]));
        m_tracks.Insert(dataBindex, l,
            matchIn.Get(dataAindex, tracks[k]));
      }
    }
    else  {
      // Else update the tracks
      for (int l = 0; l < inliers.size(); ++l)  {
        const int k = inliers[l];
        map<const Feature*, int>::const_iterator iter =
          m_featureToTrackTable.find(matchIn.Get(1, tracks[k]));

        if (iter!=m_featureToTrackTable.end())  {
          // Add a feature to the existing track
          const int trackIndex = iter->second;
          m_featureToTrackTable[matchIn.Get(0, tracks[k])] = trackIndex;
          m_tracks.Insert(dataAindex, trackIndex,
            matchIn.Get(0, tracks[k]));
        }
        else  {
          // It's a new track
          const int trackIndex = m_tracks.NumTracks();
          m_featureToTrackTable[matchIn.Get(0, tracks[k])] = trackIndex;
          m_featureToTrackTable[matchIn.Get(1, tracks[k])] = trackIndex;
          m_tracks.Insert(dataAindex, trackIndex,
              matchIn.Get(0, tracks[k]));
          m_tracks.Insert(dataBindex, trackIndex,
              matchIn.Get(1, tracks[k]));
        }
      }
    }
    // Export common feature between the two view
    if (matchesOut) {
      Matches & consistent_matches = *matchesOut;
      // Build new correspondence graph containing only inliers.
      for (int l = 0; l < inliers.size(); ++l) {
        int k = inliers[l];
        for (int i = 0; i < 2; ++i) {
          consistent_matches.Insert(images[i], tracks[k],
              matchIn.Get(images[i], tracks[k]));
        }
      }
    }
  }
  return true;
}

