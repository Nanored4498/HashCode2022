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
	array<vector<int>, NS> s2c;
	vector<vector<int>> c2s(C);

	for(int c = 0; c < C; ++c) {
		int n, l; // n=1
		string s;
		skill[c].fill(0);
		cin >> Cnames[c] >> n >> s >> l;
		if(!s2i.count(s)) s2i[s] = s2i.size();
		skill[c][s2i[s]] = l;
		ts[c] += l;
		s2c[s2i[s]].push_back(c);
		c2s[c].push_back(s2i[s]);
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
		int bestP = -1, bestlvlup = -1, bestEnd = -1;
		vector<int> bestCS;
		for(int p : ps) {
			int beste = -1, besto = -1;
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
					if(l) for(int c : s2c[s]) if(!chosen[c] && skill[c][s] >= l)
						if(best == -1 || (skill[c][s] <= l0 && skill[best][s] > l0) || ckey(c, mav, s, l0) < ckey(best, mav, s, l0))
							best = c;
					if(!l) for(int c = 0; c < C; ++c) if(!chosen[c])
						if(best == -1 || (skill[c][s] <= l0 && skill[best][s] > l0) || ckey(c, mav, s, l0) < ckey(best, mav, s, l0))
							best = c;
					if(best == -1) break;
					--nr;
					cs[o] = best;
					if(skill[best][s] <= l0) ++oo;
					for(int s : c2s[best]) ms[s] = max(ms[s], skill[best][s]);
					chosen[best] = true;
					mav = max(mav, av[best]);
				}
				for(int c : cs) chosen[c] = false;
				if(nr) continue;
				for(int i = 1; i < cs.size(); ++i) if(skill[cs[i]][req[p][i].first] > req[p][i].second)
					for(int j = 0; j < i; ++j) if(skill[cs[j]][req[p][j].first] > req[p][j].second)
						if(skill[cs[i]][req[p][j].first] >= req[p][j].second-1 && skill[cs[j]][req[p][i].first] >= req[p][i].second-1) {
							swap(cs[i], cs[j]);
							oo += 2;
						}
				if(oo > besto) {
					besto = oo;
					beste = mav+D[p];
					cs2 = cs;
				}
			}
			if(cs2.empty()) continue;
			int lvlup = 0;
			for(int i = 0; i < cs2.size(); ++i) if(skill[cs2[i]][req[p][i].first] <= req[p][i].second)
				lvlup += 1 + skill[cs2[i]][req[p][i].first];
			if(lvlup > bestlvlup) {
				bestlvlup = lvlup;
				bestP = p;
				bestCS = move(cs2);
				bestEnd = beste;
			}
		}
		if(bestP == -1) break;
		score += max(0, S[bestP] - max(0, bestEnd-B[bestP]));
		cerr << score << endl;
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
		ps.erase(bestP);
	}

	cerr << ps.size()<< endl;
	cout << sol.size() << endl;
	for(const auto &[p, cs] : sol) {
		cout << Pnames[p] << '\n';
		for(int c : cs) cout << Cnames[c] << ' ';
		cout << '\n';
	}

	return 0;
}