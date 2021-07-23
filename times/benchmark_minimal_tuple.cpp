#include <benchmark/benchmark.h>

#include "minpp/tuple.h"
#include <tuple>

#include <random>

static void BM_minpp_get_assign(benchmark::State& state) {
  std::random_device seed;
  std::mt19937 gen{seed()};
  std::uniform_int_distribution rand;

  int a = rand(gen), b = rand(gen), c = rand(gen);

  minpp::tuple<int, int, int> tup {0, 1, 2}; 
  
  for (auto _ : state) {
    minpp::get<0>(tup) = a;
    minpp::get<1>(tup) = b;
    minpp::get<2>(tup) = c;
  }
}

static void BM_std_get_assign(benchmark::State& state) {
  std::random_device seed;
  std::mt19937 gen{seed()};
  std::uniform_int_distribution rand;

  int a = rand(gen), b = rand(gen), c = rand(gen);

  std::tuple<int, int, int> tup {0, 1, 2}; 
  
  for (auto _ : state) {
    std::get<0>(tup) = a;
    std::get<1>(tup) = b;
    std::get<2>(tup) = c;
  }
}

static void BM_minpp_get_get(benchmark::State& state) {
  std::random_device seed;
  std::mt19937 gen{seed()};
  std::uniform_int_distribution rand;

  minpp::tuple<int, int, int> tup {rand(gen), rand(gen), rand(gen)}; 

  for (auto _ : state) {
    auto [a, b, c] {tup};
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    benchmark::DoNotOptimize(c);
  }
}

static void BM_std_get_get(benchmark::State& state) {
  std::random_device seed;
  std::mt19937 gen{seed()};
  std::uniform_int_distribution rand;

  std::tuple<int, int, int> tup {rand(gen), rand(gen), rand(gen)}; 

  for (auto _ : state) {
    auto [a, b, c] {tup};
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(b);
    benchmark::DoNotOptimize(c);
  }
}

static void BM_minpp_construct_copy(benchmark::State& state) {
  std::random_device seed;
  std::mt19937 gen{seed()};
  std::uniform_int_distribution rand;
  
  const int a = rand(gen), b = rand(gen), c = rand(gen);

  for (auto _ : state) {
    minpp::tuple<int, int, int> tup {a, b, c}; 
    benchmark::DoNotOptimize(tup);
  }
}

static void BM_std_construct_copy(benchmark::State& state) { 
  std::random_device seed;
  std::mt19937 gen{seed()};
  std::uniform_int_distribution rand;

  const int a = rand(gen), b = rand(gen), c = rand(gen);

  for (auto _ : state) {
    std::tuple<int, int, int> tup {a, b, c}; 
    benchmark::DoNotOptimize(tup);
  }
}

static void BM_minpp_construct_move(benchmark::State& state) { 
  std::random_device seed;
  std::mt19937 gen{seed()};
  std::uniform_int_distribution rand;

  int a = rand(gen), b = rand(gen), c = rand(gen);

  for (auto _ : state) {
    minpp::tuple<int, int, int> tup {std::move(a), std::move(b), std::move(c)}; 
    benchmark::DoNotOptimize(tup);
  }
}

static void BM_std_construct_move(benchmark::State& state) {  
  std::random_device seed;
  std::mt19937 gen{seed()};
  std::uniform_int_distribution rand;
    
  int a = rand(gen), b = rand(gen), c = rand(gen);

  for (auto _ : state) {
    std::tuple<int, int, int> tup {std::move(a), std::move(b), std::move(c)}; 
    benchmark::DoNotOptimize(tup);
  }
}

static void BM_minpp_construct_other_copy(benchmark::State& state) {
  std::random_device seed;
  std::mt19937 gen{seed()};
  std::uniform_int_distribution rand;
  
  const minpp::tuple<int, int, int> src{rand(gen), rand(gen), rand(gen)};

  for (auto _ : state) {
    minpp::tuple<long, long, long> tup {src}; 
    benchmark::DoNotOptimize(tup);
  }
}

static void BM_std_construct_other_copy(benchmark::State& state) {
  std::random_device seed;
  std::mt19937 gen{seed()};
  std::uniform_int_distribution rand;

  const std::tuple<int, int, int> src{rand(gen), rand(gen), rand(gen)};

  for (auto _ : state) {
    std::tuple<long, long, long> tup {src}; 
    benchmark::DoNotOptimize(tup);
  }
}

static void BM_minpp_construct_other_move(benchmark::State& state) {
  std::random_device seed;
  std::mt19937 gen{seed()};
  std::uniform_int_distribution rand;
    
  const minpp::tuple<int, int, int> src{rand(gen), rand(gen), rand(gen)};

  for (auto _ : state) {
    minpp::tuple<long, long, long> tup {std::move(src)}; 
    benchmark::DoNotOptimize(tup);
  }
}

static void BM_std_construct_other_move(benchmark::State& state) { 
  std::random_device seed;
  std::mt19937 gen{seed()};
  std::uniform_int_distribution rand;
  
  const std::tuple<int, int, int> src{rand(gen), rand(gen), rand(gen)};
  
  for (auto _ : state) {
    std::tuple<long, long, long> tup {std::move(src)}; 
    benchmark::DoNotOptimize(tup);
  }
}

static void BM_minpp_copy_assign(benchmark::State& state) {
  std::random_device seed;
  std::mt19937 gen{seed()};
  std::uniform_int_distribution rand;
    
  minpp::tuple<int, int, int> tup {0, 1, 2}; 
  const minpp::tuple<int, int, int> src{rand(gen), rand(gen), rand(gen)};

  for (auto _ : state) {
    tup = src;
    benchmark::DoNotOptimize(tup);
  }
}

static void BM_std_copy_assign(benchmark::State& state) { 
  std::random_device seed;
  std::mt19937 gen{seed()};
  std::uniform_int_distribution rand;
  
  std::tuple<int, int, int> tup {0, 1, 2}; 
  const std::tuple<int, int, int> src{rand(gen), rand(gen), rand(gen)};
  
  for (auto _ : state) {
    tup = src;
    benchmark::DoNotOptimize(tup);
  }
}

static void BM_minpp_move_assign(benchmark::State& state) {
  std::random_device seed;
  std::mt19937 gen{seed()};
  std::uniform_int_distribution rand;
    
  minpp::tuple<int, int, int> tup {0, 1, 2}; 
  const minpp::tuple<int, int, int> src{rand(gen), rand(gen), rand(gen)};

  for (auto _ : state) {
    tup = std::move(src);
    benchmark::DoNotOptimize(tup);
  }
}

static void BM_std_move_assign(benchmark::State& state) { 
  std::random_device seed;
  std::mt19937 gen{seed()};
  std::uniform_int_distribution rand;
  
  std::tuple<int, int, int> tup {0, 1, 2}; 
  const std::tuple<int, int, int> src{rand(gen), rand(gen), rand(gen)};
  
  for (auto _ : state) {
    tup = std::move(src);
    benchmark::DoNotOptimize(tup);
  }
}

static void BM_minpp_tuple_cat(benchmark::State& state) {
  std::random_device seed;
  std::mt19937 gen{seed()};
  std::uniform_int_distribution rand;
    
  minpp::tuple<int, int, int> tup1{rand(gen), rand(gen), rand(gen)}; 
  minpp::tuple<int, int, int> tup2{rand(gen), rand(gen), rand(gen)}; 
  minpp::tuple<int, int, int> tup3{rand(gen), rand(gen), rand(gen)}; 
  minpp::tuple<int, int, int> tup4{rand(gen), rand(gen), rand(gen)}; 

  for (auto _ : state) {
    auto tup = minpp::tuple_cat(tup1, tup2, tup3, tup4);
    benchmark::DoNotOptimize(tup);
  }
}

static void BM_std_tuple_cat(benchmark::State& state) { 
  std::random_device seed;
  std::mt19937 gen{seed()};
  std::uniform_int_distribution rand;
    
  std::tuple<int, int, int> tup1{rand(gen), rand(gen), rand(gen)}; 
  std::tuple<int, int, int> tup2{rand(gen), rand(gen), rand(gen)}; 
  std::tuple<int, int, int> tup3{rand(gen), rand(gen), rand(gen)}; 
  std::tuple<int, int, int> tup4{rand(gen), rand(gen), rand(gen)}; 

  for (auto _ : state) {
    auto tup = std::tuple_cat(tup1, tup2, tup3, tup4);
    benchmark::DoNotOptimize(tup);
  }
}

BENCHMARK(BM_minpp_get_assign);
BENCHMARK(BM_std_get_assign);

BENCHMARK(BM_minpp_get_get);
BENCHMARK(BM_std_get_get);

BENCHMARK(BM_minpp_construct_copy);
BENCHMARK(BM_std_construct_copy);

BENCHMARK(BM_minpp_construct_move);
BENCHMARK(BM_std_construct_move);

BENCHMARK(BM_minpp_construct_other_copy);
BENCHMARK(BM_std_construct_other_copy);

BENCHMARK(BM_minpp_construct_other_move);
BENCHMARK(BM_std_construct_other_move);

BENCHMARK(BM_minpp_copy_assign);
BENCHMARK(BM_std_copy_assign);

BENCHMARK(BM_minpp_move_assign);
BENCHMARK(BM_std_move_assign);

BENCHMARK(BM_minpp_tuple_cat);
BENCHMARK(BM_std_tuple_cat);

BENCHMARK_MAIN();
