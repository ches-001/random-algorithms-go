#include <iostream>
#include <cmath>
#include <chrono>
#include <vector>
#include <bitset>
using namespace std;

bool isPrime(int val){
    if (val == 1 | val == 0) return false;

    double val_sqrt = sqrt(val);
    for (int i=2; i <= sqrt(val); i++){
        if (val % i == 0) return false;
    }
    return true;
}

void regular_algo(int num){
    int num_primes = 0;
    int max_prime = 0;

    auto start = std::chrono::high_resolution_clock::now();
    for(int i=0; i <= num; i++){
        if (isPrime(i)){
            num_primes++;
            if (i > max_prime) max_prime = i;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    printf(
        "regular algorithm: num primes: %d \tmax_prime: "
        "%d \tduration: %f sec\n", num_primes, max_prime, duration
    );
}

void sieve_of_eratosthenes(int num){
    std::vector<bool> vec(num, true);
    int i = 2;
    int num_primes = 0;
    int max_prime = 0;
    auto start = std::chrono::high_resolution_clock::now();

    while(i <= sqrt(num)){
        if (vec[i]){
            for (int j=i*i; j <= num; j+=i) vec[j] = false;
        }
        i++;
    }
    for(int k=2; k < num; k++){
        if (vec[k]){
            num_primes++;
            max_prime = k;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    printf(
        "sieve of eratosthenes: num primes: %d \tmax_prime: "
        "%d \tduration: %f sec\n", num_primes, max_prime, duration
    );
}

int main(){
    const int MAX_VAL = 10000000;
    regular_algo(MAX_VAL);
    sieve_of_eratosthenes(MAX_VAL);
    return 0;
}