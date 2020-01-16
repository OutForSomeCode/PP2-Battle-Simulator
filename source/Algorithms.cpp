#include "Algorithms.h"

using namespace std;

mutex dfdgfdgfdsgfsgfgs;

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
    Tank* kdkdf = median(input);
    KD_Tree::insertTank(kdkdf);
    for (Tank* tank : input)
    {
        if (tank != kdkdf and tank->active)
            KD_Tree::insertTank(tank);
    }
}

// -----------------------------------------------------------
// Sort tanks by x(coordinates) value using bucket sort for even KD_tree distribution
// -----------------------------------------------------------
Tank* KD_Tree::median(vector<Tank*>& input)
{
    std::nth_element(input.begin(), input.begin() + 640, input.end(),
                     [&](Tank* a, Tank* b) {
                         return a->position.sqrLength() < b->position.sqrLength();
                     });

    return input[640];
}

// Inserts a new node and returns root of modified tree
// The parameter depth is used to decide axis of comparison
KD_node* KD_Tree::insertRec(KD_node* _root, Tank* tank, unsigned depth)
{
    // Tree is empty?
    if (_root == nullptr)
        return new KD_node(tank);

    // Calculate current dimension of comparison
    // Compare the new point with root on current dimension 'cd'
    // and decide the left or right subtree
    if (depth % 2 == 0 ? tank->position.x < _root->tank->position.x
                       : tank->position.y < _root->tank->position.y)
        _root->left = insertRec(_root->left, tank, depth + 1);
    else
        _root->right = insertRec(_root->right, tank, depth + 1);

    return _root;
}

// Function to insert a new point with given _tank in
// KD Tree and return new root. It mainly uses above recursive
// function "insertRec()"
void KD_Tree::insertTank(Tank* _tank)
{
    root = insertRec(root, _tank, 0);
}

// Searches a Point represented by "_tank" in the K D tree.
// The parameter depth is used to determine current axis.
Tank* KD_Tree::searchRec(KD_node* _root, Tank* _tank, unsigned depth)
{
    if (_root == nullptr || _root->tank == nullptr)
        return closest_Tank;

    float sqrDist = fabsf((_tank->Get_Position() - _root->tank->Get_Position()).sqrLength());
    if (sqrDist < min_distance)
    {
        min_distance = sqrDist;
        closest_Tank = _root->tank;
    }

    //if (_root->left || _root->right)
    // return closest_Tank;

    // Current dimension is computed using current depth and total
    // Compare point with root with respect to cd (Current dimension)
    if (depth % 2 == 0 ? _tank->Get_Position().x < _root->tank->Get_Position().x
                       : _tank->Get_Position().y < _root->tank->Get_Position().y)
    {
        if (_root->left != nullptr)
            return searchRec(_root->left, _tank, depth + 1);
        else
            return closest_Tank;
    }
    else
    {
        if (_root->right != nullptr)
            return searchRec(_root->right, _tank, depth + 1);
        else
            return closest_Tank;
    }
}

// Searches a Point in the K D tree. It mainly uses
// searchRec()
Tank* KD_Tree::findClosestTank(Tank* _tank)
{
    closest_Tank = root->tank;
    min_distance = numeric_limits<float>::infinity();
    // Pass current depth as 0
    return searchRec(root, _tank, 0);
}
}; // namespace PP2
