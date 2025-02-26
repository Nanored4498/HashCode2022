#include <iostream>
#include <vector>
#include <numeric>
#include <queue>
#include <tuple>
#include <random>
#include <algorithm>

using namespace std;
typedef vector<int> vi;
typedef vector<bool> vb;

struct Point {
	int x, y;
	Point() = default;
	Point(int x, int y): x(x), y(y) {}
	inline friend bool operator==(const Point &a, const Point &b) {
		return a.x == b.x && a.y == b.y;
	}
	inline friend bool operator!=(const Point &a, const Point &b) {
		return a.x != b.x || a.y != b.y;
	}
	inline friend int distance(const Point &a, const Point &b) {
		return abs(a.x - b.x) + abs(a.y - b.y);
	}
	Point& operator+=(char c) {
		switch(c) {
		case 'R':
			++ x;
			break;
		case 'L':
			-- x;
			break;
		case 'U':
			++ y;
			break;
		case 'D':
			-- y;
			break;
		case 'W':
			break;
		}
		return *this;
	}
	inline Point operator+(char c) { return Point(*this) += c; }
};

struct Arm {
	vector<string> how;
	string path, cp;
	vector<Point> cur;
	vi z;
	int i;
	bool done;
	Arm() = default;
	Arm(int W, int H, int x, int y, int i): how(W, string(H, 'x')), cur(1, Point(x, y)), i(i), done(false) {}
};

char opp(char c) {
	if(c == 'R') return 'L';
	if(c == 'L') return 'R';
	if(c == 'U') return 'D';
	if(c == 'D') return 'U';
	return c;
}

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	int W, H, R, M, T, L;
	cin >> W >> H >> R >> M >> T >> L;
	vector<Point> ms(M);
	for(int i = 0; i < M; ++i) cin >> ms[i].x >> ms[i].y;
	vi S(T), Len(T, 0);
	vector<vector<Point>> P(T);
	for(int t = 0; t < T; ++t) {
		int p;
		cin >> S[t] >> p;
		P[t].reserve(p);
		while(p--) {
			int x, y;
			cin >> x >> y;
			P[t].emplace_back(x, y);
		}
		for(int i = 1; i < P[t].size(); ++i) Len[t] += distance(P[t][i-1], P[t][i]);
	}

	int score = 0;
	// TODO: add time of free
	vector<vi> owner(W, vi(H, -1));
	vector<vi> until(W, vi(H, -1));
	vi ts(T); iota(ts.begin(), ts.end(), 0);
	{ // Remove some non interesting tasks
		for(int i = 0; i < T; ++i) {
			int add = 1e8;
			for(int j = 0; j < T; ++j) if(i != j) add = min(add, distance(P[i][0], P[j].back()));
			for(const Point &m : ms) add = min(add, distance(P[i][0], m));
			Len[i] += add;
		}
		int i = 0, j = 0;
		int gl = 0, GL = R*L;
		sort(ts.begin(), ts.end(), [&](int i, int j) { return double(S[i])/(Len[i]+1) > double(S[j])/(Len[j]+1); });
		while(j < T && gl+Len[ts[j]] < GL) {
			gl += Len[ts[j]];
			++j;
		}
		cerr << "KEEP: " << j << " / " << ts.size() << endl;
		ts.resize(j);
	}
	mt19937 mt(420);

	if(W == 300) { //F
		sort(ms.begin(), ms.end(), [&](const Point &a, const Point &b) { return min({a.x, W-1-a.x, a.y, H-1-a.y}) < min({b.x, W-1-b.x, b.y, H-1-b.y}); });
	} else {
		shuffle(ms.begin(), ms.end(), mt);
		// vector<int> dist(T);
		// for(int t : ts) dist[t] = distance(ms[0], P[t][0]) + distance(ms[0], P[t].back());
		// for(int i = 1; i < R; ++i) {
		// 	int d = 0;
		// 	for(int t : ts) d += S[t] * max(0, dist[t] - distance(ms[i], P[t][0]) - distance(ms[i], P[t].back()));
		// 	for(int k = i+1; k < M; ++k) {
		// 		int d2 = 0;
		// 		for(int t : ts) d2 += S[t] * max(0, dist[t] - distance(ms[k], P[t][0]) - distance(ms[k], P[t].back()));
		// 		if(d2 > d) {
		// 			d = d2;
		// 			swap(ms[i], ms[k]);
		// 		}
		// 	}
		// 	for(int t : ts) dist[t] = min(dist[t], distance(ms[i], P[t][0]) + distance(ms[i], P[t].back()));
		// }
		// for(int i = 1; i < R; ++i) {
		// 	int d = 1e8;
		// 	for(int j = 0; j < i; ++j) d = min(d, distance(ms[i], ms[j]));
		// 	for(int k = i+1; k < M; ++k) {
		// 		int d2 = 1e8;
		// 		for(int j = 0; j < i; ++j) d2 = min(d2, distance(ms[k], ms[j]));
		// 		if(d2 > d) {
		// 			d = d2;
		// 			swap(ms[i], ms[k]);
		// 		}
		// 	}
		// }
	}

	vector<Arm> arms(R);
	for(int i = 0; i < M; ++i) {
		owner[ms[i].x][ms[i].y] = i;
		until[ms[i].x][ms[i].y] = L;
		if(i < R) arms[i] = Arm(W, H, ms[i].x, ms[i].y, i);
	}

	vector<vi> seen(W, vi(H, 0));
	vector<vi> dist(W, vi(H, 0));
	vector<string> pred(W, string(H, 'x'));
	int SS = 0;

	while(true) {
		int i = -1;
		for(int i0 = 0; i0 < R; ++i0)
			if(!arms[i0].done && (i == -1 || arms[i0].path.size() < arms[i].path.size()))
				i = i0;
		if(i == -1) break;
		const int l0 = arms[i].path.size();
		cerr << "I " << i << ' ' << l0 << endl;
		Arm bestA;
		int bestT = -1;
		double bestS = -1;
		for(int t : ts) {
			bool bad = false;
			Arm a = arms[i];
			for(const Point &pt : P[t]) {
				queue<Point> Q;
				++ SS;
				bool found = a.cur.back() == pt;
				Q.push(a.cur.back());
				seen[a.cur.back().x][a.cur.back().y] = SS;
				dist[a.cur.back().x][a.cur.back().y] = a.path.size();
				pred[a.cur.back().x][a.cur.back().y] = 'x';
				while(!Q.empty() && !found) {
					auto q = Q.front(); Q.pop();
					if(dist[q.x][q.y] >= L) break;
					string vs = "RLUD";
					if(a.how[q.x][q.y]!='x' || q==a.cur.back()) for(char c : vs) {
						Point p = q+c;
						if(p.x < 0 || p.x >= W || p.y < 0 || p.y >= H || a.how[p.x][p.y] != c) continue;
						seen[p.x][p.y] = SS;
						dist[p.x][p.y] = dist[q.x][q.y]+1;
						pred[p.x][p.y] = 'x';
						if(p == pt) {
							found = true;
							break;
						}
						Q.push(p);
					}
					shuffle(vs.begin(), vs.end(), mt);
					for(char c : vs) {
						Point p = q+c;
						if(p.x < 0 || p.x >= W || p.y < 0 || p.y >= H
							|| seen[p.x][p.y] == SS
							|| a.how[p.x][p.y] != 'x'
							|| (owner[p.x][p.y] != i && until[p.x][p.y] > dist[q.x][q.y])) continue;
						seen[p.x][p.y] = SS;
						dist[p.x][p.y] = dist[q.x][q.y]+1;
						pred[p.x][p.y] = c;
						if(p == pt) {
							found = true;
							break;
						}
						Q.push(p);
					}
				}
				if(!found) {
					bad = true;
					break;
				}
				string add;
				Point p = pt;
				while(pred[p.x][p.y] != 'x') {
					add.push_back(pred[p.x][p.y]);
					p += opp(pred[p.x][p.y]);
				}
				while(a.cur.back() != p) {
					a.path += opp(a.cp.back());
					a.cur.pop_back();
					a.cp.pop_back();
					a.how[a.cur.back().x][a.cur.back().y] = 'x';
				}
				reverse(add.begin(), add.end());
				a.cp += add;
				a.path += add;
				for(char c : add) {
					a.how[a.cur.back().x][a.cur.back().y] = opp(c);
					a.cur.push_back(a.cur.back()+c);
				}
				if(a.path.size() > L) {
					bad = true;
					break;
				}
			}
			if(bad) continue;
			// double sc = double(S[t]);
			// double sc = L - double(a.path.size() - arms[i].path.size());
			// double sc = double(S[t]) / double(a.path.size() - arms[i].path.size() + a.cur.size());
			double sc = double(S[t]) / double(a.path.size() - arms[i].path.size());
			if(sc > bestS) {
				bestA = move(a);
				bestS = sc;
				bestT = t;
			}
		}
		if(bestT == -1) {
			if(arms[i].cur.size() <= 1) {
				arms[i].done = true;
				continue;
			}
			while(arms[i].cur.size() > 1 && arms[i].path.size() < L) {
				Point p = arms[i].cur.back();
				until[p.x][p.y] = arms[i].path.size();
				arms[i].path.push_back(opp(arms[i].cp.back()));
				arms[i].cur.pop_back();
				arms[i].cp.pop_back();
				arms[i].how[arms[i].cur.back().x][arms[i].cur.back().y] = 'x';
			}
			for(int j = 0; j < R; ++j) if(arms[j].path.size() < L) arms[j].done = false;
			arms[i].done = true;
			continue;
		}
		Point p = arms[i].cur.back();
		for(int l = arms[i].path.size(); l < bestA.path.size(); ++l) {
			if(bestA.how[p.x][p.y] == 'x' && p != bestA.cur.back()) until[p.x][p.y] = l;
			p += bestA.path[l];
			owner[p.x][p.y] = i;
			until[p.x][p.y] = L;
		}
		arms[i] = move(bestA);
		score += S[bestT];
		int ind = 0;
		while(ts[ind] != bestT) ++ind;
		swap(ts[ind], ts.back());
		ts.pop_back();
		arms[i].z.push_back(bestT);
		cerr << score << '\n';
		for(int j = 0; j < R; ++j) if(arms[j].path.size() < L) arms[j].done = false;
	}

	cerr << "restant: " << ts.size() << endl;

	int A = 0;
	for(const Arm &a : arms) if(!a.z.empty()) ++A;
	cout << A << '\n';
	for(const Arm &a : arms) if(!a.z.empty()) {
		cout << a.cur[0].x << ' ' << a.cur[0].y << ' ' << a.z.size() << ' ' << a.path.size() << '\n';
		for(int t : a.z) cout << t << ' ';
		cout << '\n';
		for(char c : a.path) cout << c << ' ';
		cout << '\n';
	}
	cerr << score << '\n';

	return 0;
}