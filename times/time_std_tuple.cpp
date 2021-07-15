#include <tuple>

#include <iostream>

int main() {
    {
        std::tuple<int, int, int> tup {0, 1, 2}; 

        std::cout << std::tuple_size<decltype(tup)>::value << std::endl;
        
        for(int i=0; i<2; i++) {
            auto [a, b, c] {tup};
            std::get<0>(tup) = 3;
            std::get<1>(tup) = 4;
            std::get<2>(tup) = 5;
            std::cout << a << ' ' << b << ' ' << c << std::endl;
        }
    }

    {
        std::tuple<int, int, int> tup {0, 1, 2}; 

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
        std::tuple<long long, int, int> stup {0, 1, 2};
        
        std::cout << sizeof(std::tuple<>{}) << ' ' << sizeof(stup) << std::endl;
        std::cout << &std::get<0>(stup) << ' ' << &std::get<1>(stup) << ' ' << &std::get<2>(stup) << std::endl;

        // std::cout << std::get<int>(tup) << std::endl; // compilation error
        std::cout << std::get<long long>(stup) << std::endl;
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
        std::tuple<long long, long long, long long> tup {std::tuple<int, int, int>{0, 1, 2}};

        auto [a, b, c] {tup};
        std::cout << a << ' ' << b << ' ' << c << std::endl;
    }    
}