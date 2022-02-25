#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <numeric>
#include <algorithm>
#include <random>

using namespace std;

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	int C, P;
	cin >> C >> P;
	vector<string> Cnames(C), Pnames(P);
	unordered_map<string, int> s2i;
	vector<array<int, 800>> skill(C);
	vector<int> ts(C, 0);
	vector<vector<pair<int, int>>> req(P);
	vector<int> D(P), S(P), B(P);

	double sn = 0;
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
		}
		sn += ts[c];
	}
	cerr << sn/C << ' ' << C << ' ' << P << ' ' << s2i.size() << endl;
	int sr = 0;
	for(int p = 0; p < P; ++p) {
		int r;
		cin >> Pnames[p] >> D[p] >> S[p] >> B[p] >> r;
		sr += r;
		while(r--) {
			string s; int l;
			cin >> s >> l;
			if(!s2i.count(s)) s2i[s] = s2i.size();
			req[p].emplace_back(s2i[s], l);
		}
	}
	cerr << sr/P << endl;

	// vector<bool> okc(C, false);
	// for(int c = 0; c < C; ++c) {
	// 	bool ok = false;
	// 	for(int s = 0; s < s2i.size(); ++s) if(skill[c][s]) {
	// 		bool oui = true;
	// 		for(int c2 = 0; c2 < C; ++c2) if(skill[c2][s] > skill[c][s]) oui = false;
	// 		if(oui) ok = true;
	// 	}
	// 	okc[c] = ok;
	// } 

	int score = 0;
	vector<pair<int, vector<int>>> sol;

	vector<int> av(C, 0);
	unordered_set<int> ps;
	vector<bool> chosen(C, false);
	for(int p = 0; p < P; ++p) ps.insert(p);
	mt19937 mt(42);
	const auto ckey = [&](int c, int m, int s, int l) { return make_tuple(max(av[c], m), -skill[c][s], ts[c]); };
	while(true) {
		int bestP = -1;
		int bestlvlup = -1;
		vector<vector<int>> sols(P);
		vector<int> ends(P);
		for(int p : ps) {
			int mav = 0, besto = -1;
			vector<int> cs(req[p].size(), 0), cs2;
			vector<int> ro(req[p].size());
			iota(ro.begin(), ro.end(), 0);
			bool good = false;
			for(int test = 0; test < 50; ++test) {
				shuffle(ro.begin(), ro.end(), mt);
				array<int, 800> ms; ms.fill(0);
				mav = 0;
				good = true;
				int oo = 0;
				for(int o : ro) {
					auto [s, l] = req[p][o];
					int l0 = l;
					if(ms[s] >= l) --l;
					int best = -1;
					for(int c = 0; c < C; ++c) if(!chosen[c] && skill[c][s] >= l)
						if(best == -1 || (skill[c][s] <= l0 && skill[best][s] > l0) || ckey(c, mav, s, l0) < ckey(best, mav, s, l0))
							best = c;
					if(best == -1) {
						good = false;
						break;
					}
					cs[o] = best;
					if(skill[best][s] <= l0) ++oo;
					for(int i = 0; i < s2i.size(); ++i) ms[i] = max(ms[i], skill[best][i]);
					chosen[best] = true;
					mav = max(mav, av[best]);
				}
				for(int c : cs) chosen[c] = false;
				int end = mav+D[p];
				if(!good || end >= B[p]+S[p]) continue;
				if(oo > besto) {
					besto = oo;
					cs2 = cs;
				}
			}
			int end = mav+D[p];
			if(cs2.empty()|| end >= B[p]+S[p]) {
				if(end >= B[p]+S[p]) cerr << "TIME" << endl;
				continue;
			}
			cs = cs2;
			ends[p] = end;
			sols[p] = cs;
			int lvlup = 0;
			for(int i = 0; i < sols[p].size(); ++i) if(skill[sols[p][i]][req[p][i].first] <= req[p][i].second) lvlup += 1 + skill[sols[p][i]][req[p][i].first];
			if(lvlup > bestlvlup) {
				bestlvlup = lvlup;
				bestP = p;
			}
		}
		if(bestP == -1) break;
		score += S[bestP] - max(0, ends[bestP]-B[bestP]);
		for(int i = 0; i < sols[bestP].size(); ++i) {
			av[sols[bestP][i]] = ends[bestP];
			auto [s, l] = req[bestP][i];
			if(skill[sols[bestP][i]][s] <= l) {
				++ skill[sols[bestP][i]][s];
				++ ts[sols[bestP][i]];
			}
		}
		sol.emplace_back(bestP, sols[bestP]);
		ps.erase(bestP);
	}

	cerr << ps.size()<< endl;
	cerr << score << endl;
	cout << sol.size() << endl;
	for(const auto &[p, cs] : sol) {
		cout << Pnames[p] << '\n';
		for(int c : cs) cout << Cnames[c] << ' ';
		cout << '\n';
	}

	return 0;
}