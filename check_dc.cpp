#include <bits/stdc++.h>
#define pb push_back
using namespace std;

vector<int> get_DC_num(int q)
{
    vector<int> Dq;
    q=(q/2)+1;
    int sq = ceil(sqrt(q))+2;
    for(int i=0;i<=sq;i++) Dq.pb(i);
    for(int i=sq;i<q;i+=sq) Dq.pb(i);
    Dq.pb(((q/sq)+1)*sq);
    // cout << "DQ ";
    // for (int i : Dq)
    // {
    //     cout << i << ' ';
    // }
    // cout << endl;
    return Dq;
}

int main() {
  for (int i = 3; i <= 1024; i++) {
    auto dc = get_DC_num(i);

    // calculate difference set
    auto st = set<int>{};
    for (auto &x : dc) {
      for (auto &y : dc) {
        auto d = ((x - y) % i + i) % i;
        st.insert(d);
      }
    }

    if (i != st.size()) {
      for (int x = 0; x < i; x++) {
        if (st.count(x)) continue;
        std::cout << "[0, " << i << ")\n";
        std::cout << "dc:";
        for (auto &v : dc)
          std::cout << ' ' << v;
        std::cout << '\n';
        std::cout << "cannot find " << x << " in difference set\n";
        break;
      }
    }
  }
}
