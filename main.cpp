#include <vector>
#include <iostream>
#include <map>
#include <numeric>
#include <ranges>
#include <iomanip>
#include <queue>
#include <chrono>
#include <cmath>

#include "dcs.hpp"
#include "string.hpp"
#include "prefix_doubling.hpp"

using size_type = uint32_t;
using namespace std;

void benchmark(const auto &S, auto dcs)
{
  size_type n = S.size();

  auto bg = chrono::high_resolution_clock::now();
  auto SA = dcs.suffix_array(S);
  auto ed = chrono::high_resolution_clock::now();
  cout << dcs.name() << " spend: " << (ed - bg).count() / 1e9 << "s\n";
  cout<<"level:"<<dcs.level<<endl;

  // cout<<"S:"<<S<<endl;
  // cout<<"SA:\n";
  // for(auto i:SA) cout<<i<<" ";
  // cout<<endl;
  // for(int i=0;i<SA.size();i++)
  // {
  //   std::cout<<std::setw(4)<<SA[i]<<' '<<S.substr(SA[i])<<'\n';
  // }

  // auto SV = string_view{S};
  // for (auto i = 1; i <= n; i++)
  // {
  //   if (SV.substr(SA[i - 1]) < SV.substr(SA[i]))
  //     continue;

  //   cout << "SA[" << i - 1 << "] is larger than SA[" << i << "]\n";
  //   cout << "error\n";
  //   exit(0);
  // }
}

void benchmark_nlgn(const auto &S) {
  auto bg = chrono::high_resolution_clock::now();
  auto SA = SA_nlgn(S);
  auto ed = chrono::high_resolution_clock::now();
  cout << "nlgn spend: " << (ed - bg).count() / 1e9 << "s\n";
}

int main()
{
  // size_type n=1000000,q;
  // RandomStringGenerator rsg;
  
  // for(int i=2;i<31;i++){
  //   auto S = rsg.gen_special_string(n);
  //   benchmark(S, DCS<size_type>(i*i));
  // }

  size_type n=1e7;
  RandomStringGenerator rsg;

  // auto S = rsg.gen_rand_string(n);
  // S = rsg.gen_special_string(n);
  // benchmark(S, DCS<size_type>(3));
  auto S = rsg.read_file(n);
  // benchmark(S, DCS<size_type>(3));
  // S = rsg.all_A(n);
  // benchmark(S, DCS<size_type>(3));

  benchmark_nlgn(S);
  benchmark(S, DCS<size_type>(3));
  benchmark(S, DCS<size_type>(4));
  benchmark(S, DCS<size_type>(5));
  benchmark(S, DCS<size_type>(6));
  benchmark(S, DCS<size_type>(7));
  benchmark(S, DCS<size_type>(8));
  benchmark(S, DCS<size_type>(9));
  benchmark(S, DCS<size_type>(10));
  benchmark(S, DCS<size_type>(11));
  benchmark(S, DCS<size_type>(12));
  benchmark(S, DCS<size_type>(13));
  benchmark(S, DCS<size_type>(14));
  benchmark(S, DCS<size_type>(15));
  benchmark(S, DCS<size_type>(16));
  benchmark(S, DCS<size_type>(17));
  benchmark(S, DCS<size_type>(18));
  benchmark(S, DCS<size_type>(19));
  benchmark(S, DCS<size_type>(20));
  benchmark(S, DCS<size_type>(21));

  // S = rsg.gen_rand_string(n);
  // benchmark(S, DCS<size_type>(64));
  // S = rsg.gen_special_string(n);
  // benchmark(S, DCS<size_type>(64));
  // S = rsg.read_file(n);
  // benchmark(S, DCS<size_type>(64));
  // S = rsg.all_A(n);
  // benchmark(S, DCS<size_type>(64));
}
