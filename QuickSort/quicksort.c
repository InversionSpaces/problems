#include <stdio.h>

void swap(int* a, int* b) {
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

void quicksort(int* array, size_t size) {
	if(size < 2) return;
	
	size_t l = 0, r = size - 1, p = (l + r) / 2;
	while(l < r) {	
		while(array[l] < array[p] && l < p) l++;
		while(array[r] >= array[p] && p < r) r--;
		if(l == r) break;
		
		swap(&array[l], &array[r]);
		
		if(l == p) p = r;
		else if(r == p) p = l;
	}
	
	quicksort(array, p);
	quicksort(array + p + 1, size - p - 1);
}

int main() {
	int arr[] = {0, 8, 2, 13, 3, 2, 2, 2, 0, 0, 23, 40, -5};
	size_t size = sizeof(arr) / sizeof(int);
	quicksort(arr, size);
	for(int i = 0; i < size; i++)
		printf("%d ", arr[i]);
}
