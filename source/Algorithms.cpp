#include "Algorithms.h"
#include <cmath>

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
    std::nth_element(input.begin(), input.begin() + input.size() / 2, input.end(),
                     [&](Tank* a, Tank* b) {
                         return a->position.sqrLength() < b->position.sqrLength();
                     });

    return input[input.size() / 2];
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
    if (sqrDist < distance_Closest_Tank)
    {
        distance_Closest_Tank = sqrDist;
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
    distance_Closest_Tank = numeric_limits<float>::infinity();
    // Pass current depth as 0
    return searchRec(root, tank, 0);
}

Tank* KD_Tree::findClosestTankV2(Tank* tank)
{
    closest_Tank = root->tank;
    distance_Closest_Tank = numeric_limits<float>::infinity();
    vec2<> hyperplane[] = {vec2<>(0, 720), vec2<>(1280, 0)};
    // Pass current depth as 0
    return searchNN(root, tank, hyperplane, distance_Closest_Tank, nullptr, 0);
}

Tank* KD_Tree::searchNN(KD_node* currentNode, Tank* target, vec2<> hyperplane[], float distance, Tank* nearest, unsigned depth)
{
    if (currentNode == nullptr)
        return closest_Tank;

    unsigned axis = depth % 2;
    vec2<> leftHyperplane[2] = {}, rightHyperplane[2] = {};
    vec2<> closestHyperplane[2] = {}, furthestHyperplane[2] = {};
    KD_node *closestNode = nullptr, *furthestNode = nullptr;

    if (axis == 0)
    {
        leftHyperplane[0] = hyperplane[0];
        leftHyperplane[1] = vec2<>(currentNode->tank->position[0], hyperplane[1][1]);
        rightHyperplane[0] = vec2<>(currentNode->tank->position[0], hyperplane[0][1]);
        rightHyperplane[1] = hyperplane[1];
    }
    if (axis == 1)
    {
        leftHyperplane[0] = vec2<>(hyperplane[0][0], currentNode->tank->position[1]);
        leftHyperplane[1] = hyperplane[1];
        rightHyperplane[0] = hyperplane[0];
        rightHyperplane[1] = vec2<>(hyperplane[1][0], currentNode->tank->position[1]);
    }
    if (target->position[axis] <= currentNode->tank->position[axis])
    {
        closestNode = currentNode->left;
        furthestNode = currentNode->right;
        memcpy(closestHyperplane, leftHyperplane, sizeof(closestHyperplane));
        memcpy(furthestHyperplane, rightHyperplane, sizeof(furthestHyperplane));
    }
    if (target->position[axis] > currentNode->tank->position[axis])
    {
        closestNode = currentNode->right;
        furthestNode = currentNode->left;
        memcpy(closestHyperplane, rightHyperplane, sizeof(closestHyperplane));
        memcpy(furthestHyperplane, leftHyperplane, sizeof(furthestHyperplane));
    }

    float dist = pow((currentNode->tank->position[0] - target->position[0]) + (currentNode->tank->position[1] - target->position[1]), 2);
    if (dist < distance)
    {
        nearest = currentNode->tank;
        distance = dist;
    }

    searchNN(closestNode, target, closestHyperplane, distance, nearest, depth + 1);

    if (distance < distance_Closest_Tank){
        closest_Tank = nearest;
        distance_Closest_Tank = distance;
    }

    float pointX = calculateCC(target->position[0],furthestHyperplane[0][0], furthestHyperplane[1][0]);
    float pointY = calculateCC(target->position[1],furthestHyperplane[1][1], furthestHyperplane[0][1]);

    dist = pow((pointX - target->position[0]), 2) + pow((pointY - target->position[1]), 2);

    if (dist < distance_Closest_Tank)
        searchNN(furthestNode, target, furthestHyperplane, distance, nearest, depth + 1);

    return closest_Tank;
}
float KD_Tree::calculateCC(float targetXY, float hyperplaneMinXY, float hyperplaneMaxXY)
{
    if (hyperplaneMinXY < targetXY && targetXY < hyperplaneMaxXY)
        return targetXY;
    else if (targetXY <= hyperplaneMinXY)
        return hyperplaneMinXY;
    else if (targetXY >= hyperplaneMaxXY)
        return hyperplaneMaxXY;
}

}; // namespace PP2
