#include <cstdio>
#include <cstring>
#include <algorithm>
#include <iostream>
using namespace std;

int P = 31;
int Q = 63;
int L = 127;

int page[1000][100];

int main() {
	int address;
	freopen("input.txt", "r", stdin);
	freopen("output.txt", "w", stdout);

	for (int i = 0; i < 256; ++i) {
		string str;
		cin >> str;
		cin >> str;
		for (int j = 0; j < 32; ++j)
			cin >> hex >> page[i][j];
	}

	cin >> hex >> address;
	cout << address;
	
	int pde = 17, pte, psy;
	int pde_index = (address >> 11) & P;
	int pte_index = (address >> 6 ) & P;
	int psy_index = address & Q;
	cout << hex << "pde index: " << pde_index << "pde contents: " << (pte = page[pde][pde_index]) << "valid:" << (pte >> 7);
	pte = pte & L;
	cout << hex << "pte index: " << pte_index << "contents: " << (psy = page[pte][pte_index]) << "valid:" << (psy >> 7);
	psy = psy & L;
	cout << hex << "translate : " << (psy << 5) + psy_index << "contents: " << page[psy][psy_index];
	

	return 0;
}
