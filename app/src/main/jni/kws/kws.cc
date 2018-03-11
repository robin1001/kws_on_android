/*
 * Created on 2018-03-06
 * Author: Zhang Binbin
 */

#include "kws.h"

bool Kws::DetectOnline(const std::vector<float> &wave, bool end_of_stream) {
    LOG("t %d feature accept wav", t_);
    feature_pipeline_.AcceptRawWav(wave);
    if (end_of_stream) feature_pipeline_.SetDone();
    std::vector<float> feat;
    LOG("t %d feature read feature", t_);
    int num_frames_ready = feature_pipeline_.ReadFeature(t_, &feat);
    LOG("t %d feature num_frames_ready %d", t_, num_frames_ready);
    if (num_frames_ready == 0) return false;
    int feat_dim = feature_pipeline_.FeatureDim();
    CHECK(feat_dim == net_.InDim());
    LOG("t %d nnet forward", t_);
    Matrix<float> in(feat.data(), num_frames_ready, feat_dim),
                  out(num_frames_ready, net_.OutDim());
    net_.Forward(in, &out);
  
    LOG("t %d keyword spot", t_);
    bool spot = false;
    float confidence = 0.0;
    for (int i = 0; i < out.NumRows(); i++) {
        if (keyword_spotter_.Spot(out.Row(i).Data(), out.NumCols(), &confidence)) {
            spot = true;
        }
    }
    LOG("t %d confidence %f", t_, confidence);

    t_ += num_frames_ready;
    if (t_ > 1000) { // 10 seconds
        t_ = 0;
        feature_pipeline_.Reset();
    }

    return spot;
}

