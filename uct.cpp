#include "uct.h"
#include "judger.h"

#define board(i, j) board[(i)*W + (j)]

int State::H = 0, State::W = 0, State::BanX = 0, State::BanY = 0;
int UCT::startTime = 0;

void State::init(int H, int W, int BanX, int BanY)
{
    State::H = H;
    State::W = W;
    State::BanX = BanX;
    State::BanY = BanY;
}

State::State(const bool player)
{
    this->player = player;
    board = new int [H * W];
    top = new int[W]; // H: empty, 0: full

    memset(board, 0, sizeof(int) * H * W);
    for (int i = 0; i < W; ++i) {
        top[i] = H;
    }

    if (BanX == H - 1) {
        top[BanY] = H - 1;
    }
    nextChoice = 0;
    lastPut = mp(-1, -1);
}

State::State(const State &obj)
{
    player = obj.player;
    board = new int [H * W];
    top = new int[W];
    memcpy(board, obj.board, sizeof(int) * H * W);
    memcpy(top, obj.top, sizeof(int) * W);
    nextChoice = obj.nextChoice;
    lastPut = obj.lastPut;
}

State::~State()
{
    delete[] board;
    delete[] top;
}

State *State::NextState(int x, int y)
{
    State *ret = new State(*this);
    ret->player = !player;
    ret->board(x, y) = player + 1;
    ret->top[y] = (x - 1 == BanX && y == BanY) ? x - 1 : x;
    ret->nextChoice = 0;
    ret->lastPut = mp(x, y);
    if ((player && machineWin(x, y, H, W, ret->board)) ||
            (!player && userWin(x, y, H, W, ret->board))) {
        ret->nextChoice = W;
    } else {
        for(; ret->nextChoice < W && ret->top[ret->nextChoice] == 0; ++ret->nextChoice);
    }
    return ret;
}

pii State::RandomPut()
{
    int cnt = 0;
    for (int i = 0; i < W; ++i) {
        if (top[i] > 0) {
            ++cnt;
        }
    }
    if (cnt == 0) {
        return mp(-1, -1);
    }
    int r = rand() % cnt;  // TODO: different distribution
    for (int i = 0; i < W; ++i) {
        if (top[i] > 0) {
            if (r == 0) {
                return mp(top[i] - 1, i);
            }
            --r;
        }
    }
    assert(false);
    return mp(-1, -1);
}

State *State::NextChoiceState()
{
    if (nextChoice == W) {
        return nullptr;
    }
    State *ret = NextState(top[nextChoice] - 1, nextChoice);

    // Prepare for next choice
    for(++nextChoice; nextChoice < W && top[nextChoice] == 0; ++nextChoice);

    return ret;
}

int State::DefaultPolicy() const
{
    if (nextChoice == W) {
        return -1;
    }
    State state = State(*this);
    while (true) {
        pii p = state.RandomPut();
        if (p.first == -1) {
            return 0; // Tie
        }
        state.board(p.first, p.second) = state.player + 1;
        state.top[p.second] = (p.first - 1 == BanX && p.second == BanY) ? p.first - 1 : p.first;
        if ((state.player && machineWin(p.first, p.second, H, W, state.board)) ||
                (!state.player && userWin(p.first, p.second, H, W, state.board))) {
            return state.player == player ? 1 : -1;
        }
        state.player = !state.player;
    }
}

void State::Print()
{
#ifndef ONLINE_JUDGE
    using namespace std;
    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; j++) {
            if (board(i, j) == 0) {
                if (i == BanX && j == BanY) {
                    cerr << "O ";
                } else {
                    cerr << ". ";
                }
            } else if (board(i, j) == OPPONENT + 1) {
                cerr << "P ";
            } else if (board(i, j) == FELLOW + 1) {
                cerr << "F ";
            }
        }
        cerr << endl;
    }
#endif
    return;
}

// ---------- BEGIN OF UCT ----------

TreeNode::TreeNode(State *state, TreeNode *parent)
{
    this->state = state;
    this->parent = parent;
    visit = 0;
    win = 0;
    firstChild = nullptr;
    nextSibling = nullptr;
}

TreeNode::~TreeNode()
{
    delete state;
    for (TreeNode *p = firstChild, *q; p != nullptr; p = q) {
        q = p->nextSibling;
        delete p;
    }
}

void TreeNode::Visit(int delta)
{
    ++visit;
    win += delta;
}

float TreeNode::UCB1(TreeNode *child)
{
    return -(float)child->win / child->visit + C * sqrt(2 * logf(this->visit) / child->visit);
}

TreeNode *TreeNode::Expand()
{
    State *newState = state->NextChoiceState();
    if (newState == nullptr) {
        return nullptr;
    }
    TreeNode *ret = new TreeNode(newState, this);
    ret->nextSibling = firstChild;
    firstChild = ret;
    return ret;
}

TreeNode *TreeNode::BestChild()
{
    TreeNode *ret = nullptr;
    float bestUCB1 = -1e9;
    for (TreeNode *p = firstChild; p != nullptr; p = p->nextSibling) {
        float curUCB1 = UCB1(p);
        if (curUCB1 > bestUCB1) {
            bestUCB1 = curUCB1;
            ret = p;
        }
    }
    assert(ret != nullptr);
    return ret;
}

UCT::UCT(bool player)
{
    root = new TreeNode(new State(player), nullptr);
}

UCT::~UCT()
{
    delete root;
}

TreeNode *UCT::TreePolicy()
{
    TreeNode *cur = root;
    while ((cur->state->nextChoice == State::W) && (cur->firstChild != nullptr)) {
        cur = cur->BestChild();
    }
    if (cur->state->nextChoice != State::W) {
        cur = cur->Expand();
    }
    return cur;
}

pii UCT::GetBestMove()
{
    int counter = 0;
    while((++counter % 100 != 0) || float(clock() - startTime) / CLOCKS_PER_SEC < TIME_LIMIT) {
        TreeNode *node = TreePolicy();
        int delta = node->state->DefaultPolicy();
        // Backup
        while (node != nullptr) {
            node->Visit(delta);
            delta = -delta;
            node = node->parent;
        }
    }

    // TODO: different strategy
    TreeNode *bestChild = nullptr;
    int maxTime = INT_MIN;
    for (TreeNode *p = root->firstChild; p != nullptr; p = p->nextSibling) {
        if(p->visit > maxTime) {
            maxTime = p->visit;
            bestChild = p;
        }
    }
    assert(bestChild != nullptr);
    return bestChild->state->lastPut;
}

void UCT::MoveRoot(pii put)
{
    TreeNode *node = nullptr;
    while (true) {
        node = root->Expand();
        if (node == nullptr) {
            break;
        }
    }
    assert(root->firstChild != nullptr);
    for (TreeNode *p = root->firstChild, *q; p != nullptr; p = q) {
        q = p->nextSibling;
        if (p->state->lastPut == put) {
            node = p;
            p->nextSibling = nullptr;
        } else {
            delete p;
        }
    }
    assert(node != nullptr);
    node->parent = nullptr;
    root = node;
}
