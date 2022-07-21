#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <unordered_map>
#include <queue>
#include <functional>
#include <cassert>

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
	vi l(W), a(W);
	vector<Gift> gs(G);
	for(int i = 0; i < W; ++i) cin >> l[i] >> a[i];
	for(int i = 0; i < G; ++i) cin >> gs[i].name >> gs[i].s >> gs[i].w >> gs[i].c >> gs[i].r;

	int mic=0, mac=0;
	for(int i = 0; i < G; ++i) {
		cin >> gs[i].name >> gs[i].s >> gs[i].w >> gs[i].c >> gs[i].r;
		mic = min({mic, gs[i].c, gs[i].r});
		mac = max({mac, gs[i].c, gs[i].r});
	}

	int DD = mac-mic+1;
	int NN = 1000*DD;
	const auto p2i = [&](int x, int v) { return x + DD*(v+200); };
	const auto i2p = [&](int i) { return make_pair((i%DD), (i/DD)-200); };
	vi pred(NN), dist(NN, 1e9);
	dist[p2i(0, 0)] = 0;
	queue<int> Q; Q.push(p2i(0, 0));
	while(!Q.empty()) {
		const auto [x, v] = i2p(Q.front()); Q.pop();
		const int d = dist[p2i(x, v)] + 1;
		for(int a = -100; a <= 100; ++a) {
			const int w = v+a;
			if(w < -200) continue;
			if(w >= 800) continue;
			const int y = x+w;
			if(y < 0) continue;
			if(y >= DD) continue;
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
	vi car(NN, -1); car[p2i(0, 0)] = 0;
	function<int(int, int)> getC;
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
	getC = [&](int x, int v) {
		x = abs(x);
		const int i = p2i(x, v);
		if(car[i] == -1) {
			const int add = pred[i]==0 ? 0 : 1;
			car[i] = getC(x-v, v-pred[i]) + add;
		}
		return car[i];
	};
	double w_coeff = 2.*getD(DD/4)/double(l[0]);
	vector<vi> gind(DD, vi(DD, -1));
	for(int i = 0; i < G; ++i) gind[gs[i].c-mic][gs[i].r-mic] = i;
	cerr << "Precompute done" << endl;

	int score = 0, delivered = 0;
	int time = 0, caro = 0, ww = 0;
	int x = 0, y = 0;
	vector<string> ans, tmp, gt;
	while(true) {
		int bestI = -1;
		double best_score = 0;
		for(int i = 0; i < gs.size(); ++i) if(!gs[i].used) {
			int d = getD(gs[i].c-x) + getD(gs[i].r-y);
			int w = gs[i].w + getC(gs[i].c-x, 0) + getC(gs[i].r-y, 0) + getC(gs[i].c, 0) + getC(gs[i].r, 0);
			if(time+d > T || w+ww > l[0]) continue;
			double sc = double(gs[i].s) / (d + w_coeff * w);	
			if(sc > best_score) {
				best_score = sc;
				bestI = i;
			}
		}
		if(bestI == -1) {
			if(x == 0 && y == 0) break;
			int t = getD(x) + getD(y);
			bool bad = false;
			if(time+t <= T) {
				caro += getC(x, 0) + getC(y, 0);
				move2(-x, -y, tmp);
				time += t;
			} else bad = true;
			ans.push_back("LoadCarrots " + to_string(caro));
			for(const string &s : gt) ans.push_back("LoadGift " + s);
			ans.insert(ans.end(), tmp.begin(), tmp.end());
			if(bad) break;
			caro = 0;
			ww = 0;
			tmp.clear();
			gt.clear();
			x = 0; y = 0;
			continue;
		}
		int t = getD(gs[bestI].c-x) + getD(gs[bestI].r-y);
		time += t;
		int c = getC(gs[bestI].c-x, 0) + getC(gs[bestI].r-y, 0);
		caro += c; ww += c + gs[bestI].w;
		move2(gs[bestI].c-x, gs[bestI].r-y, tmp);
		tmp.push_back("DeliverGift " + gs[bestI].name);
		gt.push_back(gs[bestI].name);
		score += gs[bestI].s;
		++ delivered;
		cerr << score << ' ' << delivered << endl;
		gs[bestI].used = true;
		x = gs[bestI].c; y = gs[bestI].r;
	}

	cerr << score << ' ' << time << '/' << T << endl;
	cout << ans.size() << endl;
	for(const string &s : ans) cout << s << '\n';

	return 0;
}