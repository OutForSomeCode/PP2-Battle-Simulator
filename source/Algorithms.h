#pragma once

#include "Grid.h"
#include "defines.h"
#include "explosion.h"
#include "particle_beam.h"
#include "rocket.h"
#include "smoke.h"
#include "tank.h"
#include <cstdint>
#include <iostream>

namespace PP2
{
template <class T>
class Node
{
  public:
    Node() : value(nullptr), next(nullptr) {}

    Node(T value) : value(value), next(nullptr) {}

    ~Node()
    {
        if (next != nullptr) delete next;
    }

    T value;
    Node<T>* next;
};

template <class T>
class LinkedList
{
  public:
    LinkedList() : head(nullptr){};

    ~LinkedList()
    {
        if (head != nullptr) delete head;
    }

    void InsertValue(T value);

    static std::vector<LinkedList<T>> Sort(std::vector<Tank*>& input, int n_buckets);

    Node<T>* head;
};

class KD_node
{
  public:
    KD_node(Tank* tank) : tank(tank){};
    ~KD_node()
    {
        delete left;
        delete right;
    };
    Tank* tank = nullptr;
    KD_node* right = nullptr;
    KD_node* left = nullptr;
};

class KD_Tree
{
  public:
    explicit KD_Tree(std::vector<Tank*>& input);
    ~KD_Tree()
    {
        delete root;
    };
    static Tank* Median(std::vector<Tank*>& input);
    void InsertTank(Tank* _tank);
    Tank* findClosestTank(Tank* tank);
    Tank* findClosestTankV2(Tank* tank);

  private:
    KD_node* insertRec(KD_node* currentNode, Tank* tank, unsigned depth);
    Tank* searchRec(KD_node* currentNode, Tank* tank, unsigned depth);
    Tank* searchNN(KD_node* currentNode, Tank* target, vec2<> hyperplane[], float distance, Tank* nearest, unsigned depth);
    float calculateCC(float targetXY, float hyperplaneMinXY, float hyperplaneMaxXY);
    KD_node* root = nullptr;
    float distance_Closest_Tank = 0;
    Tank* closest_Tank = nullptr;
};
} // namespace PP2
