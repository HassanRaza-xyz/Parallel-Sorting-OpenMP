#include <iostream>
#include <vector>
#include <omp.h>
#include <algorithm>
#include <ctime>

using namespace std;

// --- Merge Function ---
void merge(int arr[], int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;
    vector<int> L(n1), R(n2);

    for (int i = 0; i < n1; i++) L[i] = arr[l + i];
    for (int j = 0; j < n2; j++) R[j] = arr[m + 1 + j];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

// --- Parallel Merge Sort ---
void parallelMergeSort(int arr[], int l, int r, int depth) {
    if (l < r) {
        int m = l + (r - l) / 2;
        if (depth > 0) {
            #pragma omp task shared(arr)
            parallelMergeSort(arr, l, m, depth - 1);
            #pragma omp task shared(arr)
            parallelMergeSort(arr, m + 1, r, depth - 1);
            #pragma omp taskwait
        } else {
            parallelMergeSort(arr, l, m, 0);
            parallelMergeSort(arr, m + 1, r, 0);
        }
        merge(arr, l, m, r);
    }
}

// --- Verification Function ---
bool isSorted(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        if (arr[i] > arr[i + 1]) return false;
    }
    return true;
}

int main() {
    int n = 1000000; // Increased to 10 Lakh for better results
    int* arr_p = new int[n];
    int* arr_s = new int[n];

    // Data Generation
    for (int i = 0; i < n; i++) {
        int val = rand() % 10000;
        arr_p[i] = val;
        arr_s[i] = val;
    }

    cout << "==========================================" << endl;
    cout << "   PDC PROJECT: PARALLEL MERGE SORT   " << endl;
    cout << "   Student: Hassan | BS AI 6th-B     " << endl;
    cout << "==========================================" << endl;
    cout << "Sorting " << n << " elements..." << endl << endl;

    // 1. SERIAL EXECUTION
    double start_s = omp_get_wtime();
    parallelMergeSort(arr_s, 0, n - 1, 0); // depth 0 means serial
    double end_s = omp_get_wtime();
    double time_s = end_s - start_s;
    cout << "[1] Serial Merge Sort Time   : " << time_s << " sec" << endl;

    // 2. PARALLEL EXECUTION
    int threads = omp_get_max_threads();
    double start_p = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp single
        parallelMergeSort(arr_p, 0, n - 1, 4); 
    }
    double end_p = omp_get_wtime();
    double time_p = end_p - start_p;
    
    cout << "[2] Parallel Merge Sort Time : " << time_p << " sec (using " << threads << " threads)" << endl;

    // 3. PERFORMANCE RESULTS
    cout << "------------------------------------------" << endl;
    if (isSorted(arr_p, n)) {
        cout << "STATUS: Sorting Successful!" << endl;
        cout << "SPEEDUP: " << (time_s / time_p) << "x faster than Serial." << endl;
    } else {
        cout << "STATUS: Sorting Failed!" << endl;
    }
    cout << "------------------------------------------" << endl;

    delete[] arr_p;
    delete[] arr_s;
    return 0;
}