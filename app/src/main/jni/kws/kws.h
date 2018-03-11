/*
 * Created on 2018-03-06
 * Author: Zhang Binbin
 */

#ifndef KWS_H_
#define KWS_H_

#include "feature-pipeline.h"
#include "net.h"
#include "keyword-spot.h"

struct KwsConfig {
    FeaturePipelineConfig feature_config;
    std::string net_file;
    std::string fsm_file;
    float thresh; // threshold
    KwsConfig(): thresh(0.8) {}
};

class Kws {
public:
    Kws(const KwsConfig &config): kws_config_(config), 
                                  feature_pipeline_(config.feature_config),
                                  net_(config.net_file),
                                  fsm_(config.fsm_file), 
                                  keyword_spotter_(fsm_), 
                                  t_(0) {}
    void Reset() {
        t_ = 0;
        feature_pipeline_.Reset();
        //keyword_spotter_.Reset();
    }

    bool DetectOnline(const std::vector<float> &wave, bool end_of_stream = true);

    void SetThresh(float thresh) {
        keyword_spotter_.SetSpotThreshold(thresh);
    }
    
private:
    const KwsConfig &kws_config_;
    FeaturePipeline feature_pipeline_;
    Net net_;
    Fsm fsm_;
    KeywordSpot keyword_spotter_;
    int t_;
};

#endif
