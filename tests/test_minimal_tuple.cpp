#include "minpp/tuple.h"
#include <tuple>
#include <memory>

#include <iostream>
#include <algorithm>
#include <numeric>

struct print_comp {
  int val;
};

bool operator==(const print_comp& lhs, const print_comp& rhs) {
  std::cout << "comparing eq " << lhs.val << " and " << rhs.val << std::endl;
  return lhs.val == rhs.val;
}

auto operator<=>(const print_comp& lhs, const print_comp& rhs) {
  std::cout << "comparing three way " << lhs.val << " and " << rhs.val << std::endl;
  return lhs.val <=> rhs.val;
}

int main() {
  std::cout << std::boolalpha;

  {
    std::cout << std::integral_constant<int, minpp::get<0>(minpp::tuple<int>{0})>::value << std::endl;
  }

  {
    minpp::tuple<int, int, int> tup {0, 1, 2}; 

    std::cout << std::tuple_size<decltype(tup)>::value << std::endl;
    
    for(int i=0; i<2; i++) {
      auto [a, b, c] {tup};
      minpp::get<0>(tup) = 3;
      minpp::get<1>(tup) = 4;
      minpp::get<2>(tup) = 5;
      std::cout << a << ' ' << b << ' ' << c << std::endl;
    }
  }

  {
    minpp::tuple<int, int, int> tup {0, 1, 2}; 

    std::cout << std::tuple_size<decltype(tup)>::value << std::endl;
    
    for(int i=0; i<2; i++) {
      auto [a, b, c] {tup};
      a = 3;
      b = 4;
      c = 5;
      auto [x, y, z] {tup};
      std::cout << x << ' ' << y << ' ' << z << std::endl;
    }
  }

  {
    {
      std::tuple<size_t, int, int> stup {-1, 1, 2}; // not report narrowing conversion

      std::cout << sizeof(std::tuple<>{}) << ' ' << sizeof(stup) << std::endl;
      std::cout << &std::get<0>(stup) << ' ' << &std::get<1>(stup) << ' ' << &std::get<2>(stup) << std::endl;

      // std::cout << std::get<int>(stup) << std::endl; // compilation error
      std::cout << std::get<size_t>(stup) << std::endl;
    }
    
    {
      minpp::tuple<size_t, int, int> mtup {-1, 1, 2}; // report narrowing conversion
      
      std::cout << sizeof(minpp::tuple<>{}) << ' ' << sizeof(mtup) << std::endl;
      std::cout << &minpp::get<0>(mtup) << ' ' << &minpp::get<1>(mtup) << ' ' << &minpp::get<2>(mtup) << std::endl;

      // std::cout << minpp::get<int>(tup) << std::endl; // compilation error
      std::cout << minpp::get<size_t>(mtup) << std::endl;
    }
  }

  {
    minpp::tuple<int, int, int> tup1 {0, 1, 2}, tup2;

    for(int i=0; i<2; i++) {
      auto [a, b, c] {tup2};
      std::cout << a << ' ' << b << ' ' << c << std::endl;

      tup2 = tup1;
    }
  }

  {
    std::tuple<int, int, int> tup1 {0, 1, 2}, tup2;

    for(int i=0; i<2; i++) {
      auto [a, b, c] {tup2};
      std::cout << a << ' ' << b << ' ' << c << std::endl;

      tup2 = tup1;
    }
  }
  
  {
    minpp::tuple<long, long, long> tup {minpp::tuple<int, int, int>{0, 1, 2}};

    auto [a, b, c] {tup};
    std::cout << a << ' ' << b << ' ' << c << std::endl;
  }

  {
    minpp::tuple<long, long, long> tup {std::tuple<int, int, int>{0, 1, 2}};

    auto [a, b, c] {tup};
    std::cout << a << ' ' << b << ' ' << c << std::endl;
  }

  /* CANNOT CONSTRUCT: USE make_from_tuple INSTEAD
  {
    std::tuple<long, long, long> tup {minpp::tuple<int, int, int>{0, 1, 2}};

    auto [a, b, c] {tup};
    std::cout << a << ' ' << b << ' ' << c << std::endl;
  }
  */

  {
    std::tuple<long, long, long> tup {std::tuple<int, int, int>{0, 1, 2}};

    auto [a, b, c] {tup};
    std::cout << a << ' ' << b << ' ' << c << std::endl;
  }

  {
    minpp::tuple<std::pair<int, int>, std::vector<int>, std::vector<int>, std::vector<int>> tup {
      {1, 2}, 
      {3, 4}, 
      {5}, 
      std::vector<int>(6)
    };

    auto [a, b, c, d] {tup};
    std::cout << a.first << ' ' << a.second << std::endl;
    for(auto i: b) std::cout << i << ' ';
    std::cout << std::endl;
    for(auto i: c) std::cout << i << ' ';
    std::cout << std::endl;
    for(auto i: d) std::cout << i << ' ';
    std::cout << std::endl;
  }

  {
    std::tuple<int, short, long> t1;
    std::tuple<> t2;
    std::tuple<float, double, long double> t3;
    std::tuple<void*, char*> t4;
    auto x = std::tuple_cat(t1, t2, t3, t4, std::make_tuple(std::unique_ptr<int>{}));
    using expected_type = std::tuple<int, short, long, float, double, long double, void*, char*, std::unique_ptr<int>>;
    static_assert(std::is_same<decltype(x), expected_type>::value, "std tuple_cat got wrong tuple type!");
  }

  {
    minpp::tuple<int, short, long> t1;
    minpp::tuple<> t2;
    minpp::tuple<float, double, long double> t3;
    minpp::tuple<void*, char*> t4;
    auto x = minpp::tuple_cat(t1, t2, t3, t4, minpp::make_tuple(std::unique_ptr<int>{}));
    using expected_type = minpp::tuple<int, short, long, float, double, long double, void*, char*, std::unique_ptr<int>>;
    static_assert(std::is_same<decltype(x), expected_type>::value, "minpp tuple_cat got wrong tuple type!");
  }

  {
    static_assert(!std::is_copy_assignable_v<std::tuple<std::unique_ptr<int>>>, "std tuple can copy assign unique_ptr");
    static_assert(!std::is_copy_assignable_v<minpp::tuple<std::unique_ptr<int>>>, "minpp tuple can copy assign unique_ptr");
  }
  
  {
    std::vector<int> vec(1024);
    std::iota(vec.begin(), vec.end(), 0);
    std::cout << std::apply(std::accumulate<std::vector<int>::iterator, int>, std::tuple{vec.begin(), vec.end(), 0}) <<std::endl;
    std::cout << minpp::apply(std::accumulate<std::vector<int>::iterator, int>, std::tuple{vec.begin(), vec.end(), 0}) <<std::endl;
    // std::cout << std::apply(std::accumulate<std::vector<int>::iterator, int>, minpp::tuple{vec.begin(), vec.end(), 0}) <<std::endl;
    std::cout << minpp::apply(std::accumulate<std::vector<int>::iterator, int>, minpp::tuple{vec.begin(), vec.end(), 0}) <<std::endl;
  }

  {
    minpp::tuple<print_comp, print_comp, print_comp, print_comp> tup1{{1}, {2}, {3}, {4}}, tup2{{1}, {2}, {4}, {5}};
    minpp::tuple<print_comp, print_comp, print_comp, print_comp> etup1{{1}, {2}, {3}, {4}}, etup2{{1}, {2}, {3}, {4}};
    std::cout << (tup1 == tup2) << std::endl;
    std::cout << (etup1 == etup2) << std::endl;

    std::cout << (tup1 < tup2) << std::endl;
    std::cout << (etup1 < etup2) << std::endl;

    std::cout << (tup1 > tup2) << std::endl;
    std::cout << (etup1 > etup2) << std::endl;
  }
  
}