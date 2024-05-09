#pragma once

template <int DC_SIZE, typename size_type = uint32_t>
class DCS {
  std::array<std::array<size_type, DC_SIZE>, DC_SIZE> DIFF{};
  std::vector<size_type> DC;

  auto
  encode_string(const std::string &S_) {
    auto mp = std::map<char, size_type>{};
    for (const auto &c : S_)
      mp[c];

    auto stamp = size_type{};
    for (auto &[k, v] : mp)
      v = ++stamp;

    size_type n = S_.size();
    auto S = std::vector<size_type>(n + DC_SIZE);
    for (auto i = size_type{}; i < n; i++)
      S[i] = mp[S_[i]];

    return std::tuple{S, mp.size() + 1};
  }

  void
  radix_sort(
    std::vector<size_type> &I,
    std::vector<size_type> &O,
    size_type K,
    auto &&func
  ) {
    auto cnt = std::vector<size_type>(K);
    for (auto &v : I)
      cnt[func(v)]++;

    std::inclusive_scan(cnt.begin(), cnt.end(), cnt.begin());

    for (auto i = size_type{}; i < I.size(); i++) {
      auto v = I[I.size() - i - 1];
      O[--cnt[func(v)]] = v;
    }
  }

  auto
  rerank(
    const std::vector<size_type> &SA_DC,
    size_type n,
    auto &&cmp
  ) {
    auto RK = std::vector<size_type>(n);
    auto K_DC = size_type{1};
    for (size_type i = 1; i < SA_DC.size(); i++) {
      K_DC += cmp(SA_DC[i - 1], SA_DC[i]);
      RK[SA_DC[i]] = K_DC;
    }
    RK[SA_DC[0]] = 1;

    return std::tuple{RK, K_DC + 1};
  }

  auto
  gen_non_DC() {
    auto non_DC = std::vector<size_type>{};
    for (auto i = size_type{}; i < DC_SIZE; i++) {
      if (DIFF[i][i] == 0)
        continue;
      non_DC.push_back(i);
    }
    return non_DC;
  }

  // radix_sort by c_1, c_2, c_3, ..., c_DC_SIZE
  void
  sort_SA_DC(
    std::vector<size_type> &S,
    std::vector<size_type> &SA,
    size_type K
  ) {
    auto &buf1 = SA;
    auto  buf2 = buf1;
    for (size_type i = 0; i < DC_SIZE; i++) {
      auto offset = DC_SIZE - i - 1;
      radix_sort(
        buf1, buf2, K,
        [&S, offset](size_type i) {
          return S[i + offset];
        }
      );
      buf1.swap(buf2);
    }
  }

  auto
  gen_S_DC(std::vector<size_type> &RK) {
    auto S_DC = std::vector<size_type>{};
    S_DC.reserve(RK.size());
    for (auto dc : DC)
      for (auto i = dc; i < RK.size(); i += DC_SIZE)
        S_DC.push_back(RK[i]);
    for (auto i = size_type{}; i < DC_SIZE; i++)
      S_DC.push_back(0);
    return S_DC;
  }

  // radix_sort by c_1, c_2, c_3, ..., c_diff, RK[i + diff]
  void
  sort_SA_non_DC(
    std::vector<size_type> &SA_non_DC,
    std::vector<size_type> &RK,
    size_type K_DC,
    std::vector<size_type> &S,
    size_type diff,
    size_type K
  ) {
    auto &buf1 = SA_non_DC;
    auto  buf2 = buf1;

    radix_sort(
      buf1, buf2, K_DC,
      [&](size_type i) {
        return RK[i + diff];
      }
    );
    buf1.swap(buf2);

    for (auto i = size_type{}; i < diff; i++) {
      auto offset = diff - i - 1;
      radix_sort(
        buf1, buf2, K,
        [&S, offset](size_type i) {
          return S[i + offset];
        }
      );
      buf1.swap(buf2);
    }
  }

  void
  merge(
    auto &SA_list,
    auto &SA,
    auto &&cmp
  ) {
    using Item = std::pair<size_type, size_type>;
    auto cmpItem = [&cmp](Item a, Item b){
      return cmp(a.first, b.first);
    };

    // sort SA_list.size() sorted array by a priority_queue
    auto pq = std::priority_queue<Item, std::vector<Item>, decltype(cmpItem)>(cmpItem);

    auto ptr = std::vector<size_type>(SA_list.size());
    for (auto i = 0; i < ptr.size(); i++) {
      pq.push(Item{SA_list[i][ptr[i]++], i});
    }

    auto sa_ptr = size_type{};
    while (pq.size()) {
      auto [sa_v, i] = pq.top();
      pq.pop();
      SA[sa_ptr++] = sa_v;
      if (ptr[i] == SA_list[i].size())
        continue;
      pq.push({SA_list[i][ptr[i]++], i});
    }
  }

  void
  suffix_array(
    std::vector<size_type> &S,
    std::vector<size_type> &SA,
    size_type K
  ) {
    // collect index in DC (difference cover)
    auto SA_DC = std::vector<size_type>{};
    SA_DC.reserve(SA.size());
    for (auto dc : DC)
      for (auto i = dc; i < SA.size(); i += DC_SIZE)
        SA_DC.push_back(i);

    // for each suffix in DC, sort by its first DC_SIZE character (a.k.a k-mer when k = DC_SIZE, called DC_SIZE-mer)
    sort_SA_DC(S, SA_DC, K);

    // relabel each DC_SIZE-mer
    auto [RK, K_DC] = rerank(
      SA_DC, SA.size(),
      [&S](size_type i, size_type j){
        for (size_type offset = 0; offset < DC_SIZE; offset++) {
          if (S[i + offset] == S[j + offset])
            continue;
          return S[i + offset] < S[j + offset];
        }
        return false;
      }
    );

    // if all the DC_SIZE-mer is not unique, do the recursion
    if (K_DC != SA_DC.size() + 1) {
      // generate S in subproblem (S_DC)
      auto S_DC = gen_S_DC(RK);

      // do the recursion, get SA_DC from S_DC
      suffix_array(S_DC, SA_DC, K_DC);

      // reindex SA_DC and RK
      auto idx = std::vector<size_type>{};
      for (auto dc : DC)
        for (auto i = dc; i < SA.size(); i += DC_SIZE)
          idx.push_back(i);

      size_type stamp = 0;
      for (auto &v : SA_DC) {
        v = idx[v];
        RK[v] = ++stamp;
      }
    }

    // sort each non_dc independently
    auto SA_list = std::vector<std::vector<size_type>>{SA_DC};
    for (auto non_dc : gen_non_DC()) {
      auto SA_non_DC = std::vector<size_type>{};
      for (auto i = non_dc; i < SA.size(); i += DC_SIZE)
        SA_non_DC.push_back(i);

      if (SA_non_DC.size() == 0)
        continue;

      auto diff = DIFF[non_dc][non_dc];
      sort_SA_non_DC(SA_non_DC, RK, SA_DC.size() + 1, S, diff, K);
      // get SA_non_DC from the suffix where i % DC_SIZE == non_dc
      SA_list.push_back(SA_non_DC);
    }

    // merge all SA_list (SA_DC, SA_non_DC1, SA_non_DC2, SA_non_DC3, ...)
    merge(SA_list, SA, [&](size_type a, size_type b){
      auto d = DIFF[a % DC_SIZE][b % DC_SIZE];
      for (size_type x = 0; x < d; x++) {
        if (S[a + x] == S[b + x])
          continue;
        return S[a + x] > S[b + x];
      }
      return RK[a + d] > RK[b + d];
    });
  }

 public:
  DCS(const std::vector<size_type> &DC_) : DC(DC_) {
    // prepare diff table
    // let d = DIFF[x][y]. (x + d) % DC_SIZE, (y + d) % DC_SIZE in DC
    auto in_DC = std::vector<bool>(DC_SIZE);
    for (auto &dc : DC)
      in_DC[dc] = 1;

    for (size_type x = 0; x < DC_SIZE; x++) {
      for (size_type y = x; y < DC_SIZE; y++) {
        bool find = false;
        for (size_type d = 0; d < DC_SIZE; d++) {
          if (not in_DC[(x + d) % DC_SIZE] or not in_DC[(y + d) % DC_SIZE])
            continue;
          DIFF[x][y] = DIFF[y][x] = d;
          find = true;
          break;
        }

        if (not find)
          throw std::runtime_error("invalid difference cover set");
      }
    }
  }

  const std::string name() const {
    return "dc" + std::to_string(DC_SIZE);
  }

  auto
  suffix_array(const std::string &S_) {
    size_type n = S_.size();

    // encode string to vector<size_type>
    // append DC_SIZE '\0' at the end of S
    // S[i] \in [0, K), where 0 <= i < |S|
    auto [S, K] = encode_string(S_);

    auto SA = std::vector<size_type>(n + 1);
    suffix_array(S, SA, K);
    return SA;
  }
};

