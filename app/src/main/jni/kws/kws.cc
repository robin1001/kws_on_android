// Copyright (c) 2016 Personal (Binbin Zhang)
// Created on 2018-03-06
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "kws.h"

bool Kws::DetectOnline(const std::vector<float>& wave, bool end_of_stream,
                       float* confidence, int32_t* keyword) {
  feature_pipeline_.AcceptRawWav(wave);
  if (end_of_stream) feature_pipeline_.SetDone();
  std::vector<float> feat;
  int num_frames_ready = feature_pipeline_.ReadFeature(t_, &feat);
  if (num_frames_ready == 0) return false;
  int feat_dim = feature_pipeline_.FeatureDim();
  CHECK(feat_dim == net_.InDim());
  Matrix<float> in(feat.data(), num_frames_ready, feat_dim),
                out(num_frames_ready, net_.OutDim());
  net_.Forward(in, &out);

  *confidence = 0.0f;
  *keyword = 0;
  bool has_legal = false;
  for (int i = 0; i < out.NumRows(); i++) {
    float tmp_confidence = 0.0;
    int32_t tmp_keyword = 0;
    bool legal = keyword_spotter_.Spot(out.Row(i).Data(), out.NumCols(),
                                       &tmp_confidence, &tmp_keyword);
    // LOG("t %d confidence %f %d", t_+i, confidence, (int)detected);
    if (legal) {
      has_legal = true;
      if (tmp_confidence > *confidence) {
        *confidence = tmp_confidence;
        *keyword = tmp_keyword;
      }
    }
  }
  t_ += num_frames_ready;
  if (t_ > 1000) {  // 10 seconds
    t_ = 0;
    feature_pipeline_.Reset();
  }
  return has_legal;
}

