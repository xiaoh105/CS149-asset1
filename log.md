## CS149 Asset1 (Write-ups and logs)

~~In fact, I didn't intend to write such a log. But the magnificent homework changed my mind.~~

### Program 1: Parallel Fractal Generation Using Threads

**Version 1: ** The simplest idea it to divide work so that each thread is responsible for calculating a continuous range of rows. This solution maximizes space locality, but leads to severely imbalanced work. (It is evident that rows in the center take longer time to compute) As a result, it doesn't lead to linear increase of efficiency.

**Version2: ** The revised work distribution is to make each thread responsible for a single line, and they calculate alternatively. This may make work distributed more evenly. This leads to approximately linear increase of efficiency.(About 7.1x boost with 8 threads since imbalanced distribution exists after all.)

**Q: Better to run with > 8 threads? **

**A: **~~Yeah, it runs faster for my computer.~~ For a dual-core computer with hyper thread, >8 threads leads to frequent shift of context without actually utilizing more computing resources. So it isn't recommended to do so.



### Program 2: Vectorizing Code Using SIMD Intrinsics

As an introduction to parallel computing, it doesn't have too much pitfalls or challenges. So... not much to say about. QAQ

**About vector utilization: **Although vector utilization decreases as vector width increases, the decrease is quite small (About 2% decrease as vector width doubles). 

**Extra credit:** The extra credit asks you to write an element-wise sum of an arbitrary vector. Simply, use `hadd` to add up adjacent elements. Afterwards, use `interleave` to swap partial sum together. If the vector has $2^n$ elements, repeating $n$ will make the first element of the vector the sum of all elements.



### Program 3: Parallel Fractal Generation Using ISPC

#### Part 1

**Q: Why can't ISPC program reach ideal speed? **

**A: **Since SIMD uses a single control flow to manipulate multiple data, it may not make full use of its vector width if there're branches or loops. Thus, we only get about 6x speedup with only SIMD optimizations.

#### Part 2

**Q: How to determine the number of tasks? **

**A: **In theory, use the number of virtual processors as the number of tasks spawn may be the best answer. However, due to the abstraction of OS, if we choose the tasks within a proper range, it all works quiet fast. (About 60x speedup with 32 tasks)

**Q: What's the nuance between task abstraction and thread abstraction?**

**A: ** **I Guess that** tasks are another of abstraction of threads. For threads, they're considered as separate works to be managed by the OS, and may takes higher cost of creating and joining (if there're plenty of them). But tasks may be managed by something like the thread pool inside the program so that it can make full use of the multi-core computer without actually bringing too much overhead of threads.



### Program 4: Iterative `sqrt`

#### Random situations

Under random data in the vector, SIMD speeds up the program only about 4.86x. This may be due to the iterations of each number isn't equal. Some numbers take significantly more iterations, reducing the utilization of SIMD vector. For optimization, considering putting data of similar iterations may be better.

#### Best situations

Array with same numbers is undoubtedly the best choice. As for the choice of the identical element, using $2.999$(6.94x) is a lot better than $1$(<2x). This may be caused by the overhead of creating threads and scheduling tasks. The more iterations it runs, the less proportion of overhead takes, thus the more speedup it produces.

#### Worst situations

Since AVX2 utilizes float point vector of 8 elements, an array that has $2.999$ in every 8 consecutive elements, and $1$ for the rest will make most elements of the SIMD vector idle for the most of the time, resulting in a <1x speedup(about 0.86x speedup indeed), since calculating masks also takes time.

#### Extra credits

Extra credits ask you to manually write a SIMD function based on the AVX and AVX2 SIMD extension. I didn't check the mask at the border(since N is a multiple of 8), and it results in a function even faster than the ISPC. By the way, Intel's Intrinsics document is **horrible**. It can only be better understood with GPT.



### Program 5: BLAS `saxpy`

**Q: Can the program be substantially improved? **

**A: ** Not, since the memory bandwidth is drained in this program, the bottleneck is the memory fetch and write rather than compute. Thus, both SIMD and SIMD with tasks can only achieve speedup of <2x. (What a pity! It seems so suitable for parallel computing QAQ)



#### Program 6: Making `K-Means` Faster

Using clock provided in the `common/` directory, it can be easily found out that the `computeAssignments` function is the bottleneck. By parallelizing the function, we can achieve about 1.5x-2x speedup(the former with worse space locality and the latter with good space locality per thread). Since the asset asks you to reach 2.1x speedup, I did a little black magic, and use SIMD to optimize the computation of the `dist` function, and finally reach a 2.5x speedup. ~~Don't follow me~~