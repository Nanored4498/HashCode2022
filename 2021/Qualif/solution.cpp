#include <iostream>
#include <vector>
#include <unordered_map>
#include <numeric>
#include <queue>
#include <cmath>
#include <algorithm>

using namespace std;
using vi = vector<int>;
using pii = pair<int, int>;
using vii = vector<pii>;

bool use_dbg = false;
template<typename A, typename B>
ostream& operator<<(ostream &out, const pair<A, B> &p) {
	return out << '(' << p.first << ", " << p.second << ')';
}
template<typename T>
ostream& operator<<(ostream &out, const vector<T> &v) {
	out << '[';
	for(int i = 0; i < (int)v.size(); ++i) {
		if(i) out << ", ";
		out << v[i];
	}
	return out << ']';
}
void dbg() {
	if(use_dbg) cerr << endl;
}
template<typename T, typename... Ts>
void dbg(const T &x, Ts... vals) {
	if(use_dbg) {
		cerr << x << ' ';
		dbg(vals...);
	};
}

struct Street {
	int a, b, l;
	string name;
};

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	int D, I, S, V, F;
	cin >> D >> I >> S >> V >> F;
	dbg(D, I, S, V, F);

	vector<Street> ss(S);
	unordered_map<string, int> s2i;
	for(int i = 0; i < S; ++i) {
		Street &s = ss[i];
		cin >> s.a >> s.b >> s.name >> s.l;
		s2i[s.name] = i;
	}
	vector<vi> vs(V);
	for(vi &v : vs) {
		int p; cin >> p;
		v.resize(p);
		for(int &a : v) {
			string s; cin >> s;
			a = s2i[s];
		}
	}

	vector<vii> bestAns;
	int bestScore = 0;
	vector<bool> okv(V, true);
	while(true) {
		vector<vii> ans(I);

		using QEl = tuple<int, int, int>;
		priority_queue<QEl, vector<QEl>, greater<QEl>> Q;
		vector<int> rl(V);
		vi countS(S, 0);
		vector<vi> green(I, vi(D+1, -1));
		for(int v = 0; v < V; ++v) if(okv[v]) {
			int L = 0;
			for(int i = 1; i < (int)vs[v].size(); ++i) L += ss[vs[v][i]].l;
			rl[v] = L;
			for(int i = 0; i+1 < (int)vs[v].size(); ++i) ++countS[vs[v][i]];
			Q.emplace(0, v, 0);
		}

		vector<vi> i2s(I);
		vi its(V, 0);
		vi countI(I, 0);
		for(int s = 0; s < S; ++s) if(countS[s]) {
			i2s[ss[s].b].push_back(s);
			countI[ss[s].b] += countS[s];
		}
		for(int v = 0; v < V; ++v) if(okv[v]) {
			for(int i = 0; i+1 < (int)vs[v].size(); ++i) if(countS[vs[v][i]] == 1) {
				its[v] += countI[ss[vs[v][i]].b] - 1;
			}
		}

		vi special(S, 0);
		for(int i = 0; i < I; ++i) {
			ans[i].assign(i2s[i].size(), pii{-1, 1});
			if(i2s[i].size() == 2) {
				int s0 = i2s[i][0];
				int s1 = i2s[i][1];
				if(countS[s0] > countS[s1]) swap(s0, s1);
				if(countS[s0] > 1) continue;
				if(countS[s1] < 2) continue;
				ans[i][0].first = s1;
				ans[i][1].first = s0;
				swap(countS[s1], special[s0]);
				green[i].assign(D+1, s1);
			}
		}

		// Compute score
		int score = 0;
		int nvg = 0, tw = 0, mrt = 0, rv = -1, mit = -1;
		while(!Q.empty()) {
			auto [t, v, i] = Q.top(); Q.pop();
			if(i == vs[v].size()-1) {
				score += F + D - t;
				if(mrt == 0 && its[v]/2+t > mit) {
					mit = its[v]/2+t;
					rv = v;
				}
				++ nvg;
				continue;
			}
			int s = vs[v][i];
			if(countS[s]) {
				int b = ss[s].b;
				if(special[s]) {
					ans[b][0].second = max({1, t, (int)sqrt(special[s])-1});
					for(int u = ans[b][0].second; u <= D; u += ans[b][0].second+1) green[b][u] = s;
				} else {
					int m = ans[b].size();
					int j = t % m;
					while(ans[b][j].first != -1) j = (j+1)%m;
					ans[b][j].first = s;
					for(int u = j; u <= D; u += m) green[b][u] = s;
				}
				countS[s] = 0;
			}
			while(t <= D && green[ss[s].b][t] != s) ++t, ++tw;
			if(t+rl[v] <= D) {
				green[ss[s].b][t] = -1;
				s = vs[v][++i];
				rl[v] -= ss[s].l;
				Q.emplace(t + ss[s].l, v, i);
			} else {
				int rt = rl[v]+t-D;
				for(; i+1 < (int)vs[v].size(); ++i) rt += (ans[ss[vs[v][i]].b].size()-1)/2;
				if(rt > mrt) {
					mrt = rt;
					rv = v;
				}
			}
		}
		dbg(nvg, tw, (V-nvg)*F);
		dbg(score);

		if(score > bestScore) {
			bestScore = score;
			for(int s = 0; s < S; ++s) if(countS[s]) {
				int b = ss[s].b;
				if(special[s]) {
					ans[b].pop_back();
					ans[b][0].second = 1;
				} else {
					int i = 0; while(ans[b][i].first != -1) ++i;
					ans[b][i].first = s;
				}
			}
			bestAns = ans;
		}

		if(rv == -1 || score < .99*bestScore) break;
		okv[rv] = false;
	}
	
	// Output solution
	cerr << bestScore << endl;
	int A = 0;
	for(vii &v : bestAns) if(!v.empty()) ++ A;
	cout << A << '\n';
	for(int i = 0; i < I; ++i) if(!bestAns[i].empty()) {
		cout << i << '\n' << bestAns[i].size() << '\n';
		for(auto [s, t] : bestAns[i]) cout << ss[s].name << ' ' << t << '\n';
	}
	return 0;
}