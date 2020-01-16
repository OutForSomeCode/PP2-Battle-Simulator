#include "Algorithms.h"

using namespace std;

namespace PP2
{

template <class T>
void LinkedList<T>::InsertValue(T value)
{
    Node<T>* new_node = new Node<T>(value);

    if (head == nullptr || value <= head->value)
    {
        new_node->next = head;
        head = new_node;
        return;
    }

    Node<T>* current = head;
    while (current->next != nullptr && value >= current->next->value) { current = current->next; }

    //Add node
    new_node->next = current->next;
    current->next = new_node;
}

// -----------------------------------------------------------
// Sort tanks by health value using bucket sort
// -----------------------------------------------------------
template <>
vector<LinkedList<int>> LinkedList<int>::Sort(vector<Tank*>& input, int n_buckets)
{
    vector<LinkedList<int>> buckets(n_buckets);
    for (auto tank : input) { buckets.at(tank->health / n_buckets).InsertValue(tank->health); }
    return buckets;
}

KD_Tree::KD_Tree(std::vector<Tank*>& input)
{
    Tank* _median = Median(input);
    KD_Tree::InsertTank(_median);
    for (Tank* tank : input)
    {
        if (tank != _median && tank->active)
            KD_Tree::InsertTank(tank);
    }
}

// -----------------------------------------------------------
// Sort tanks by x(coordinates) value using bucket sort for even KD_tree distribution
// -----------------------------------------------------------
Tank* KD_Tree::Median(vector<Tank*>& input)
{
    std::nth_element(input.begin(), input.begin() + 640, input.end(),
                     [&](Tank* a, Tank* b) {
                         return a->position.sqrLength() < b->position.sqrLength();
                     });

    return input[640];
}

// Inserts a new node and returns root of modified tree
// The parameter depth is used to decide axis of comparison
KD_node* KD_Tree::insertRec(KD_node* currentNode, Tank* tank, unsigned depth)
{
    // Tree is empty?
    if (currentNode == nullptr)
        return new KD_node(tank);

    // Calculate current dimension of comparison
    // Compare the new point with root on current dimension 'cd'
    // and decide the left or right subtree
    if (depth % 2 == 0 ? tank->position.x < currentNode->tank->position.x
                       : tank->position.y < currentNode->tank->position.y)
        currentNode->left = insertRec(currentNode->left, tank, depth + 1);
    else
        currentNode->right = insertRec(currentNode->right, tank, depth + 1);

    return currentNode;
}

// Function to insert a new point with given _tank in
// KD Tree and return new root. It mainly uses above recursive
// function "insertRec()"
void KD_Tree::InsertTank(Tank* tank)
{
    root = insertRec(root, tank, 0);
}

// Searches a Point represented by "_tank" in the K D tree.
// The parameter depth is used to determine current axis.
Tank* KD_Tree::searchRec(KD_node* currentNode, Tank* tank, unsigned depth)
{
    if (currentNode == nullptr || currentNode->tank == nullptr)
        return closest_Tank;

    float sqrDist = fabsf((tank->position - currentNode->tank->position).sqrLength());
    if (sqrDist < min_distance)
    {
        min_distance = sqrDist;
        closest_Tank = currentNode->tank;
    }

    //if (_root->left || _root->right)
    // return closest_Tank;

    // Current dimension is computed using current depth and total
    // Compare point with root with respect to cd (Current dimension)
    if (depth % 2 == 0 ? tank->position.x < currentNode->tank->position.x
                       : tank->position.y < currentNode->tank->position.y)
    {
        if (currentNode->left != nullptr)
            return searchRec(currentNode->left, tank, depth + 1);
        else
            return closest_Tank;
    }
    else
    {
        if (currentNode->right != nullptr)
            return searchRec(currentNode->right, tank, depth + 1);
        else
            return closest_Tank;
    }
}

// Searches a Point in the K D tree. It mainly uses
// searchRec()
Tank* KD_Tree::findClosestTank(Tank* tank)
{
    closest_Tank = root->tank;
    min_distance = numeric_limits<float>::infinity();
    // Pass current depth as 0
    return searchRec(root, tank, 0);
}
}; // namespace PP2
