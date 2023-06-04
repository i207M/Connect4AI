# ifndef UCT_H
# define UCT_H

#include "general.h"

#define TIME_LIMIT 2.7
#define C 0.5

#define OPPONENT 0
#define FELLOW 1

class State
{
    State *NextState(int x, int y);
    pii RandomPut();

public:
    bool player;
    int nextChoice;
    int *board, *top;
    pii lastPut;
    static int H, W, BanX, BanY;
    static void init(int H, int W, int BanX, int BanY);
    explicit State(bool player);
    State(const State &obj);
    ~State();

    State *NextChoiceState();
    int DefaultPolicy() const;
    void Print();
};


class TreeNode
{
public:
    State *state;
    TreeNode *parent;
    TreeNode *firstChild, *nextSibling;
    int win, visit;
    TreeNode(State *state, TreeNode *parent);
    ~TreeNode();
    void Visit(int delta);
    float UCB1(TreeNode *child);
    TreeNode *Expand();
    TreeNode *BestChild();
};

class UCT
{
    TreeNode *BestChild(TreeNode *node);
    TreeNode *TreePolicy();

public:
    TreeNode *root;
    static int startTime;
    explicit UCT(bool player);
    ~UCT();
    pii GetBestMove();
    void MoveRoot(pii put);
};

# endif // !UCT_H
