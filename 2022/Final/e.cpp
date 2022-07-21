#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <queue>
#include <functional>
#include <cmath>

using namespace std;
typedef vector<int> vi;
typedef pair<int, int> pii;
typedef long long ll;

template<typename T>
ostream& print(ostream& out, const T &v) {
	out << '[';
	for(const auto &x : v) out << ' ' << x << ',';
	return out << ']';
}
ostream& operator<<(ostream& out, const vi &v) { return print(out, v); }

struct Gift {
	string name;
	int s, w, c, r;
	bool used = false;
};

unordered_map<string, string> opp;

struct St {
	int DD, NEG, POS, NN;
	vi pred, dist, car;
	int p2i(int x, int v) { return x + DD*(v+NEG); };
	pii i2p(int i) { return make_pair((i%DD), (i/DD)-NEG); };
	St() = default;
	St(int DD, int AA): DD(DD), NEG(3*AA), POS(sqrt(DD*AA)+1), NN(DD*(NEG+POS)), pred(NN), dist(NN, 1e9), car(NN, -1) {
		dist[p2i(0, 0)] = 0;
		queue<int> Q; Q.push(p2i(0, 0));
		while(!Q.empty()) {
			const auto [x, v] = i2p(Q.front()); Q.pop();
			const int d = dist[p2i(x, v)] + 1;
			for(int a = -AA; a <= AA; ++a) {
				const int w = v+a;
				if(w < -NEG || w >= POS) continue;
				const int y = x+w;
				if(y < 0 || y >= DD) continue;
				const int j = p2i(y, w);
				if(dist[j] <= d) continue;
				dist[j] = d;
				pred[j] = a;
				Q.push(j);
			}
		}
	 	car[p2i(0, 0)] = 0;
	}
	int getD(int x) { return dist[p2i(abs(x), 0)]; };
	void move0(int x, const string &op, vector<string> &a) {
		vector<string> b;
		int i = p2i(abs(x), 0);
		while(i != p2i(0, 0)) {
			b.push_back("Float 1");
			if(pred[i] > 0) b.push_back(op + " " + to_string(pred[i]));
			else if(pred[i] < 0) b.push_back(opp[op] + " " + to_string(-pred[i]));
			auto [x, v] = i2p(i);
			i = p2i(x-v, v-pred[i]);
		}
		a.insert(a.end(), b.rbegin(), b.rend());
	};
	void move1(int x, int y, vector<string> &a) {
		if(x > 0) move0(x, "AccRight", a);
		else if(x < 0) move0(x, "AccLeft", a);
		if(y > 0) move0(y, "AccUp", a);
		else if(y < 0) move0(y, "AccDown", a);
	};
	void move2(int x, int y, vector<string> &a) {
		if(y > 0) move0(y, "AccUp", a);
		else if(y < 0) move0(y, "AccDown", a);
		if(x > 0) move0(x, "AccRight", a);
		else if(x < 0) move0(x, "AccLeft", a);
	};
	int getC(int x, int v) {
		x = abs(x);
		const int i = p2i(x, v);
		if(car[i] == -1) {
			const int add = pred[i]==0 ? 0 : 1;
			car[i] = getC(x-v, v-pred[i]) + add;
		}
		return car[i];
	};
};

// Assume Float parameter is 1
vector<pii> path(int x, int y, const vector<string> &a) {
	vector<pii> ans(1, {x, y});
	int vx = 0, vy = 0;
	for(const string &s : a) {
		if(s[0] == 'F') {
			ans.emplace_back(x += vx, y += vy);
		} else {
			if(s[3] == 'R') vx += stoi(s.substr(9));
			else if(s[3] == 'L') vx -= stoi(s.substr(8));
			else if(s[3] == 'U') vy += stoi(s.substr(6));
			else vy -= stoi(s.substr(8));
		}
	}
	return ans;
} 

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);
	opp["AccRight"] = "AccLeft";
	opp["AccLeft"] = "AccRight";
	opp["AccUp"] = "AccDown";
	opp["AccDown"] = "AccUp";

	int T, D, W, G;
	cin >> T >> D >> W >> G;
	vi l(W), aa(W);
	vector<Gift> gs(G);
	for(int i = 0; i < W; ++i) cin >> l[i] >> aa[i];
	for(int i = 0; i < G; ++i) cin >> gs[i].name >> gs[i].s >> gs[i].w >> gs[i].c >> gs[i].r;

	int mic=0, mac=0;
	for(int i = 0; i < G; ++i) {
		cin >> gs[i].name >> gs[i].s >> gs[i].w >> gs[i].c >> gs[i].r;
		mic = min({mic, gs[i].c, gs[i].r});
		mac = max({mac, gs[i].c, gs[i].r});
	}
	mic -= D;
	mac += D;

	const int P0 = 3;
	const int DD = mac-mic+1;
	vector<St> sts(W);
	for(int i = 0; i < W; ++i) sts[i] = St(DD, aa[i]);
	const double w_coeff = 2.*sts[0].getD(DD/4)/double(l[P0]);
	cerr << "Precompute done" << endl;

	int score = 0, delivered = 0, goback = 0;
	int time = 0;
	int caro = 0, ww = 0;
	int x = 0, y = 0;
	vector<string> ans, tmp, gt;
	while(true) {
		int P = 0; while(ww < l[P0]-l[P]) ++P;
		vector<vector<vi>> gi(DD, vector<vi>(DD));
		for(int i = 0; i < gs.size(); ++i) if(!gs[i].used) {
			for(int dx = -D; dx <= D; ++dx) {
				for(int dy = -D; dy <= D; ++dy) {
					if(dx*dx+dy*dy > D*D) continue;
					gi[gs[i].c+dx-mic][gs[i].r+dy-mic].push_back(i);
				}
			}
		}
		double best_score = -1;
		int xx=0, yy=0;
		bool mv2 = false;
		vi gib;
		#pragma omp parallel for
		for(int i = 0; i < DD; ++i) {
			for(int j = 0; j < DD; ++j) {
				if(gi[i][j].empty()) continue;
				int d = sts[P].getD(i+mic-x) + sts[P].getD(j+mic-y);
				if(time+d > T) continue;
				const int x2 = i+mic, y2 = j+mic;
				int w = l[P0]-ww-sts[P].getC(x2-x, 0)-sts[P].getC(y2-y, 0)-sts[0].getC(x2, 0)-sts[0].getC(y2, 0);
				if(w <= 0) continue;
				for(bool mm : {false, true}) {
					vector<string> pth0;
					if(mm) sts[P].move2(x2-x, y2-y, pth0);
					else sts[P].move1(x2-x, y2-y, pth0);
					vi ind0; for(const auto [u, v] : path(x, y, pth0)) ind0.insert(ind0.end(), gi[u-mic][v-mic].begin(), gi[u-mic][v-mic].end());
					sort(ind0.begin(), ind0.end());
					ind0.erase(unique(ind0.begin(), ind0.end()), ind0.end());
					sort(ind0.begin(), ind0.end(), [&](int i, int j) { return double(gs[i].s)/double(gs[i].w) > double(gs[j].s)/double(gs[j].w); });
					vi ind; int q = w;
					double sc = 0.;
					for(int i : ind0) if(gs[i].w <= q) { ind.push_back(i); q -= gs[i].w; sc += gs[i].s; }
					sc /= (d + w_coeff * (w-q));
					#pragma omp critical
					if(sc > best_score) {
						best_score = sc;
						xx = x2;
						yy = y2;
						gib = move(ind);
						mv2 = mm;
					}
				}
			}
		}
		if(best_score < 0 || (ww+600>l[P0] && T*best_score < 1.5e6)) {
			if(!ww) break;
			int t = 1e9;
			for(int x2 = -D; x2 <= D; ++x2) {
				for(int y2 = -D; y2 <= D; ++y2) {
					if(x2*x2+y2*y2 > D*D) continue;
					int t0 = sts[0].getD(x2-x) + sts[0].getD(y2-y);
					if(t0 < t) {
						t = t0;
						xx = x2;
						yy = y2;
					}
				}
			}
			bool bad = false;
			if(time+t <= T) {
				caro += sts[0].getC(xx-x, 0) + sts[0].getC(yy-y, 0);
				sts[0].move2(xx-x, yy-y, tmp);
				time += t;
				cerr << "GOBACK: " << ++ goback << endl;
			} else bad = true;
			ans.push_back("LoadCarrots " + to_string(caro));
			for(const string &s : gt) ans.push_back("LoadGift " + s);
			ans.insert(ans.end(), tmp.begin(), tmp.end());
			if(bad) break;
			caro = 0;
			ww = 0;
			tmp.clear();
			gt.clear();
			x = xx; y = yy;
			continue;
		}
		int t = sts[P].getD(xx-x) + sts[P].getD(yy-y);
		time += t;
		int c = sts[P].getC(xx-x, 0) + sts[P].getC(yy-y, 0);
		caro += c; ww += c;
		vector<string> pth0;
		if(mv2) sts[P].move2(xx-x, yy-y, pth0);
		else sts[P].move1(xx-x, yy-y, pth0);
		int k = 0;
		for(auto [x, y] : path(x, y, pth0)) {
			for(int i : gib) if(!gs[i].used) {
				const int dx = gs[i].c-x;
				const int dy = gs[i].r-y;
				if(dx*dx+dy*dy > D*D) continue;
				tmp.push_back("DeliverGift " + gs[i].name);
				ww += gs[i].w;
				gt.push_back(gs[i].name);
				score += gs[i].s;
				++ delivered;
				gs[i].used = true;
			}
			while(k < (int) pth0.size()) {
				tmp.push_back(pth0[k]);
				if(pth0[k++][0] == 'F') break;
			}
		}
		cerr << score << ' ' << delivered << ' ' << ww << ' ' << time << endl;
		x = xx; y = yy;
	}

	cerr << score << ' ' << time << '/' << T << endl;
	cout << ans.size() << endl;
	for(const string &s : ans) cout << s << '\n';

	return 0;
}