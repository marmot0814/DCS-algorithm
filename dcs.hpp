#pragma once
#define debug
template <typename size_type = uint32_t>
class DCS
{
  int DC_SIZE;
  
  std::vector<std::vector<size_type>> DIFF =
      std::vector<std::vector<size_type>>(DC_SIZE, std::vector<size_type>(DC_SIZE));
  std::vector<size_type> DC;

  auto
  encode_string(const std::string &S_)
  {
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
      auto &&func)
  {
    auto cnt = std::vector<size_type>(K);
    for (auto &v : I)
      cnt[func(v)]++;

    std::inclusive_scan(cnt.begin(), cnt.end(), cnt.begin());

    for (auto i = size_type{}; i < I.size(); i++)
    {
      auto v = I[I.size() - i - 1];
      O[--cnt[func(v)]] = v;
    }
  }

  auto
  rerank(
      const std::vector<size_type> &SA_DC,
      size_type n,
      auto &&cmp)
  {
    auto RK = std::vector<size_type>(n + DC_SIZE);
    auto K_DC = size_type{1};
    for (size_type i = 1; i < SA_DC.size(); i++)
    {
      K_DC += cmp(SA_DC[i - 1], SA_DC[i]);
      RK[SA_DC[i]] = K_DC;
    }
    RK[SA_DC[0]] = 1;

    return std::tuple{RK, K_DC + 1};
  }

  auto
  gen_non_DC()
  {
    auto non_DC = std::vector<size_type>{};
    for (auto i = size_type{}; i < DC_SIZE; i++)
    {
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
      size_type K)
  {
    auto &buf1 = SA;
    auto buf2 = buf1;
    for (size_type i = 0; i < DC_SIZE; i++)
    {
      auto offset = DC_SIZE - i - 1;
      radix_sort(
          buf1, buf2, K,
          [&S, offset](size_type i)
          {
            return S[i + offset];
          });
      buf1.swap(buf2);
    }
  }

  auto
  gen_S_DC(size_type sa_n, std::vector<size_type> &RK)
  {
    auto S_DC = std::vector<size_type>{};
    S_DC.reserve(sa_n + DC_SIZE);
    for (auto dc : DC)
      for (auto i = dc; i < sa_n; i += DC_SIZE)
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
      size_type K)
  {
    auto &buf1 = SA_non_DC;
    auto buf2 = buf1;

    radix_sort(
        buf1, buf2, K_DC,
        [&](size_type i)
        {
          return RK[i + diff];
        });
    buf1.swap(buf2);

    for (auto i = size_type{}; i < diff; i++)
    {
      auto offset = diff - i - 1;
      radix_sort(
          buf1, buf2, K,
          [&S, offset](size_type i)
          {
            return S[i + offset];
          });
      buf1.swap(buf2);
    }
    
  }

  void
  merge_DC_non_DC(
      auto &SA_list,
      auto &merged
      )
  {
    auto DC_list = std::vector<std::vector<size_type>> (DC_SIZE);
    for(auto i:SA_list){
      for(auto j:i){
        DC_list[j%DC_SIZE].push_back(j);
      }
    }
    size_type tmp=0;
    for(auto i:DC_list){
      //std::cout<<"mod:"<<i[0]%DC_SIZE<<'\n';
      for(auto j:i){
        merged[tmp]=j;
        tmp++;
        //std::cout<<j<<" ";
      }
      //std::cout<<'\n';
    }
    
    // std::cout<<"merged:\n";
    // for(auto &i:merged) std::cout<<i<<' ';
    // std::cout<<'\n';
    
  }

  size_type
  hash_substr(
      auto &S,
      size_type l,
      size_type r,
      size_type K
  )
  {
    size_type h = 0;
    for(int i=--r;i>=l;i--){
      if(i>=S.length())
        continue;
      h = h*(K+1)+S[i];
    }
    return h;
  }

  void
  suffix_array(
      std::vector<size_type> &S,
      std::vector<size_type> &SA,
      size_type K)
  {
    level++;
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
        [&S, this](size_type i, size_type j)
        {
          for (size_type offset = 0; offset < DC_SIZE; offset++)
          {
            if (S[i + offset] == S[j + offset])
              continue;
            return S[i + offset] < S[j + offset];
          }
          return false;
        });

    // if all the DC_SIZE-mer is not unique, do the recursion
    if (K_DC != SA_DC.size() + 1)
    {
      // generate S in subproblem (S_DC)
      auto S_DC = gen_S_DC(SA.size(), RK);

      // do the recursion, get SA_DC from S_DC
      suffix_array(S_DC, SA_DC, K_DC);

      // reindex SA_DC and RK
      auto idx = std::vector<size_type>{};
      for (auto dc : DC)
        for (auto i = dc; i < SA.size(); i += DC_SIZE)
          idx.push_back(i);

      size_type stamp = 0;
      for (auto &v : SA_DC)
      {
        v = idx[v];
        RK[v] = ++stamp;
      }
    }

    // sort each non_dc independently
    auto SA_list = std::vector<std::vector<size_type>>{SA_DC};
    for (auto non_dc : gen_non_DC())
    {
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
    merge_DC_non_DC(SA_list,SA);
    sort_SA_DC(S,SA,K);
    // std::cout<<"sorted:\n";
    // for(auto &i:SA){
    //   std::cout<<i<<" ";
    // }
    // std::cout<<'\n';

    size_type l=0,r=0;
    
    while(r!=SA.size())
    {
      //std::cout<<"l:"<<l<<'\n';
      
      bool flag=1;
      while(flag){
        r++;
        if(r==SA.size()) break;
        for(size_type i=0;i<DC_SIZE;i++){
          if(S[SA[l]+i]!=S[SA[r]+i]) {
            flag=0;
            break;
          }
        }
      }
      //std::cout<<"r:"<<r<<'\n';
      std::vector<size_type> ptr, ptr_size;
      ptr.push_back(l);
      ptr_size.push_back(1);
      for(size_type i=l+1;i<r;i++)
      {
        if((SA[i]%DC_SIZE)!=(SA[ptr.back()]%DC_SIZE)){
          ptr.push_back(i);
          ptr_size.push_back(1);
        } else ptr_size.back()++;
      }
      size_type total=r-l;
      std::vector<size_type> tmp;
      while(total--){
        size_type min_idx=std::numeric_limits<size_type>::max();
        for(size_type i=0;i<ptr.size();i++)
        {
          if(ptr_size[i]==0)
            continue;
          if(min_idx==std::numeric_limits<size_type>::max()){
            min_idx=i;
          }else{
            size_type sa_x=SA[ptr[min_idx]],sa_y=SA[ptr[i]];
            auto d=DIFF[sa_x%DC_SIZE][sa_y%DC_SIZE];
            if(RK[sa_x+d]>RK[sa_y+d]){
              min_idx=i;
            }
          }
        }
        tmp.push_back(SA[ptr[min_idx]]);
        ptr[min_idx]++;
        ptr_size[min_idx]--;
      }
      for(size_type i=0;i<tmp.size();i++){
        SA[l+i]=tmp[i];
      }
      l=r;
    }

    
  }

public:
  int level=0;

  std::map<size_t, std::vector<size_type>> DC_lookup = {
    { 3, std::vector<size_type>{1, 2}},
    { 4, std::vector<size_type>{1, 2, 3}},
    { 5, std::vector<size_type>{1, 2, 3}},
    { 6, std::vector<size_type>{1, 2, 4}},
    { 7, std::vector<size_type>{1, 2, 4}},
    { 8, std::vector<size_type>{1, 2, 3, 5}},
    { 9, std::vector<size_type>{1, 2, 3, 5}},
    {10, std::vector<size_type>{1, 2, 3, 6}},
    {11, std::vector<size_type>{1, 2, 3, 6}},
    {12, std::vector<size_type>{1, 2, 4, 8}},
    {13, std::vector<size_type>{1, 2, 4, 10}},
    {14, std::vector<size_type>{1, 2, 3, 4, 8}},
    {15, std::vector<size_type>{1, 2, 3, 4, 8}},
    {16, std::vector<size_type>{1, 2, 3, 6, 9}},
    {17, std::vector<size_type>{1, 2, 3, 5, 13}},
    {18, std::vector<size_type>{1, 2, 3, 6, 12}},
    {19, std::vector<size_type>{1, 2, 3, 7, 10}},
    {20, std::vector<size_type>{1, 2, 3, 4, 7, 11}},
    {21, std::vector<size_type>{1, 2, 5, 15, 17}}
  };

  DCS(const int DCsz) : DC_SIZE(DCsz)
  {
    size_type q = DC_SIZE;
    size_type qq = (q + 1) / 2;
    size_type sq = std::ceil(std::sqrt(qq));
    DC.resize(0);
    if (sq * sq * 2 == q || q == 4)
      DC.push_back(0);
    for (size_type i = 1; i < sq; i++)
      DC.push_back(i);
    for (size_type i = 1; i <= sq && i * sq < q; i++)
      DC.push_back(i * sq);

    if (DC_lookup.count(DC_SIZE))
      DC = DC_lookup[DC_SIZE];

    // for(auto i:DC) std::cout<<i<<" ";
    // std::cout<<std::endl;
    for (auto &vv : DIFF)
      for (auto &v : vv)
        v = std::numeric_limits<size_type>::max();

    // prepare diff table
    // let d = DIFF[x][y]. (x + d) % DC_SIZE, (y + d) % DC_SIZE in DC
    for (auto i : DC)
    {
      for (auto j : DC)
      {
        for (size_type d = 0; d < DC_SIZE; d++)
        {
          size_type x = (i + d) % DC_SIZE;
          size_type y = (j + d) % DC_SIZE;
          auto &diff = DIFF[x][y];
          diff = std::min(diff, (DC_SIZE - d) % DC_SIZE);
        }
      }
    }

    for (auto &vv : DIFF)
      for (auto &v : vv)
        if (v == std::numeric_limits<size_type>::max())
          throw std::runtime_error("invalid difference cover set");
  }

  const std::string name() const
  {
    return "dc" + std::to_string(DC_SIZE);
  }

  auto
  suffix_array(const std::string &S_)
  {
    size_type n = S_.size();

    if (n == 0)
      return std::vector<size_type>{0};

    // encode string to vector<size_type>
    // append DC_SIZE '\0' at the end of S
    // S[i] \in [0, K), where 0 <= i < |S|
    auto [S, K] = encode_string(S_);

    auto SA = std::vector<size_type>(n + 1);
    suffix_array(S, SA, K);
    return SA;
  }
};
