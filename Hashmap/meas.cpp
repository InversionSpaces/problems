#include <iostream>
#include <fstream>
#include <cinttypes>
#include <vector>
#include <random>
#include <string>
#include <stdexcept>

#include "hashmap.hpp"

using namespace std;

uint64_t xor_hash(const string& str) {
	uint64_t hash = 0;
	
	size_t i = 0;
	for (const auto& c: str) {
		hash ^= static_cast<uint64_t>(c) << 8 * i;
		i = (i + 1) % 8;
	}
	
	return hash;
}

extern "C" uint64_t __xor_hash(const char* str, size_t len);

uint64_t xor_hash_asm(const string& str) {
	return __xor_hash(str.data(), str.size());
}

vector<string> load_lines(const char* filename) {
	ifstream input(filename);

	if (!input.is_open()) {
		string msg = "File " + string(filename) + " doesn't exist";
		throw runtime_error(msg);
	}

	vector<string> retval;
	for (	retval.emplace_back(); 
		getline(input, retval.back()); 
		retval.emplace_back());
	retval.pop_back();

	return retval;
}

class randdist {
private:
	static random_device rd;
	
	mt19937 gen;
	uniform_int_distribution<> dis;
public:
	randdist(int st, int ed) : dis(st, ed), gen(randdist::rd()) {}

	int next() {
		return dis(gen);
	}
};

random_device randdist::rd;

struct streq {
	bool operator()(const string& lhs, const string& rhs) const {
		if (lhs.size() != rhs.size())
			return false;

		for (size_t i = 0; i < lhs.size(); ++i)
			if (lhs[i] != rhs[i])
				return false;

		return true;
	}
};

int main() {
	cout << xor_hash_asm("simplestring") << endl;
	cout << xor_hash("simplestring") << endl;
	/*
	auto lines = load_lines("strings");
	randdist dist(0, lines.size() - 1);

	HashMap<string, streq> hmap(xor_hash);
	for (int i = 0; i < 5000000; ++i) {
		hmap.insert(lines[dist.next()]);
		hmap.contains(lines[dist.next()]);
		hmap.erase(lines[dist.next()]);
	}
	*/
}
