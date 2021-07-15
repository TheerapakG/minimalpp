#include "minpp/tuple.h"

#include <iostream>

int main() {
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
        minpp::tuple<long long, int, int> mtup {0, 1, 2};
        
        std::cout << sizeof(minpp::tuple<>{}) << ' ' << sizeof(mtup) << std::endl;
        std::cout << &minpp::get<0>(mtup) << ' ' << &minpp::get<1>(mtup) << ' ' << &minpp::get<2>(mtup) << std::endl;

        // std::cout << minpp::get<int>(tup) << std::endl; // compilation error
        std::cout << minpp::get<long long>(mtup) << std::endl;
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
        minpp::tuple<long long, long long, long long> tup {minpp::tuple<int, int, int>{0, 1, 2}};

        auto [a, b, c] {tup};
        std::cout << a << ' ' << b << ' ' << c << std::endl;
    }    
}