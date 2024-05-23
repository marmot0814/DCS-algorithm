#pragma once

#include <bits/stdc++.h>
using namespace std;

#define ALL(x) (x).begin(), (x).end()
#define PB push_back
using VI = vector<int>;
#define NX(x) ((x+k)%n)
#define C(a,b) (V[a]==V[b]?V[NX(a)]<V[NX(b)]:V[a]<V[b])
#define go(XD) V2[sa[0]]=N=0; \
  for(int i=1;i<n;i++) N+=XD, V2[sa[i]]=N; V=V2;
#define RSP(V) sa,n,N+1,V
void RS(auto &sa, int n, int N, auto &V) {
  VI B(N), t(n); for (auto &v : sa) B[V[v]]++;
  partial_sum(ALL(B), B.begin()); int i = n;
  while (i--) t[--B[V[sa[i]]]]=sa[i]; sa=t;
}
VI SA_nlgn(string s) { s+='$'; int n=s.size(), k=0, N=255;
  VI sa(n), V(n), V2(n); iota(ALL(sa), 0);
  RS(RSP(s)); go(s[sa[i-1]]!=s[sa[i]]);
  for (int k=1;N+1<n;k<<=1,k%=n) { for (auto &v : sa)
    (v+=n-k)%=n; RS(RSP(V));
    go(C(sa[i-1],sa[i])||C(sa[i],sa[i-1]));
  } return sa;
}

