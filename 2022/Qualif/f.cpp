// Copyright (C) 2026, Coudert--Osmont Yoann
// SPDX-License-Identifier: MIT
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <numeric>
#include <ranges>
#include <set>
#include <unordered_map>
#include <vector>

using namespace std;
const int NS = 500;

template<typename Scal>
struct Hungarian {
	int N, M;
	vector<int> xy, yx;

	Hungarian(int N, int M):
		N(N), M(M),
		slackx(M), slack(M),
		_w(N*M) { }

	void solve() {
		xy.assign(N, -1);
		yx.assign(M, -1);
		lx.assign(N, numeric_limits<Scal>::max());
		ly.assign(M, (Scal) 0);
		for(int i = 0; i < N; ++i)
			for(int j = 0; j < M; ++j)
				lx[i] = min(lx[i], w(i, j));
		for(int i = 0; i < N; ++i) augment();
	}

	inline Scal& w(int i, int j) { return _w[i*M+j]; }

protected:
	vector<bool> S, T;
	vector<int> slackx;
	vector<Scal> lx, ly, slack, _w;

	bool add(int j) {
		T[j] = true;
		int i = yx[j];
		if(i == -1) {
			while(j >= 0) swap(j, xy[yx[j] = slackx[j]]);
			return true;
		}
		if(S[i]) return false;
		S[i] = true;
		for(int j2 = 0; j2 < M; ++j2) if(!T[j2]) {
			Scal new_slack = w(i, j2) - lx[i] - ly[j2];
			if(new_slack < slack[j2]) {
				slack[j2] = new_slack;
				slackx[j2] = i;
				if(new_slack == 0 && add(j2)) return true;
			}
		}
		return false;
	}

	void augment() {
		S.assign(N, false);
		T.assign(M, false);
		int i = 0;
		while(xy[i] != -1) ++i;
		S[i] = true;
		for(int j = 0; j < M; ++j) {
			slackx[j] = i;
			slack[j] = w(i, j) - lx[i] - ly[j];
		}
		while(true) {
			for(int j = 0; j < M; ++j)
				if(!T[j] && slack[j] == 0 && add(j))
					return;
			Scal delta = numeric_limits<Scal>::max();
			for(int j = 0; j < M; ++j) if(!T[j]) delta = min(delta, slack[j]);
			for(int i = 0; i < N; ++i) if(S[i]) lx[i] += delta;
			for(int j = 0; j < M; ++j)
				if(T[j]) ly[j] -= delta;
				else slack[j] -= delta;
		}
	}

};

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	int C, P;
	cin >> C >> P;
	cerr << C << ' ' << P << endl;
	vector<string> Cnames(C), Pnames(P);
	unordered_map<string, int> s2i;
	vector<array<int, NS>> skill(C);
	vector<int> ts(C, 0); // sum of skill levels
	vector<vector<pair<int, int>>> req(P);
	vector<int> D(P), S(P), B(P);
	array<vector<int>, NS> s2c;
	vector<vector<int>> c2s(C);

	for(int c = 0; c < C; ++c) {
		int n;
		cin >> Cnames[c] >> n;
		skill[c].fill(0);
		while(n--) {
			string s; int l;
			cin >> s >> l;
			if(!s2i.count(s)) s2i[s] = s2i.size();
			skill[c][s2i[s]] = l;
			ts[c] += l;
			s2c[s2i[s]].push_back(c);
			c2s[c].push_back(s2i[s]);
		}
	}
	vector<bool> sameR(P);
	for(int p = 0; p < P; ++p) {
		int r;
		cin >> Pnames[p] >> D[p] >> S[p] >> B[p] >> r;
		while(r--) {
			string s; int l;
			cin >> s >> l;
			if(!s2i.count(s)) s2i[s] = s2i.size();
			req[p].emplace_back(s2i[s], l);
		}
		set<pair<int, int>> sr(req[p].begin(), req[p].end());
		sameR[p] = sr.size() == 1 && req[p][0].second > 1;
	}
	if(s2i.size() != NS) {
		cerr << "Skill array too large: " << s2i.size() << endl;
		assert(s2i.size() <= NS);
	}

	int score = 0;
	vector<pair<int, vector<int>>> sol;

	vector<int> ps(P);
	vector<char> chosen(C, false);
	vector<int> av(C, 0), subR(C);
	iota(ps.begin(), ps.end(), 0);
	while(true) {
		for(int s = 0; s < NS; ++s) ranges::sort(s2c[s], {}, [&](int i) { return av[i]; });
		int bestP = -1, bestEnd = -1;
		double bestScore = -1.;
		vector<int> bestCS, cs, cs0;

		// Re-assignment of chosen contributors to roles maximinisng learning
		const auto re_assign = [&](const vector<pair<int, int>> &req, vector<int> &cs)->int {
			if(bestScore > 1000.) return 0;
			int nblvlup = 0;

			Hungarian<int> H(cs.size(), cs.size());
			for(int i = 0; i < (int) cs.size(); ++i)
				for(int j = 0; j < (int) cs.size(); ++j)
					H.w(i, j) = skill[cs[i]][req[j].first] < req[j].second-1 ? 10'000
						: (skill[cs[i]][req[j].first] <= req[j].second ? 1 : 2);
			H.solve();
			cs0.assign(cs.begin(), cs.end());
			for(int i = 0; i < (int) cs.size(); ++i) {
				const int j = H.xy[i];
				const int w = H.w(i, j);
				assert(w <= 2);
				if(w == 1) ++ nblvlup;
				cs[j] = cs0[i];
			}

			return nblvlup;
		};

		for(int p : ps) {
			int sav = 0, mav = 0;

			if(sameR[p]) {
				const auto [s, l] = req[p][0];
				const int r = req[p].size();
				int N = 0;
				for(int c : s2c[s]) if(skill[c][s] >= l-1) subR[N++] = c;
				if(N < r) continue;
				int lastL = -1, i = 0, bestI = -1, bestSAV = -1;
				double best = -1.;
				for(; i < r-1; ++i) {
					sav += av[subR[i]];
					if(skill[subR[i]][s] >= l) lastL = i;
				}
				for(; i < N; ++i) {
					sav += av[subR[i]];
					if(skill[subR[i]][s] >= l) lastL = i;
					if(i-lastL < r) {
						const int e = av[subR[i]] + D[p];
						const int t = r*e-sav;
						const double sc = double(max(0, S[p] - max(0, e-B[p]))) / double(t);
						if(sc > best) {
							best = sc;
							bestI = i;
							bestSAV = sav;
						}
					}
					sav -= av[subR[i-r+1]];
				}
				if(bestI == -1) continue;
				cs.assign(subR.begin()+bestI-r+1, subR.begin()+bestI+1);
				mav = av[subR[bestI]];
				sav = bestSAV;
			} else {
				cs.clear();
				array<int, NS> ms; ms.fill(0);
				for(const auto [s, l0] : req[p]) {
					int l = l0, best;
					if(ms[s] >= l) --l;
					const auto ckey = [&](int c) { return make_tuple(max(av[c], mav), -min(av[c], mav), skill[c][s], ts[c]); };
					const auto choose = [&](auto &&choices) {
						const auto it = ranges::min_element(choices, {}, ckey);
						best = it == choices.end() ? -1 : *it;
					};
					if(l) choose(s2c[s] | views::filter([&](int c) { return !chosen[c] && skill[c][s] >= l; }));
					else choose(views::iota(0, C) | views::filter([&](int c) { return !chosen[c]; }));
					if(best == -1) break;
					cs.push_back(best);
					for(int s : c2s[best]) ms[s] = max(ms[s], skill[best][s]);
					chosen[best] = true;
					sav += av[best];
					mav = max(mav, av[best]);
				}
				for(int c : cs) chosen[c] = false;
				if(cs.size() < req[p].size()) continue;
			}

			// compute score
			int end = mav+D[p];
			double time = cs.size()*end - sav;
			int sco = max(0, S[p] - max(0, end-B[p]));
			double score = sco ?
				1000. + double(sco) / time
				: double(re_assign(req[p], cs)) / time;
			if(score > bestScore) {
				bestScore = score;
				bestEnd = end;
				bestCS = move(cs);
				bestP = p;
			}
		}
		if(bestP == -1) break;
		score += max(0, S[bestP] - max(0, bestEnd-B[bestP]));
		cerr << score << endl;
		if(!sameR[bestP]) {
			bestScore = 0.;
			re_assign(req[bestP], bestCS);
		}
		for(int i = 0; i < bestCS.size(); ++i) {
			av[bestCS[i]] = bestEnd;
			auto [s, l] = req[bestP][i];
			if(skill[bestCS[i]][s] <= l) {
				if(!skill[bestCS[i]][s]) {
					s2c[s].push_back(bestCS[i]);
					c2s[bestCS[i]].push_back(s);
				}
				++ skill[bestCS[i]][s];
				++ ts[bestCS[i]];
			}
		}
		sol.emplace_back(bestP, bestCS);
		*ranges::find(ps, bestP) = ps.back();
		ps.pop_back();
		if(sol.size() > 3200) break;
	}

	cerr << score << endl;
	cout << sol.size() << endl;
	for(const auto &[p, cs] : sol) {
		cout << Pnames[p] << '\n';
		for(int c : cs) cout << Cnames[c] << ' ';
		cout << '\n';
	}

	return 0;
}