#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>

using namespace std;
typedef vector<int> vi;
typedef pair<int,int> pii;

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	int L, G, S, B, F, N;
	cin >> L >> G >> S >> B >> F >>N;

	vector<string> Snames(S);
	unordered_map<string, int> s2i;
	vi s2b(S);
	vector<unordered_set<int>> b2s(B);
	for(int s = 0; s < S; ++s) {
		cin >> Snames[s] >> s2b[s];
		s2i[Snames[s]] = s;
		b2s[s2b[s]].insert(s);
	}

	vector<string> Fnames(F);
	unordered_map<string, int> f2i;
	vector<vi> f2s(F);
	vector<unordered_set<int>> s2f(S);
	vi D(F), U(F);
	for(int f = 0; f < F; ++f) {
		int M;
		cin >> Fnames[f] >> M >> D[f] >> U[f];
		f2i[Fnames[f]] = f;
		f2s[f].resize(M);
		while(M--) {
			string s;
			cin >> s;
			f2s[f][M] = s2i[s];
			s2f[s2i[s]].insert(f);
		}
	}

	int score = 0;
	priority_queue<pii, vector<pii>, greater<pii>> GQ;
	for(int g = 0; g < G; ++g) GQ.emplace(0, g);
	vector<vi> C(B, vi(L, 0));
	vector<bool> used(B, false);

	unordered_set<int> fs;
	for(int f = 0; f < F; ++f) fs.emplace_hint(fs.end(), f);
	
	while(!fs.empty()) {
		int bestF = -1;
		double best = -1;
		int bscore = -1;
		vector<tuple<int, int, int>> bmodif;
		priority_queue<pii, vector<pii>, greater<pii>> GQ2 = GQ, bGQ;
		for(int f : fs) {
			GQ = GQ2;
			int time = 0;
			int dt = 0, dt2 = 0;
			vector<tuple<int, int, int>> modif;
			bool bad = false;
			for(int s : f2s[f]) {
				int b = s2b[s];
				if(used[b]) continue;
				auto [t1, g] = GQ.top();
				dt -= t1;
				for(int t = t1+1; t+D[f]+b2s[b].size() <= L; ++t)
					if(t+C[b][t] < t1+C[b][t1])
						t1 = t;
				int t2 = t1 + D[f] + b2s[b].size() + C[b][t1];
				if(t2 >= L) {
					bad = true;
					break;
				}
				dt += t2;
				dt2 += t2-t1;
				modif.emplace_back(b, t1, t2);
				GQ.pop();
				GQ.emplace(t2, g);
				time = max(time, t2);
				used[b] = true;
			}
			for(auto [b, t1, t2] : modif) used[b] = false;
			if(bad) continue;
			int sc = U[f]*max(0, L-time);
			double sct = 500*double(sc)/double(dt)+100*double(sc)/double(dt2)+double(sc);
			if(sct > best) {
				best = sct;
				bscore = sc;
				bestF = f;
				bmodif = move(modif);
				bGQ = move(GQ);
			}
		}
		if(bestF == -1) break;
		for(auto [b, t1, t2] : bmodif)
			for(int t = t1; t < t2; ++t) ++ C[b][t];
		score += bscore;
		GQ = move(bGQ);
		fs.erase(bestF);
	}

	cerr << score << endl;

	return 0;
}