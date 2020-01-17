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
    KD_node(Tank* t) : tank(t){};
    KD_node(Tank* t, KD_node* r, KD_node* l) : tank(t), right(r), left(l){};
    ~KD_node()
    {
        delete left;
        delete right;
    };

    std::string print()
    {
        char buff[50];
        sprintf(buff, "(%g,%g)", tank->position.x, tank->position.y);
        return buff;
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
    Tank* findClosestTank(Tank* tank);
    Tank* findClosestTankV2(Tank* tank);

    void printTree()
    {
        auto pFile = fopen("./TreeDebug.dot", "w");
        bst_print_dot(root, pFile);
        fclose(pFile);
    };

  private:
    KD_node* InsertTank(std::vector<Tank*>& input, unsigned depth);
    float calculateCC(float targetXY, float hyperplaneMinXY, float hyperplaneMaxXY);
    KD_node* root = nullptr;
    //float distance_Closest_Tank = 0;
    //Tank* closest_Tank = nullptr;
    void bst_print_dot(KD_node* tree, FILE* stream);
    void bst_print_dot_aux(KD_node* node, FILE* stream);
    void bst_print_dot_null(const std::string& key, int nullcount, FILE* stream);
    Tank* searchNN(KD_node* currentNode, Tank* target, vec2<>* hyperplane, float distance, Tank* nearest, unsigned int depth, Tank* closest_Tank, float distance_Closest_Tank);
    Tank* searchRec(KD_node* currentNode, Tank* tank, unsigned int depth, Tank* closest_Tank, float distance_Closest_Tank);
};
} // namespace PP2
