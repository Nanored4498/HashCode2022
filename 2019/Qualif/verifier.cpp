#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>

using namespace std;

int main(int argc, char* argv[]) {
	if(argc != 3) {
		cerr << "Usage:\n";
		cerr << '\t' << argv[0] << " `in.txt` `out.txt`\n";
		return 1;
	}

	ifstream in(argv[1]), out(argv[2]);
	if(!in) {
		cerr << "Impossible to open input: " << argv[1] << endl;
		return 1;
	}
	if(!out) {
		cerr << "Impossible to open output: " << argv[2] << endl;
		return 1;
	}

	struct Photo {
		char type;
		vector<string> tags;
		bool used = false;
	};
	int score = 0;

	int N;
	in >> N;
	vector<Photo> photos(N);
	for(Photo &p : photos) {
		int m;
		in >> p.type >> m;
		p.tags.resize(m);
		for(string &t : p.tags) in >> t;
	}

	int S;
	string tmp;
	out >> S;
	getline(out, tmp);
	vector<string> last;
	while(S--) {
		string line;
		getline(out, line);
		istringstream iss(line);
		char type = 0;
		int id;
		vector<string> tags;
		while(iss >> id) {
			if(id < 0 || id >= N) {
				cerr << "Bad photo id " << id << '\n';
				return 1;
			}
			if(photos[id].used) {
				cerr << "Photo " << id << " is used multiple times...\n";
				return 1;
			}
			photos[id].used = true;
			if(!type) type = photos[id].type;
			else if(type == 'V') type = 1;
			else {
				cerr << "Bad compositions of photos in a slide...\n";
				return 1;
			}
			tags.insert(tags.end(), photos[id].tags.begin(), photos[id].tags.end());
		}
		if(!type) {
			cerr << "A slide is empty... (remaining " << S << " slides)\n";
			return 1;
		}
		sort(tags.begin(), tags.end());
		tags.resize(unique(tags.begin(), tags.end())-tags.begin());
		if(last.empty()) {
			last = std::move(tags);
			continue;
		}
		int C = 0;
		int i = 0, j = 0;
		while(i < (int) last.size() && j < (int) tags.size()) {
			if(last[i] < tags[j]) {
				++i;
			} else if(last[i] == tags[j]) {
				++ C;
				++i;
				++j;
			} else {
				++j;
			}
		}
		score += min(C, min((int)last.size(), (int)tags.size())-C);
		last = std::move(tags);
	}
	cout << score << endl;

	return 0;
}