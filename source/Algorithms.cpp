#include "Algorithms.h"
#include <cmath>

#ifdef USING_EASY_PROFILER
#include <easy/profiler.h>
#endif

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
    /*Tank* _median = Median(input);
    KD_Tree::InsertTank(_median);
    for (Tank* tank : input)
    {
        if (tank != _median && tank->active)
            KD_Tree::InsertTank(tank);
    }*/
    root = InsertTank(input, 0);
}
// Inserts a new node and returns root of modified tree
// The parameter depth is used to decide axis of comparison
KD_node* KD_Tree::InsertTank(std::vector<Tank*>& input, unsigned depth)
{
    // Tree is empty?
    if (input.empty())
        return nullptr;

    unsigned k = depth % 2;

    std::sort(input.begin(), input.end(), [k](Tank* a, Tank* b) {
        return a->position[k] < b->position[k];
    });

    /*// Calculate current dimension of comparison
    // Compare the new point with root on current dimension 'cd'
    // and decide the left or right subtree
    if (depth % 2 == 0 ? tank->position.x < currentNode->tank->position.x
                       : tank->position.y < currentNode->tank->position.y)
        currentNode->left =;
    else
        currentNode->right = ;*/

    unsigned hsize = input.size() / 2;

    Tank* tank = input[hsize];
    std::vector<Tank*> front(input.begin(), input.begin() + hsize);
    std::vector<Tank*> back(input.begin() + hsize + 1, input.end());

    return new KD_node(tank, InsertTank(front, depth + 1), InsertTank(back, depth + 1));
}

// Searches a Point represented by "_tank" in the K D tree.
// The parameter depth is used to determine current axis.
Tank* KD_Tree::searchRec(KD_node* currentNode, Tank* tank, unsigned depth, Tank* closest_Tank, float distance_Closest_Tank)
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
            return searchRec(currentNode->left, tank, depth + 1, closest_Tank, distance_Closest_Tank);
        else
            return closest_Tank;
    }
    else
    {
        if (currentNode->right != nullptr)
            return searchRec(currentNode->right, tank, depth + 1, closest_Tank, distance_Closest_Tank);
        else
            return closest_Tank;
    }
}

// Searches a Point in the K D tree. It mainly uses
// searchRec()
Tank* KD_Tree::findClosestTank(Tank* tank)
{
    Tank* closest_Tank = root->tank;
    float distance_Closest_Tank = numeric_limits<float>::infinity();
    // Pass current depth as 0
    return searchRec(root, tank, 0, closest_Tank, distance_Closest_Tank);
}

Tank* KD_Tree::findClosestTankV2(Tank* tank)
{
    Tank* closest_Tank = root->tank;
    float distance_Closest_Tank = numeric_limits<float>::infinity();
    vec2<> hyperplane[] = {vec2<>(0, 2000), vec2<>(2000, 0)};
    // Pass current depth as 0
    return searchNN(root, tank, hyperplane, distance_Closest_Tank, nullptr, 0, closest_Tank, distance_Closest_Tank);
}

Tank* KD_Tree::searchNN(KD_node* currentNode, Tank* target, vec2<> hyperplane[], float distance, Tank* nearest, unsigned depth, Tank* closest_Tank, float distance_Closest_Tank)
{
#ifdef USING_EASY_PROFILER
    //EASY_BLOCK("searchNN", profiler::colors::Red);
#endif
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

    searchNN(closestNode, target, closestHyperplane, distance, nearest, depth + 1, closest_Tank, distance_Closest_Tank);

    if (distance < distance_Closest_Tank)
    {
        closest_Tank = nearest;
        distance_Closest_Tank = distance;
    }

    float pointX = calculateCC(target->position[0], furthestHyperplane[0][0], furthestHyperplane[1][0]);
    float pointY = calculateCC(target->position[1], furthestHyperplane[1][1], furthestHyperplane[0][1]);

    dist = pow((pointX - target->position[0]), 2) + pow((pointY - target->position[1]), 2);

    if (dist < distance_Closest_Tank)
        searchNN(furthestNode, target, furthestHyperplane, distance, nearest, depth + 1, closest_Tank, distance_Closest_Tank);

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

void KD_Tree::bst_print_dot_null(const string& key, int nullcount, FILE* stream)
{
    fprintf(stream, "    null%d [shape=point];\n", nullcount);
    fprintf(stream, "    \"%s\" -> null%d;\n", key.c_str(), nullcount);
}

void KD_Tree::bst_print_dot_aux(KD_node* node, FILE* stream)
{
    static int nullcount = 0;

    if (node->left)
    {
        fprintf(stream, "    \"%s\" -> \"%s\";\n", node->print().c_str(), node->left->print().c_str());
        bst_print_dot_aux(node->left, stream);
    }
    else
        bst_print_dot_null(node->print(), nullcount++, stream);

    if (node->right)
    {
        fprintf(stream, "    \"%s\" -> \"%s\";\n", node->print().c_str(), node->right->print().c_str());
        bst_print_dot_aux(node->right, stream);
    }
    else
        bst_print_dot_null(node->print(), nullcount++, stream);
}

void KD_Tree::bst_print_dot(KD_node* tree, FILE* stream)
{
    fprintf(stream, "digraph BST {\n");
    fprintf(stream, "    node [fontname=\"Arial\"];\n");

    if (!tree)
        fprintf(stream, "\n");
    else if (!tree->right && !tree->left)
        fprintf(stream, "    \"%s\";\n", tree->print().c_str());
    else
        bst_print_dot_aux(tree, stream);

    fprintf(stream, "}\n");
}

}; // namespace PP2
