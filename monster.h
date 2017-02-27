#ifndef MONSTER_H
#define MONSTER_H

#include <string>
#include "citizen.h"

enum monsterType {
    typeMummy,
    typeZombie,
    typeVampire
};

std::string getName(monsterType type) {
    switch(type) {
        case typeMummy:
            return "Mummy";
        case typeZombie:
            return "Zombie";
        case typeVampire:
            return "Vampire";
        default:
            assert(false);
    }
}

template <typename T, monsterType type>
class Monster {
    static_assert(std::is_arithmetic<T>::value, "Only arithmetic types are allowed");
    T _health;
    T _attackPower;
public:
    typedef T valueType;

    Monster(T health, T attackPower) : _health(health), _attackPower(attackPower) {
        assert(health > 0 && attackPower >= 0);
    }

    T getHealth() {
        return _health;
    }

    T getAttackPower() {
        return _attackPower;
    }

    void takeDamage(T damage) {
        if(_health - damage <= 0) {
            _health = 0;
        } else {
            _health = _health - damage;
        }
    }

    std::string getTypeName() {
        return getName(type);
    }
};


template <typename T>
using Mummy = Monster<T, typeMummy>;

template <typename T>
using Zombie = Monster<T, typeZombie>;

template <typename T>
using Vampire = Monster<T, typeVampire>;




template <typename M, typename U>
void attack(M& monster, U& victim) {
    victim.takeDamage(monster.getAttackPower());
}

template <typename M,  typename T>
void attack(M& monster, Sheriff<T>& victim) {
    if(victim.getHealth() > 0) {
        monster.takeDamage(victim.getAttackPower());
    }

    victim.takeDamage(monster.getAttackPower());
}

#endif