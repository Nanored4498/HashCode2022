#include "common.h"
#include <fstream>

int main(int argc, char* argv[]) {
	ifstream in(argv[1]);
	ifstream out(argv[2]);

	int D, I, S, V, F;
	in >> D >> I >> S >> V >> F;
	dbg(D, I, S, V, F);

	vector<Street> ss(S);
	unordered_map<string, int> s2i;
	for(int i = 0; i < S; ++i) {
		Street &s = ss[i];
		in >> s.a >> s.b >> s.name >> s.l;
		s2i[s.name] = i;
	}
	vector<vi> vs(V);
	for(vi &v : vs) {
		int p; in >> p;
		v.resize(p);
		for(int &a : v) {
			string s; in >> s;
			a = s2i[s];
		}
	}

	vector<vii> ans(I);
	int best_score = 0;
	vector<vi> green(I, vi(D+1, -1));
	vector<bool> available(S, false);
	vector<tuple<int, int, vii>> modifs;
	vector<tuple<int, int, int>> swaps0, swaps;

	int A; out >> A;
	while(A--) {
		int i, e; out >> i >> e;
		for(int j = 0; j < e; ++j) for(int k = 0; k < j; ++k) swaps0.emplace_back(i, j, k);
		while(e--) {
			string s; int t;
			out >> s >> t;
			ans[i].emplace_back(s2i[s], t);
			available[ans[i].back().first] = true;
		}
	}

	in.close();
	out.close();
	mt19937_64 mt;
	vector<vii> ar(I);
	vector<vi> ars(S);
	vector<tuple<int, int, int>> mvbck;
	bool rollbck = false;
	int lasti; vii lastai;

	auto compG = [&](int i) {
		int m = 0;
		for(auto [s, t] : ans[i]) for(int k = 0; k < t; ++k) green[i][m++] = s;
		for(int t = m; t <= D; ++t) green[i][t] = green[i][t%m];
	};
	vector<vii> Qs(D+1);

	while(true) {
		for(int v = V-1; v >= 0; --v) Qs[0].emplace_back(v, 0);
		for(int i = 0; i < I; ++i) if(!ans[i].empty()) {
			if(!rollbck) compG(i);
			ar[i].clear();
		}
		rollbck = false;

		// Compute score
		int score = 0;
		mvbck.clear();
		for(int t0 = 0; t0 <= D; ++t0) while(!Qs[t0].empty()) {
			auto [v, i] = Qs[t0].back(); Qs[t0].pop_back();
			if(i == vs[v].size()-1) {
				score += F + D - t0;
				continue;
			}
			int s = vs[v][i], t = t0;
			if(available[s]) ar[ss[s].b].emplace_back(t, s);
			while(t <= D && green[ss[s].b][t] != s) ++t;
			if(t + ss[vs[v][i+1]].l <= D) {
				mvbck.emplace_back(ss[s].b, t, s);
				green[ss[s].b][t] = -1;
				s = vs[v][++i];
				Qs[t + ss[s].l].emplace_back(v, i);
			}
		}

		if(score > best_score) {
			best_score = score;
			dbg("NEW BEST:", best_score);
			// Output solution
			ofstream oo(argv[2]);
			int A = 0;
			for(vii &v : ans) if(!v.empty()) ++ A;
			oo << A << '\n';
			for(int i = 0; i < I; ++i) if(!ans[i].empty()) {
				oo << i << '\n' << ans[i].size() << '\n';
				for(auto [s, t] : ans[i]) oo << ss[s].name << ' ' << t << '\n';
			}
			oo.close();
		} else {
			for(auto [i, t, s] : mvbck) green[i][t] = s;
			rollbck = true;
			ans[lasti] = lastai;
			compG(lasti);
			if(!modifs.empty()) {
				lasti = get<1>(modifs.back());
				lastai = ans[lasti];
				ans[lasti] = get<2>(modifs.back());
				modifs.pop_back();
			} else if(!swaps.empty()) {
				auto [i, j, k] = swaps.back();
				swaps.pop_back();
				lasti = i;
				lastai = ans[i];
				swap(ans[i][j], ans[i][k]);
			} else break;
			compG(lasti);
			continue;
		}

		int base[4000], X = -2;
		auto compW = [&](int i, const vii &a) {
			int m = 0, w = 0;
			--X;
			for(auto [s, t] : a) for(int k = 0; k < t; ++k) base[m++] = s;
			for(auto [t, s] : ar[i]) {
				while(t <= D && (green[i][t] == X || base[t%m] != s)) ++t, ++w;
				if(t <= D) green[i][t] = X; else w += F;
			}
			return w;
		};
		modifs.clear();
		swaps = swaps0;
		for(int i = 0; i < I; ++i) if(ans[i].size() > 1) {
			int cw = compW(i, ans[i]), bw = cw;
			vii a = ans[i], ba = a;
			if(ans[i].size() < 9) {
				sort(a.begin(), a.end());
				do {
					int w = compW(i, a);
					if(w < bw) {
						bw = w;
						ba = a;
					}
				} while(next_permutation(a.begin(), a.end()));
			} else {
				bool bad = false;
				for(auto [s, t] : ans[i]) {
					if(t != 1) bad = true;
					ars[s].clear();
				}
				if(bad) continue;
				int m = ans[i].size();
				vi x(m);
				for(int j = 0; j < m; ++j) x[j] = ans[i][j].first;
				for(auto [t, s] : ar[i]) ars[s].push_back(t);
				for(int step = 0; step < 100; ++step) {
					int w = 0;
					shuffle(x.begin(), x.end(), mt);
					a.assign(m, pii{-1, 1});
					for(int s : x) {
						int bj = 0, bjw = 1e9;
						for(int j = 0; j < m; ++j) if(a[j].first == -1) {
							-- X;
							int jw = 0;
							for(int t : ars[s]) {
								int of = (j - t) % m;
								if(of < 0) of += m;
								t += of;
								jw += of;
								while(t <= D && green[i][t] == X) t += m, jw += m;
								if(t <= D) green[i][t] = X; else jw += F;
							}
							if(jw < bjw) {
								bjw = jw;
								bj = j;
							}
						}
						w += bjw;
						a[bj].first = s;
					}
					if(w < bw) {
						bw = w;
						ba = a;
					}
				}
			}
			int diff = cw - bw;
			if(diff > 0) modifs.emplace_back(diff, i, ba);			
		}
		sort(modifs.begin(), modifs.end());
		shuffle(swaps.begin(), swaps.end(), mt);
		if(!modifs.empty()) {
			lasti = get<1>(modifs.back());
			lastai = ans[lasti];
			ans[lasti] = get<2>(modifs.back());
			modifs.pop_back();
		} else if(!swaps.empty()) {
			auto [i, j, k] = swaps.back();
			lasti = i;
			lastai = ans[i];
			swap(ans[i][j], ans[i][k]);
			swaps.pop_back();
		} else break;
	}
	
	return 0;
}