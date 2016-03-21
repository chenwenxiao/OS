#include <cstdio>
#include <cstring>
#include <algorithm>
using namespace std;

const int MAXN = 101, MAXP = 1001;

int hits;
int n, L;
int mem[MAXP];
int addr[MAXN];

int main() {
	freopen("input.txt", "r", stdin);
	freopen("output.txt", "w", stdout);
	
	scanf("%d %d", &n, &L);
	for (int i = 0; i < n; ++i)
		scanf("%d", &addr[i]);
	
	int last = -1;
	for (int i = 0; i < n; ++i) {
		if (mem[addr[i]]) {
			printf("Address %d hit\n", addr[i]);
			hits++;
		} else {
			if (i - last <= L) {
				mem[addr[i]] = true;
				printf("Add Address %d into memory\n", addr[i]);
			} else {
				memset(mem, 0, sizeof(mem));
				printf("Remove All Address\n");
				for (int j = last + 1; j <= i; ++j) {
					mem[addr[j]] = true;
					printf("Add Address %d into memory\n", addr[j]);
				}
				
			}
		}	
	}
	printf("Hits rate is %lf", 1.0 * hits / n);
	
	
	return 0;
}
