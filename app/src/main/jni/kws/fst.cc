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

#include <string.h>

#include <fstream>

#include "fst.h"

template<class T>
static void ReadBasic(std::istream& is, T* t) {
  is.read(reinterpret_cast<char *>(t), sizeof(T));
}

template<class T>
static void WriteBasic(std::ostream& os, T t) {
  os.write(reinterpret_cast<char *>(&t), sizeof(T));
}


void Arc::Read(std::istream& is) {
  ReadBasic(is, &ilabel);
  ReadBasic(is, &weight);
  ReadBasic(is, &olabel);
  ReadBasic(is, &next_state);
}

void Arc::Write(std::ostream& os) const {
  WriteBasic(os, ilabel);
  WriteBasic(os, weight);
  WriteBasic(os, olabel);
  WriteBasic(os, next_state);
}

void Fst::Reset() {
  start_ = 0;
  arcs_.clear();
  arc_offset_.clear();
  finals_.clear();
}

// A stupid implementation, it will be very slow when the
// isymbol_tabel or osymbol_table is very big, for that
// SymbolTable is designed for find symbol by id, but aslo
// have a stupid implementaion of find id by symbol.
// However,for memory reasons, it's not in an efficient way
void Fst::ReadTopo(const std::string& topo_file,
                   const SymbolTable& isymbol_table,
                   const SymbolTable& osymbol_table) {
  Reset();
  FILE *fp = fopen(topo_file.c_str(), "r");
  if (!fp) {
    ERROR("file %s not exist", topo_file.c_str());
  }

  char buffer[1024];
  char ilabel[1024], olabel[1024];
  bool first_line = true;
  int32_t src, dest;
  float weight = 0.0f;

  std::vector<std::vector<Arc> > all_arcs;
  while (fgets(buffer, 1024, fp)) {
    int32_t num = sscanf(buffer, "%d %d %s %s %f",
                         &src, &dest, ilabel, olabel, &weight);
    if (num >= 4) {
      if (num == 4) weight = 0;
      if (first_line) {
        first_line = false;
        start_ = src;
      }
      Arc arc(isymbol_table.GetId(ilabel), osymbol_table.GetId(olabel),
              weight, dest);
      if (src >= static_cast<int32_t>(all_arcs.size()))
        all_arcs.resize(src + 1);
      if (dest >= static_cast<int32_t>(all_arcs.size()))
        all_arcs.resize(dest + 1);
      all_arcs[src].push_back(arc);
    } else if (sscanf(buffer, "%d %f", &src, &weight) == 2) {
      finals_[src] = weight;
    } else if (sscanf(buffer, "%d", &src) == 1) {
      finals_[src] = 0.0f;
    } else {
      ERROR("wrong line, expected (src, dest, ilabel, olabel, weight) "
            "or (final, weight) but get %s", buffer);
      break;
    }
  }
  fclose(fp);

  arc_offset_.resize(all_arcs.size());
  int32_t offset = 0;
  for (uint32_t i = 0; i < all_arcs.size(); i++) {
    arc_offset_[i] = offset;
    arcs_.insert(arcs_.end(), all_arcs[i].begin(), all_arcs[i].end());
    offset += all_arcs[i].size();
  }
}

// For directly convert openfst file to xdecoder fst
void Fst::ReadTopo(const std::string& topo_file) {
  Reset();
  FILE *fp = fopen(topo_file.c_str(), "r");
  if (!fp) {
    ERROR("file %s not exist", topo_file.c_str());
  }

  char buffer[1024];
  int32_t ilabel, olabel;
  bool first_line = true;
  int32_t src, dest;
  float weight = 0.0f;

  std::vector<std::vector<Arc> > all_arcs;
  while (fgets(buffer, 1024, fp)) {
    int32_t num = sscanf(buffer, "%d %d %d %d %f",
                         &src, &dest, &ilabel, &olabel, &weight);
    if (num >= 4) {
      if (num == 4) weight = 0;
      if (first_line) {
        first_line = false;
        start_ = src;
      }
      Arc arc(ilabel, olabel, weight, dest);
      if (src >= static_cast<int32_t>(all_arcs.size()))
        all_arcs.resize(src + 1);
      if (dest >= static_cast<int32_t>(all_arcs.size()))
        all_arcs.resize(dest + 1);
      all_arcs[src].push_back(arc);
    } else if (sscanf(buffer, "%d %f", &src, &weight) == 2) {
      finals_[src] = weight;
    } else if (sscanf(buffer, "%d", &src) == 1) {
      finals_[src] = 0.0f;
    } else {
      ERROR("wrong line, expected (src, dest, ilabel, olabel, weight) "
            "or (final, weight) but get %s", buffer);
      break;
    }
  }
  fclose(fp);

  arc_offset_.resize(all_arcs.size());
  int32_t offset = 0;
  for (uint32_t i = 0; i < all_arcs.size(); i++) {
    arc_offset_[i] = offset;
    arcs_.insert(arcs_.end(), all_arcs[i].begin(), all_arcs[i].end());
    offset += all_arcs[i].size();
  }
}



// Show the text format fsm info
void Fst::Info() const {
  printf("fst info table\n");
  // state arc start info
  printf("start id:\t%d\n", start_);
  printf("num_states:\t%d\n", NumStates());
  printf("num_arcs:\t%d\n", NumArcs());
  // final set info
  printf("final states:\t%d { ", NumFinals());
  std::unordered_map<int32_t, float>::const_iterator it = finals_.begin();
  for (; it != finals_.end(); it++) {
    printf("(%d, %f) ", it->first, it->second);
  }
  printf("}\n");

  // state info
  for (int32_t i = 0; i < NumStates(); i++) {
    printf("state %d arcs %d: { ", i, NumArcs(i));
    for (const Arc *arc = ArcStart(i); arc != ArcEnd(i); arc++) {
      printf("(%d, %d, %f, %d) ", arc->ilabel,
                                           arc->olabel,
                                           arc->weight,
                                           arc->next_state);
    }
    printf("}\n");
  }
}

void Fst::Read(const std::string& filename) {
  Reset();
  std::ifstream is(filename, std::ifstream::binary);
  if (is.fail()) {
    ERROR("read file %s error, check!!!", filename.c_str());
  }
  int32_t num_states, num_finals, num_arcs;
  ReadBasic(is, &start_);
  ReadBasic(is, &num_states);
  ReadBasic(is, &num_finals);
  ReadBasic(is, &num_arcs);

  arc_offset_.resize(num_states);
  for (int i = 0; i < num_states; i++) {
    ReadBasic(is, &arc_offset_[i]);
  }

  for (int i = 0; i < num_finals; i++) {
    int32_t state;
    float weight;
    ReadBasic(is, &state);
    ReadBasic(is, &weight);
    finals_[state] = weight;
  }

  arcs_.resize(num_arcs);
  for (int i = 0; i < num_arcs; i++) {
    arcs_[i].Read(is);
  }
}

void Fst::Write(const std::string& filename) const {
  std::ofstream os(filename, std::ofstream::binary);
  if (os.fail()) {
    ERROR("write file %s error, check!!!", filename.c_str());
  }

  int32_t num_states = NumStates(),
          num_finals = NumFinals(),
          num_arcs = NumArcs();
  WriteBasic(os, start_);
  WriteBasic(os, num_states);
  WriteBasic(os, num_finals);
  WriteBasic(os, num_arcs);

  for (int i = 0; i < num_states; i++) {
    WriteBasic(os, arc_offset_[i]);
  }

  std::unordered_map<int, float>::const_iterator it = finals_.begin();
  for (; it != finals_.end(); it++) {
    WriteBasic(os, it->first);
    WriteBasic(os, it->second);
  }

  for (int i = 0; i < num_arcs; i++) {
    arcs_[i].Write(os);
  }
}

void Fst::Dot(const SymbolTable& isymbol_table,
              const SymbolTable& osymbol_table) const {
  printf("digraph FSM {\n");
  printf("rankdir = LR;\n");
  // printf("orientation = Landscape;\n");
  printf("node [shape = \"circle\"]\n");
  for (int32_t i = 0; i < NumStates(); i++) {
    if (IsFinal(i)) {
      printf("%d [label = \"%d\" shape = doublecircle ]\n", i, i);
    } else {
      printf("%d [label = \"%d\" ]\n", i, i);
    }
    for (const Arc *arc = ArcStart(i); arc != ArcEnd(i); arc++) {
      printf("\t %d -> %d [label = \"%s:%s/%f\" ]\n", i,
                    arc->next_state,
                    isymbol_table.GetSymbol(arc->ilabel).c_str(),
                    osymbol_table.GetSymbol(arc->olabel).c_str(),
                    arc->weight);
    }
  }
  printf("}\n");
}

