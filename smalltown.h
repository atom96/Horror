#ifndef SMALLTOWN_H
#define SMALLTOWN_H

#include "citizen.h"
#include "monster.h"
#include <tuple>
#include <iostream>
#include <algorithm>

/**
 * Struct which has static field - n-th Fibonacci number
 */
template <typename T, int n>
struct Fibonacci {
    static const T value = Fibonacci<T, n - 1>::value + Fibonacci<T, n - 2>::value;
};

template <typename T>
struct Fibonacci<T, 1> {
    static const T value = 1;
};

template <typename T>
struct Fibonacci<T, 0> {
    static const T value = 1;
};

/**
 * Template function which calculate in compile-time
 * how many Fibonacci numbers we have to generate
 */
template <typename T, unsigned int i, T num>
constexpr
typename std::enable_if<Fibonacci<T, i>::value >= num, unsigned int>::type
howManyFib() {
    return i + 1;
}

template <typename T, unsigned int i, T num>
constexpr
typename std::enable_if<Fibonacci<T, i>::value < num, unsigned int>::type
howManyFib() {
    return howManyFib<T, i+1, num>();
}

/**
 * Template function - generates std::array of N Fibonacci numbers
 */

template <typename T, unsigned int N, T ...Vals>
constexpr
typename std::enable_if<N==sizeof...(Vals),std::array<T, N>>::type
makeFibArray() {
    return std::move(std::array<T, N>{{Vals...}});
}

template <typename T, unsigned int N, T ...Vals>
constexpr
typename std::enable_if<N!=sizeof...(Vals), std::array<T, N>>::type
makeFibArray() {
    return makeFibArray<T, N, Vals..., Fibonacci<T, sizeof...(Vals)>::value>();
}

/**
 * Struct which allow us to run function and give I-th tuple element
 * as parameter
 */
template <size_t I>
struct getImpl {
    template <typename T, typename F>
    static void get(T& tup, size_t index, F func) {
        if (index == I - 1) func(std::get<I - 1>(tup));
        else getImpl<I - 1>::get(tup, index, func);
    }
};

template <>
struct getImpl<0> {
    template <typename T, typename F>
    static void get(T& tup, size_t index, F func) { assert(false); }
};

template <typename F, typename... Ts>
void getAt(std::tuple<Ts...> const& tup, size_t index, F func) {
    getImpl<sizeof...(Ts)>::get(tup, index, func);
}

template <typename F, typename... Ts>
void getAt(std::tuple<Ts...>& tup, size_t index, F func) {
    getImpl<sizeof...(Ts)>::get(tup, index, func);
}

template <typename M, typename U, U t0, U t1, typename ...C>
class SmallTown {
    static_assert(std::is_arithmetic<U>::value, "Only arithmetic types are allowed");
    static_assert(t0 >= 0 && t1 > 0 && t1 > 0, "Incorrect time parameters");
private:
    M _monster;
    U _startTime;
    U _endTime;
    U _actTime;
    size_t aliveCitizens;
    std::tuple<C...> _citizens;
    size_t tupleSize = sizeof...(C);
    static const unsigned int howManyFibs = howManyFib<U, 0, t1>();
    std::array<U, howManyFibs> fibonacciNumbers;

/**
 * Functor which is passed to template function which iterates over a tuple.
 * Counts how many citizens are alive.
 */
    struct countAliveCitizens {
    private:
        size_t& _x;
    public:
        countAliveCitizens(size_t& x) : _x(x) {}

        template <typename T>
        void operator()(T& v) {
            if (v.getHealth() > 0) {
                ++_x;
            }
        }
    };

/**
 * Another functor. Performs attack and also counts how many citizens
 * are still alive.
 */
    struct performAttack {
    private:
        size_t& _x;
        M& _monster;
    public:
        performAttack(size_t& x, M& monster) : _x(x), _monster(monster) {}


        template <typename T>
        void operator()(T& v) {
            attack(_monster, v);
            if (v.getHealth() > 0) {
                ++_x;
            }
        }
    };

    void updateLivingCitizens() {
        aliveCitizens = 0;
        for(size_t i = 0; i < tupleSize; ++i) {
            getAt(_citizens, i, countAliveCitizens(aliveCitizens));
        }
    }

    bool isFib(U time) {
        return std::binary_search(fibonacciNumbers.begin(), fibonacciNumbers.end(), time);
    }

public:
    SmallTown(M monster, C... citizens): _monster(monster),_startTime(t0), _endTime(t1), _actTime(t0),  _citizens{citizens...}  {

        constexpr auto constFibonacciNumbers = makeFibArray<U, howManyFibs>();
        fibonacciNumbers = std::move(constFibonacciNumbers);
        updateLivingCitizens();
    }

    std::tuple<std::string, typename M::valueType, size_t> getStatus() {
        return std::make_tuple(_monster.getTypeName(), _monster.getHealth(), aliveCitizens);
    }

    void tick(U timeStep) {
        assert(timeStep >= 0);
        if(_monster.getHealth() == 0 && aliveCitizens == 0) {
            std::cout<<"DRAW"<<std::endl;
        } else if (_monster.getHealth() == 0) {
            std::cout<<"CITIZENS WON"<<std::endl;
        } else if (aliveCitizens == 0) {
            std::cout<<"MONSTER WON"<<std::endl;
        } else if (isFib(_actTime)) {
            aliveCitizens = 0;

            for(size_t i = 0; i < tupleSize; ++i) {
                getAt(_citizens, i, performAttack(aliveCitizens, _monster));
            }
        }

        _actTime = (_actTime + timeStep) % (_endTime + 1);
    }
};

#endif
