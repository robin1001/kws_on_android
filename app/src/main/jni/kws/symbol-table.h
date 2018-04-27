// Copyright (c) 2016 Personal (Binbin Zhang)
// Created on 2016-11-11
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

#ifndef SYMBOL_TABLE_H_
#define SYMBOL_TABLE_H_

#include <stdio.h>

#include <string>
#include <unordered_map>

#include "utils.h"

const int kEpsilon = 0;

class SymbolTable {
 public:
  explicit SymbolTable(const std::string& symbol_file) {
    ReadSymbolFile(symbol_file);
  }

  ~SymbolTable() {}

  std::string GetSymbol(int32_t id) const {
    CHECK(symbol_tabel_.find(id) != symbol_tabel_.end());
    return symbol_tabel_[id];
  }

  // GetId is used in the construction fst period
  // so here just a lazy/inefficient implemenation
  int32_t GetId(const std::string &symbol) const {
    for (uint32_t i = 0; i < symbol_tabel_.size(); i++) {
      if (symbol == symbol_tabel_[i]) return static_cast<int32_t>(i);
    }
    // return -f if not find
    return -1;
  }

  bool HaveId(int32_t id) const {
    return (symbol_tabel_.find(id) != symbol_tabel_.end());
  }

 protected:
  void ReadSymbolFile(const std::string &symbol_file) {
    FILE *fp = fopen(symbol_file.c_str(), "r");
    if (!fp) {
      ERROR("%s not exint, please check!!!", symbol_file.c_str());
    }
    char buffer[1024], str[1024];
    int id;
    while (fgets(buffer, 1024, fp)) {
      int num = sscanf(buffer, "%s %d", str, &id);
      if (num != 2) {
        ERROR("each line shoud have 2 fields, symbol & id");
      }
      CHECK(str != NULL);
      CHECK(id >= 0);

      std::string symbol = str;
      symbol_tabel_[id] = symbol;
    }
    fclose(fp);
  }

 private:
  mutable std::unordered_map<int32_t, std::string> symbol_tabel_;
  DISALLOW_COPY_AND_ASSIGN(SymbolTable);
};

#endif  // SYMBOL_TABLE_H_
