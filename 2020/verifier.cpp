#include <iostream>
#include <vector>
#include <fstream>

using namespace std;
typedef vector<int> vi;
typedef vector<bool> vb;

struct Point {
	int x, y;
	Point() = default;
	Point(int x, int y): x(x), y(y) {}
	friend bool operator==(const Point &a, const Point &b) {
		return a.x == b.x && a.y == b.y;
	}
};

int main(int argc, const char* argv[]) {
	if(argc < 3) {
		cerr << "Usage:" << endl;
		cerr << "./prog input_file output_file" << endl;
		return 1;
	}

	// IN
	ifstream in(argv[1]);
	if(in.bad()) {
		cerr << "Bad input file" << endl;
		return 1;
	}
	int W, H, R, M, T, L;
	in >> W >> H >> R >> M >> T >> L;
	vector<vb> m(W, vb(H, false));
	for(int i = 0; i < M; ++i) {
		int x, y;
		in >> x >> y;
		m[x][y] = true;
	}
	vi S(T);
	vector<vector<Point>> P(T);
	for(int t = 0; t < T; ++t) {
		int p;
		in >> S[t] >> p;
		P[t].reserve(p);
		while(p--) {
			int x, y;
			in >> x >> y;
			P[t].emplace_back(x, y);
		}
	}

	// OUT
	ifstream out(argv[2]);
	if(out.bad()) {
		cerr << "Bad input file" << endl;
		return 1;
	}
	int A;
	out >> A;
	if(A < 1 || A > R) {
		cerr << "A must be in [1, R], value: " << A << endl;
		return 1;
	}
	vector<vector<Point>> arm(A);
	vector<string> mvs(A);
	vector<vi> Z(A);
	vb completed(T, false);
	vector<vi> pp(W, vi(H, -1));
	for(int i = 0; i < A; ++i) {
		int k, z;
		arm[i].resize(1);
		Point &a = arm[i][0];
		out >> a.x >> a.y >> z >> k;
		if(a.x < 0 || a.x >= W || a.y < 0 || a.y >= H || !m[a.x][a.y] || pp[a.x][a.y] != -1) {
			cerr << "An arm is not at a valid position: " << a.x << ' ' << a.y << endl;
			return 1;
		}
		pp[a.x][a.y] = i;
		if(z < 1 || z > T) {
			cerr << "Number of tasks not valid: " << z << endl;
			return 1;
		}
		if(k < 1 || k > L) {
			cerr << "number of instructions not valid: " << k << endl;
			return 1;
		}
		Z[i].resize(z);
		for(int &t : Z[i]) out >> t;
		mvs[i].resize(k);
		for(char &c : mvs[i]) out >> c;
	}

	for(int i = 0; i < W; ++i) for(int j = 0; j < H; ++j) if(m[i][j] && pp[i][j] == -1) pp[i][j] = A;
	vector<pair<int, int>> st(A, {0, 0});
	int score = 0;

	for(int l = 0; l < L; ++l) {
		for(int i = 0; i < A; ++i) {
			if(l < mvs[i].size()) {
				Point a = arm[i].back();
				switch(mvs[i][l]) {
				case 'R':
					if(++ a.x >= W) {
						cerr << "Arm outside..." << endl;
						return 1;
					}
					break;
				case 'L':
					if(-- a.x < 0) {
						cerr << "Arm outside..." << endl;
						return 1;
					}
					break;
				case 'U':
					if(++ a.y >= H) {
						cerr << "Arm outside..." << endl;
						return 1;
					}
					break;
				case 'D':
					if(-- a.y < 0) {
						cerr << "Arm outside..." << endl;
						return 1;
					}
					break;
				case 'W':
					break;
				default:
					cerr << "Unrecognized move: " << mvs[i][l] << endl;
					return 1;
				}
				if(a == arm[i].back()) {
					// Nothing
				} else if(pp[a.x][a.y] == -1) {
					pp[a.x][a.y] = i;
					arm[i].push_back(a);
				} else if(pp[a.x][a.y] == i) {
					if(arm[i].size() > 1 && arm[i][arm[i].size()-2] == a) {
						pp[arm[i].back().x][arm[i].back().y] = -1;
						arm[i].pop_back();
					} else {
						cerr << "Unvalid move!!! " << i << ' ' << l << endl;
						return 1;
					}
				} else {
					arm[i].push_back(a);
					// we hope it's good
				}
			}
		}
		for(int i = 0; i < A; ++i) if(pp[arm[i].back().x][arm[i].back().y] != i) {
			if(pp[arm[i].back().x][arm[i].back().y] != -1) {
				cerr << "Unvalid move!!!!!! (at " << l << ' ' << i << ' ' << pp[arm[i].back().x][arm[i].back().y]
					<< "  " << arm[i].back().x << ',' << arm[i].back().y << ")" << endl;
				return 1;
			}
			pp[arm[i].back().x][arm[i].back().y] = i;
		}
		for(int i = 0; i < A; ++i) {
			while(st[i].first < Z[i].size()) {
				int t = Z[i][st[i].first];
				int &j = st[i].second;
				if(P[t][j] == arm[i].back()) {
					if(++j >= P[t].size()) {
						++ st[i].first;
						j = 0;
						if(completed[t]) {
							cerr << "Task " << t << " is completed several times" << endl;
							return 1;
						}
						completed[t] = true;
						score += S[t];
					}
				} else break;
			}
		}
	}

	for(int i = 0; i < A; ++i) if(st[i].first < Z[i].size()) {
		cerr << "Arm " << i << " has not completed its tasks!!" << endl;
		return 1;
	}

	cerr << "Valid" << endl;
	cerr << "Score: " << score << endl;

	return 0;
}