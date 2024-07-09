#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h>


void sqrtSerial(int N,
                float initialGuess,
                float values[],
                float output[])
{

    static const float kThreshold = 0.00001f;

    for (int i=0; i<N; i++) {

        float x = values[i];
        float guess = initialGuess;

        float error = fabs(guess * guess * x - 1.f);

        while (error > kThreshold) {
            guess = (3.f * guess - x * guess * guess * guess) * 0.5f;
            error = fabs(guess * guess * x - 1.f);
        }

        output[i] = x * guess;
    }
}

constexpr int VECTOR_WIDTH = 8;

void sqrtSIMD(int N,
                float initialGuess,
                float values[],
                float output[])
{

    static const float kThreshold = 0.00001f;
    __m256 threshold_vec = _mm256_set1_ps(kThreshold);
    __m256 absolute_mask = _mm256_castsi256_ps(_mm256_set1_epi32(0x7FFFFFFF));
    __m256 x, guess;
    __m256 mask_all = _mm256_castsi256_ps(_mm256_set1_epi32(0xFFFFFFFF));

    for (int i = 0; i < N; i += VECTOR_WIDTH) {
        x = _mm256_loadu_ps(values + i);
        guess = _mm256_set1_ps(initialGuess);

        __m256 tmp = guess * guess * x - 1.f;
        tmp = _mm256_and_ps(tmp, absolute_mask);

        __m256 mask = _mm256_cmp_ps(tmp, threshold_vec, 1); // error mask
        // mask = tmp < threshold_vec;
        while (_mm256_testc_ps(mask, mask_all) == 0) {
            guess = (3.f * guess - x * guess * guess * guess) * 0.5f;
            tmp = guess * guess * x - 1.f;
            tmp = _mm256_and_ps(tmp, absolute_mask);
            mask = _mm256_cmp_ps(tmp, threshold_vec, 1);
        }
        guess *= x;
        _mm256_storeu_ps(output + i, guess);
    }
}
