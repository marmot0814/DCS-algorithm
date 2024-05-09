#include <vector>
#include <iostream>
#include <map>
#include <numeric>
#include <ranges>
#include <iomanip>
#include <queue>
#include <chrono>

#include "dcs.hpp"

using size_type = uint32_t;
void benchmark(const auto &S, auto dcs) {
  size_type n = S.size();

  auto bg = std::chrono::high_resolution_clock::now();
  auto SA = dcs.suffix_array(S);
  auto ed = std::chrono::high_resolution_clock::now();
  std::cout << dcs.name() << " spend: " << (ed - bg).count() / 1e9 << "s\n";

  auto SV = std::string_view{S};
  for (auto i = 1; i <= n; i++) {
    if (SV.substr(SA[i - 1]) < SV.substr(SA[i]))
      continue;

    std::cout << "SA[" << SA[i - 1] << "] is larger than SA[" << SA[i] << "]\n";
    std::cout << "error\n";
    exit(0);
  }
}

auto gen_dna_string(size_type n) {
  auto S = std::string{};
  for (auto i = size_type{}; i < n; i++)
    S += "ACGT"[rand() % 4];
  return S;
}

int main(int argc, char **argv) {
  if (argc - 1 != 1) {
    std::cerr << "Usage: " << argv[0] << " <sequence length>\n";
    exit(0);
  }

  size_type n = std::atoi(argv[1]);
  auto S = gen_dna_string(n);

  benchmark(S, DCS<64, size_type>({1, 2, 3, 6, 15, 17, 35, 43, 60}));
  benchmark(S, DCS< 3, size_type>({1, 2}));
}
