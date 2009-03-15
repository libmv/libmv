// Copyright (c) 2007, 2008 libmv authors.
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


#ifndef UI_TVR_MAIN_WINWOW_H_
#define UI_TVR_MAIN_WINWOW_H_


#include <QMainWindow>
#include <QMenu>
#include <QAction>

#include "ui/tvr/features.h"
#include "ui/tvr/tvr_document.h"
#include "ui/tvr/match_viewer.h"


class TvrMainWindow : public QMainWindow {
  Q_OBJECT

 public:
  TvrMainWindow(QWidget *parent = 0);
  ~TvrMainWindow();

 public slots:
  void OpenImages();
  void ComputeFeatures();
  void ComputeFeatures(int image_index);
  void ComputeCandidateMatches();
  void ComputeRobustMatches();
  void FocalFromFundamental();
  
 private:
  void CreateActions();
  void CreateMenus();
  void SynchronizeDepthmapList();
  
 private:
  // Data.
  TvrDocument document_;

  // Qt widgets, menus and actions.
  QMenu *file_menu_;
  QAction *open_images_action_;
  QMenu *matching_menu_;
  QAction *compute_features_action_;
  QAction *compute_candidate_matches_action_;
  QAction *compute_robust_matches_action_;
  QMenu *calibration_menu_;
  QAction *focal_from_fundamental_action_;

  MatchViewer *viewer_;
};

#endif // UI_TVR_MAIN_WINWOW_H_
