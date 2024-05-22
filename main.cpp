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

  auto S = rsg.gen_rand_string(n);
  benchmark(S, DCS<size_type>(3));
  // S = rsg.gen_special_string(n);
  // benchmark(S, DCS<size_type>(3));
  // S = rsg.read_file(n);
  // benchmark(S, DCS<size_type>(3));
  // S = rsg.all_A(n);
  // benchmark(S, DCS<size_type>(3));
  
  
  // S = rsg.gen_rand_string(n);
  benchmark(S, DCS<size_type>(64));
  // S = rsg.gen_special_string(n);
  // benchmark(S, DCS<size_type>(64));
  // S = rsg.read_file(n);
  // benchmark(S, DCS<size_type>(64));
  // S = rsg.all_A(n);
  // benchmark(S, DCS<size_type>(64));
  

}
