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
    std::vector<Tank*> activeTanks = {};

    // only use active tanks for building the KD tree
    for (auto tank : input)
        if (tank->active)
            activeTanks.emplace_back(tank);

    root = InsertTank(activeTanks, 0);
}
// Inserts list of tanks in the tree and return the root
// The parameter depth is used to decide axis of comparison
KD_node* KD_Tree::InsertTank(std::vector<Tank*> input, unsigned depth)
{
    // Tree is empty?
    if (input.size() == 1)
        return new KD_node(input[0]);

    if (input.empty())
        return nullptr;

    unsigned median = input.size() / 2;
    unsigned axis = depth % 2;

    // Sort input based on current depth
    sort(input.begin(), input.end(), [axis](Tank* a, Tank* b) {
        return a->position[axis] < b->position[axis];
    });

    Tank* tank = input[median];
    input.erase(input.begin() + median);
    vector<Tank*> left(input.begin(), input.begin() + (input.size() / 2));
    vector<Tank*> right(input.begin() + ((input.size() / 2) + 1), input.end());

    KD_node* newTank = new KD_node(tank);
    newTank->left = InsertTank(left, depth + 1);
    newTank->right = InsertTank(right, depth + 1);

    return newTank;
}

// Searches the closest enemy tank in the K D tree.
Tank* KD_Tree::findClosestTankV2(Tank* tank)
{
    Tank* closestTank = root->tank;
    float max = numeric_limits<float>::infinity();
    vec2<> hyperplane[] = {vec2<>(0, 2000), vec2<>(2000, 0)};
    // Pass current depth as 0
    return searchNN(root, tank, hyperplane, max, nullptr, 0, closestTank, max);
}

Tank* KD_Tree::searchNN(KD_node* currentNode, Tank* target, vec2<> hyperplane[], float distanceCurrentClosestTank, Tank* currentClosestTank, unsigned depth, Tank* closestTank, float distanceClosestTank)
{
#ifdef USING_EASY_PROFILER
    //EASY_BLOCK("searchNN", profiler::colors::Red);
#endif
    if (currentNode == nullptr)
        return closestTank;

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
    if (dist < distanceCurrentClosestTank)
    {
        currentClosestTank = currentNode->tank;
        distanceCurrentClosestTank = dist;
    }

    searchNN(closestNode, target, closestHyperplane, distanceCurrentClosestTank, currentClosestTank, depth + 1, closestTank, distanceClosestTank);

    if (distanceCurrentClosestTank < distanceClosestTank)
    {
        closestTank = currentClosestTank;
        distanceClosestTank = distanceCurrentClosestTank;
    }

    float pointX = calculateCC(target->position[0], furthestHyperplane[0][0], furthestHyperplane[1][0]);
    float pointY = calculateCC(target->position[1], furthestHyperplane[1][1], furthestHyperplane[0][1]);

    dist = pow((pointX - target->position[0]), 2) + pow((pointY - target->position[1]), 2);

    if (dist < distanceClosestTank)
        searchNN(furthestNode, target, furthestHyperplane, distanceCurrentClosestTank, currentClosestTank, depth + 1, closestTank, distanceClosestTank);

    return closestTank;
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
