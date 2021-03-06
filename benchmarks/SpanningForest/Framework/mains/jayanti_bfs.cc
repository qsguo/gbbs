// This code is part of the project "Theoretically Efficient Parallel Graph
// Algorithms Can Be Fast and Scalable", presented at Symposium on Parallelism
// in Algorithms and Architectures, 2018.
// Copyright (c) 2018 Laxman Dhulipala, Guy Blelloch, and Julian Shun
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all  copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "benchmarks/SpanningForest/Framework/framework.h"
#include "benchmarks/SpanningForest/BFSSF/SpanningForest.h"
#include "benchmarks/SpanningForest/common.h"

#include "bench_utils.h"
#include "uf_utils.h"

namespace connectit {
template <class Graph>
void jayanti_find_twotrysplit(Graph& G, int rounds, commandLine& P, pbbs::sequence<edge>& correct) {
    run_multiple_jayanti_alg<Graph, sample_bfs, find_twotrysplit>(G, rounds, correct,  P);
}

template <class Graph>
void jayanti_find_simple(Graph& G, int rounds, commandLine& P, pbbs::sequence<edge>& correct) {
    run_multiple_jayanti_alg<Graph, sample_bfs, find_simple>(G, rounds, correct,  P);
}

}

/* Not sure how to supply type of F w-out decltype or supplying one entry as an
 * argument like below... */
template <class F, class Graph>
void run_tests(Graph& G, int rounds, commandLine& P, pbbs::sequence<edge>& correct,
    F test,
    std::initializer_list<F> tests) {
  for (auto test : tests) {
#ifdef USE_PCM_LIB
  auto before_state = get_pcm_state();
  timer ot; ot.start();
#endif
    test(G, rounds, P, correct);
#ifdef USE_PCM_LIB
  double elapsed = ot.stop();
  auto after_state = get_pcm_state();
  cpu_stats stats = get_pcm_stats(before_state, after_state, elapsed, rounds);
  print_cpu_stats(stats, P);
#endif
  }
}

template <class Graph>
double Benchmark_runner(Graph& G, commandLine P) {
  int test_num = P.getOptionIntValue("-t", -1);
  int rounds = P.getOptionIntValue("-r", 5);

  auto correct = pbbs::sequence<edge>();
  if (P.getOptionValue("-check")) {
    correct = bfs_sf::SpanningForestDet(G);
  }
  run_tests(G, rounds, P, correct, connectit::jayanti_find_simple<Graph>,
    {
      connectit::jayanti_find_simple<Graph>,
      connectit::jayanti_find_twotrysplit<Graph>
    });
  return 1.0;
}

generate_symmetric_once_main(Benchmark_runner, false);
