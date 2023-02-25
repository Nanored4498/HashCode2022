#include "common.h"

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
	int bestScore = -1;
	vector<bool> okv(V, true);
	for(int step = 0; step < 140; ++step) {
		vector<vii> ans(I);

		using QEl = tuple<int, int, int>;
		priority_queue<QEl, vector<QEl>, greater<QEl>> Q;
		vector<int> rl(V);
		vi countS(S, 0);
		vector<vi> green(I, vi(D+1, -1));
		for(int v = 0; v < V; ++v) {
			for(int i = 1; i < (int)vs[v].size(); ++i) rl[v] += ss[vs[v][i]].l;
			if(okv[v]) for(int i = 0; i+1 < (int)vs[v].size(); ++i) ++countS[vs[v][i]];
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

		mt19937 rnd(1);
		for(int i = 0; i < I; ++i) if(!i2s[i].empty()) {
			int m = 0;
			shuffle(i2s[i].begin(), i2s[i].end(), rnd);
			for(int s : i2s[i]) {
				ans[i].emplace_back(s, max(1, countS[s]/20));
				for(int k = 0; k < ans[i].back().second; ++k) green[i][m++] = s;
			}
			for(int t = m; t <= D; ++t) green[i][t] = green[i][t%m];
		}

		// Compute score
		int score = 0;
		int nvg = 0, tw = 0, mrt = 0, rv = -1, mit = -1;
		while(!Q.empty()) {
			auto [t, v, i] = Q.top(); Q.pop();
			if(i == vs[v].size()-1) {
				score += F + D - t;
				if(okv[v] && mrt == 0 && its[v]/2+t > mit) {
					mit = its[v]/2+t;
					rv = v;
				}
				++ nvg;
				continue;
			}
			int s = vs[v][i];
			while(t <= D && green[ss[s].b][t] != s) ++t, ++tw;
			if(t + ss[vs[v][i+1]].l <= D) {
				green[ss[s].b][t] = -1;
				s = vs[v][++i];
				rl[v] -= ss[s].l;
				Q.emplace(t + ss[s].l, v, i);
			} else if(okv[v]) {
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