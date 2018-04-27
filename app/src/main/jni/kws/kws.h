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

#ifndef KWS_H_
#define KWS_H_

#include <string>
#include <vector>

#include "net.h"
#include "feature-pipeline.h"
#include "keyword-spot.h"

struct KwsConfig {
  FeaturePipelineConfig feature_config;
  std::string net_file;
  std::string fst_file;
  std::string filler_table_file;
  float thresh;  // threshold
  float min_keyword_frames;
  float min_frames_for_last_state;

  KwsConfig(): thresh(0.8),
               min_keyword_frames(0),
               min_frames_for_last_state(3) {}
};

class Kws {
 public:
  explicit Kws(const KwsConfig &config): kws_config_(config),
                                feature_pipeline_(config.feature_config),
                                net_(config.net_file),
                                filler_table_(config.filler_table_file),
                                fst_(config.fst_file),
                                keyword_spotter_(fst_, filler_table_),
                                t_(0) {
      keyword_spotter_.SetSpotThreshold(kws_config_.thresh);
      keyword_spotter_.SetMinKeywordFrames(kws_config_.min_keyword_frames);
      keyword_spotter_.SetMinFramesForLastState(
          kws_config_.min_frames_for_last_state);
  }
  void Reset() {
    t_ = 0;
    feature_pipeline_.Reset();
    keyword_spotter_.Reset();
  }

  bool DetectOnline(const std::vector<float>& wave, bool end_of_stream,
                    float* confidence, int32_t* keyword);

  void SetThresh(float thresh) {
    keyword_spotter_.SetSpotThreshold(thresh);
  }

 private:
  const KwsConfig& kws_config_;
  FeaturePipeline feature_pipeline_;
  Net net_;
  SymbolTable filler_table_;
  Fst fst_;
  KeywordSpot keyword_spotter_;
  int t_;
};

#endif  // KWS_H_
