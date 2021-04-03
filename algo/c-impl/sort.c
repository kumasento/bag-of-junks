#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void swap(int *a, int *b) {
  int tmp = *a;
  *a = *b;
  *b = tmp;
}

static void print_array(int arr[], int n) {
  int i;
  printf("[ ");
  for (i = 0; i < n; i++)
    printf("%3d ", arr[i]);
  printf("]\n");
}

void insert_sort(int arr[], int n) {
  assert(n >= 0);

  int i, j;

  for (i = 0; i < n - 1; i++) {
    for (j = i + 1; j < n; j++) {
      if (arr[j] < arr[i]) {
        swap(&arr[i], &arr[j]);
      }
    }
  }
}

void merge_sort(int arr[], int n) {
  if (n <= 1)
    return;
  if (n == 2) {
    if (arr[1] < arr[0])
      swap(&arr[0], &arr[1]);
    return;
  }

  int tmp[n];
  int i, j, k;

  merge_sort(arr, n / 2);
  merge_sort(arr + n / 2, n - n / 2);

  j = 0, k = n / 2;
  for (i = 0; i < n; i++) {
    if (k == n || (j != n / 2 && arr[j] < arr[k]))
      tmp[i] = arr[j++];
    else
      tmp[i] = arr[k++];
  }
  memcpy(arr, tmp, n * sizeof(int));
}

void quick_sort(int arr[], int n) {
  if (n <= 1)
    return;
  if (n == 2) {
    if (arr[1] < arr[0])
      swap(&arr[0], &arr[1]);
    return;
  }

  int i;
  int m = 0; // where the pivot is
  int pivot = arr[m];
  for (i = 1; i < n; i++) {
    if (arr[i] < pivot) {
      swap(&arr[m], &arr[m + 1]);
      if (i != m + 1)
        swap(&arr[m], &arr[i]);
      m++;
    }
  }

  quick_sort(arr, m);
  quick_sort(arr + m, n - m);
}

// Heap
#define LEFT_CHILD(i) (((i) << 1) + 1)
#define RIGHT_CHILD(i) (((i + 1) << 1))

void max_heapify(int arr[], int i, int n) {
  if (LEFT_CHILD(i) < n) {
    max_heapify(arr, LEFT_CHILD(i), n);
    if (arr[i] < arr[LEFT_CHILD(i)])
      swap(&arr[i], &arr[LEFT_CHILD(i)]);
  }
  if (RIGHT_CHILD(i) < n) {
    max_heapify(arr, RIGHT_CHILD(i), n);
    if (arr[i] < arr[RIGHT_CHILD(i)])
      swap(&arr[i], &arr[RIGHT_CHILD(i)]);
  }
}

void make_heap(int arr[], int n) {
  for (int i = n / 2; i >= 0; i--)
    max_heapify(arr, i, n);
}

void pop_heap(int arr[], int n) {
  swap(&arr[0], &arr[n - 1]);
  max_heapify(arr, 0, n - 1);
}

void heap_sort(int arr[], int n) {
  int i;
  make_heap(arr, n);
  for (i = n; i >= 2; i--)
    pop_heap(arr, i);
}

void main() {
  int arr[] = {1, 3, 8, -1, -5, 6, 7, 7, -10, 2};
  int n = sizeof(arr) / sizeof(int);
  int i;

  print_array(arr, n);
  insert_sort(arr, n);
  print_array(arr, n);

  int arr2[] = {1, 3, 8, -1, -5, 6, 7, 7, -10, 2};
  print_array(arr2, n);
  merge_sort(arr2, n);
  print_array(arr2, n);

  int arr3[] = {1, 3, 8, -1, -5, 6, 7, 7, -10, 2};
  print_array(arr3, n);
  merge_sort(arr3, n);
  print_array(arr3, n);

  int arr4[] = {1, 3, 8, -1, -5, 6, 7, 7, -10, 2};
  print_array(arr4, n);
  heap_sort(arr4, n);
  print_array(arr4, n);
}
