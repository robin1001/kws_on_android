// Copyright (c) 2016 Personal (Binbin Zhang)
// Created on 2016-11-08
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

#ifndef FST_H_
#define FST_H_

#include <stdio.h>

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <unordered_map>

#include "utils.h"
#include "symbol-table.h"

struct Arc {
 public:
  Arc(): ilabel(0), olabel(0), weight(0.0f), next_state(0) {}
  Arc(int32_t ilabel, int32_t olabel, float weight, int32_t next_state):
      ilabel(ilabel), olabel(olabel), weight(weight),
      next_state(next_state) {}

  bool operator< (const Arc &arc) const {
    return ilabel < arc.ilabel;
  }

  void Read(std::istream& is);
  void Write(std::ostream& os) const;

  int32_t ilabel, olabel;
  float weight;
  int32_t next_state;
};

class Fst {
 public:
  Fst(): start_(0) {}
  explicit Fst(const std::string& file) {
    Read(file);
  }
  // ~Fst();
  void Reset();
  void Info() const;

  int32_t Start() const {
    return start_;
  }

  void SetStart(int32_t id) {
    start_ = id;
  }

  int32_t NumFinals() const {
    return finals_.size();
  }

  int32_t NumArcs() const {
    return arcs_.size();
  }

  int32_t NumStates() const {
    return arc_offset_.size();
  }

  bool IsFinal(int32_t id) const {
    return (finals_.find(id) != finals_.end());
  }

  int32_t NumArcs(int32_t id) const {
    if (id < NumStates() - 1) {
      return arc_offset_[id + 1] - arc_offset_[id];
    } else {
      return arcs_.size() - arc_offset_[id];
    }
  }

  const Arc *ArcStart(int32_t id) const {
    CHECK(id < NumStates());
    return arcs_.data() + arc_offset_[id];
  }

  const Arc *ArcEnd(int32_t id) const {
    CHECK(id < NumStates());
    if (id < NumStates() - 1) {
      return arcs_.data() + arc_offset_[id + 1];
    } else {
      return arcs_.data() + arcs_.size();
    }
  }

  void ReadTopo(const std::string& topo_file,
                const SymbolTable& isymbol_table,
                const SymbolTable& osymbol_table);
  void ReadTopo(const std::string& topo_file);

  void Read(const std::string& file);
  void Write(const std::string& file) const;
  void Dot(const SymbolTable& isymbol_table,
           const SymbolTable& osymbol_table) const;

 private:
  int32_t start_;
  std::vector<int32_t> arc_offset_;  // arc offset of state
  std::unordered_map<int32_t, float> finals_;
  std::vector<Arc> arcs_;
  DISALLOW_COPY_AND_ASSIGN(Fst);
};

#endif  // FST_H_
