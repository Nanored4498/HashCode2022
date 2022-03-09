#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <numeric>
#include <algorithm>
#include <random>

using namespace std;
const int NS = 188;

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	int C, P;
	cin >> C >> P;
	vector<string> Cnames(C), Pnames(P);
	unordered_map<string, int> s2i;
	vector<array<int, NS>> skill(C);
	vector<int> ts(C, 0);
	vector<vector<pair<int, int>>> req(P);
	vector<int> D(P), S(P), B(P);

	double sn = 0;
	for(int c = 0; c < C; ++c) {
		int n; // n=1
		cin >> Cnames[c] >> n;
		skill[c].fill(0);
		string s; int l;
		cin >> s >> l;
		if(!s2i.count(s)) s2i[s] = s2i.size();
		skill[c][s2i[s]] = l;
		ts[c] += l;
	}
	cerr << C << ' ' << P << ' ' << s2i.size() << endl;
	for(int p = 0; p < P; ++p) {
		int r;
		cin >> Pnames[p] >> D[p] >> S[p] >> B[p] >> r;
		while(r--) {
			string s; int l;
			cin >> s >> l;
			if(!s2i.count(s)) s2i[s] = s2i.size();
			req[p].emplace_back(s2i[s], l);
		}
	}

	int score = 0;
	vector<pair<int, vector<int>>> sol;

	vector<int> av(C, 0);
	unordered_set<int> ps;
	vector<bool> chosen(C, false);
	for(int p = 0; p < P; ++p) ps.insert(p);
	mt19937 mt(42);
	const auto ckey = [&](int c, int m, int s, int l) { return make_tuple(max(av[c], m), -skill[c][s], ts[c]); };
	while(true) {
		int bestP = -1, bestlvlup = -1;
		vector<vector<int>> sols(P);
		vector<int> ends(P);
		for(int p : ps) {
			int beste, besto = -1;
			vector<int> cs(req[p].size(), 0), cs2;
			vector<int> ro(req[p].size());
			array<int, NS> ms;
			iota(ro.begin(), ro.end(), 0);
			for(int test = 0; test < 50; ++test) {
				shuffle(ro.begin(), ro.end(), mt);
				ms.fill(0);
				int mav = 0, oo = 0, nr = req[p].size();
				for(int o : ro) {
					auto [s, l] = req[p][o];
					const int l0 = l;
					if(ms[s] >= l) --l;
					int best = -1;
					for(int c = 0; c < C; ++c) if(!chosen[c] && skill[c][s] >= l)
						if(best == -1 || (skill[c][s] <= l0 && skill[best][s] > l0) || ckey(c, mav, s, l0) < ckey(best, mav, s, l0))
							best = c;
					if(best == -1) break;
					--nr;
					cs[o] = best;
					if(skill[best][s] <= l0) ++oo;
					for(int i = 0; i < s2i.size(); ++i) ms[i] = max(ms[i], skill[best][i]);
					chosen[best] = true;
					mav = max(mav, av[best]);
				}
				for(int c : cs) chosen[c] = false;
				int end = mav+D[p];
				if(nr || end >= B[p]+S[p]) continue;
				for(int i = 1; i < cs.size(); ++i) if(skill[cs[i]][req[p][i].first] > req[p][i].second)
					for(int j = 0; j < i; ++j) if(skill[cs[j]][req[p][j].first] > req[p][j].second)
						if(skill[cs[i]][req[p][j].first] >= req[p][j].second-1 && skill[cs[j]][req[p][i].first] >= req[p][i].second-1) {
							swap(cs[i], cs[j]);
							oo += 2;
						}
				if(oo > besto) {
					besto = oo;
					beste = end;
					cs2 = cs;
				}
			}
			int end = beste;
			if(cs2.empty()|| end >= B[p]+S[p]) {
				// if(end >= B[p]+S[p]) cerr << "TIME" << endl;
				continue;
			}
			cs = cs2;
			ends[p] = end;
			sols[p] = cs;
			int lvlup = 0;
			for(int i = 0; i < sols[p].size(); ++i) if(skill[sols[p][i]][req[p][i].first] <= req[p][i].second)
				lvlup += 1 + skill[sols[p][i]][req[p][i].first];
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