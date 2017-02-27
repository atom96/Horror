#ifndef CITIZEN_H
#define CITIZEN_H

#include <cassert>
#include <type_traits>


static constexpr unsigned int minAgeNotTeenager = 18;
static constexpr unsigned int maxAgeNotTeenager = 100;
static constexpr unsigned int minAgeTeenager = 11;
static constexpr unsigned int maxAgeTeenager = 17;

template <typename T, int minAge, int maxAge, bool canFight>

class Citizen {
    static_assert(std::is_arithmetic<T>::value, "Only arithmetic types are allowed");
    T _health;
    T _age;
    T _attackPower;
public:

    template <typename U = T>
    Citizen(typename std::enable_if<!canFight, U>::type health, T age) : _health(health), _age(age) {
        static_assert(std::is_same<U, T>::value, "Types are not the same");
        assert(age >= minAge && age <= maxAge && health > 0);
    }

    template <typename U = T>
    Citizen(typename std::enable_if<canFight, U>::type health, T age, T attackPower) : _health(health), _age(age), _attackPower(attackPower) {
        static_assert(std::is_same<U, T>::value, "Types are not the same");
        assert(age >= minAge && age <= maxAge && health > 0 && attackPower >= 0);
    }

    T getHealth() {
        return _health;
    }

    T getAge() {
        return _age;
    }

    template <typename U = T>
    typename std::enable_if<canFight, U>::type
    getAttackPower() {
        static_assert(std::is_same<U, T>::value, "Types are not the same");
        return _attackPower;
    }

    void takeDamage(T damage) {
        if(_health - damage <= 0) {
            _health = 0;
        } else {
            _health = _health - damage;
        }
    }
};

template <typename T>
using Teenager = Citizen<T, minAgeTeenager, maxAgeTeenager, false>;

template <typename T>
using Adult = Citizen<T, minAgeNotTeenager, maxAgeNotTeenager, false>;

template <typename T>
using Sheriff = Citizen<T, minAgeNotTeenager, maxAgeNotTeenager, true>;

#endif