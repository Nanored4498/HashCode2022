#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <queue>
#include <functional>

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
	bool used=false;
};

map<string, string> opp;

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

	const int P = 0;
	const int NEG = 40;
	const int POS = 1200;
	const int NEGPOS = NEG+POS;
	int DD = mac-mic+1;
	int NN = NEGPOS*DD;
	cerr << NEGPOS << ' ' << NN << ' ' << DD << endl;
	const auto p2i = [&](int x, int v) { return x + DD*(v+NEG); };
	const auto i2p = [&](int i) { return make_pair((i%DD), (i/DD)-NEG); };
	vi pred(NN), dist(NN, 1e9);
	dist[p2i(0, 0)] = 0;
	queue<int> Q; Q.push(p2i(0, 0));
	while(!Q.empty()) {
		const auto [x, v] = i2p(Q.front()); Q.pop();
		const int d = dist[p2i(x, v)] + 1;
		for(int a = -aa[P]; a <= aa[P]; ++a) {
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
	const auto getD = [&](int x) {
		return dist[p2i(abs(x), 0)];
	};
	const auto move0 = [&](int x, const string &op, vector<string> &a) {
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
	const auto move2 = [&](int x, int y, vector<string> &a) {
		if(x > 0) move0(x, "AccRight", a);
		else if(x < 0) move0(x, "AccLeft", a);
		if(y > 0) move0(y, "AccUp", a);
		else if(y < 0) move0(y, "AccDown", a);
	};
	vi car(NN, -1); car[p2i(0, 0)] = 0;
	function<int(int, int)> getC;
	getC = [&](int x, int v) {
		x = abs(x);
		const int i = p2i(x, v);
		if(car[i] == -1) {
			const int add = pred[i]==0 ? 0 : 1;
			car[i] = getC(x-v, v-pred[i]) + add;
		}
		return car[i];
	};
	cerr << "Precompute done" << endl;

	int score = 0;
	int time = 0, caro = 0;
	int x = 0, y = 0;
	vector<string> ans, tmp, gt;
	while(true) {
		map<pair<int, int>, pair<int, int>> loc;
		for(const Gift &g : gs) if(!g.used) {
			if(g.c > 0 || g.r < 0) continue;
			for(int dx = -D; dx <= D; ++dx) {
				for(int dy = -D; dy <= D; ++dy) {
					if(dx*dx+dy*dy > D*D) continue;
					int x2 = g.c+dx;
					int y2 = g.r+dy;
					if(loc.count({x2, y2})) loc[{x2, y2}].second += g.s;
					else {
						int d = getD(x2-x) + getD(y2-y);
						if(time+d > T) continue;
						loc[{x2, y2}] = {d, g.s};
					}
				}
			}
		}
		double best_score = -1;
		int xx=0, yy=0;
		for(const auto &[p, s] : loc) {
			double sc = double(s.second) / double(s.first);
			if(sc > best_score) {
				best_score = sc;
				tie(xx, yy) = p;
			}
		}
		if(best_score < 0) break;
		int t = getD(xx-x) + getD(yy-y);
		time += t;
		move2(xx-x, yy-y, tmp);
		caro += getC(xx-x, 0) + getC(yy-y, 0);
		for(Gift &g : gs) if(!g.used) {
			const double dx = abs(xx-g.c);
			if(dx > D) continue;
			const double dy = abs(yy-g.r);
			if(dy > D) continue;
			if(dx*dx+dy*dy > D*D) continue;
			g.used = true;
			score += g.s;
			tmp.push_back("DeliverGift " + g.name);
			gt.push_back(g.name);
			cerr << score << endl;
		} 
		x = xx; y = yy;
	}
	ans.push_back("LoadCarrots " + to_string(caro));
	for(const string &s : gt) ans.push_back("LoadGift " + s);
	ans.insert(ans.end(), tmp.begin(), tmp.end());

	cerr << score << ' ' << time << '/' << T << endl;
	cout << ans.size() << endl;
	for(const string &s : ans) cout << s << '\n';

	return 0;
}