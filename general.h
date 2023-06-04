#ifndef GENERAL_H
#define GENERAL_H

#ifdef ONLINE_JUDGE
#define NDEBUG
template<class T>void err(const T &x) {}
template<class T>void err_space(const T &x) {}
template<class T, class ...Args>void err(const T &x, const Args &...args) {}
#else
// *INDENT-OFF*
#include <iostream>
template<class T>void err(const T &x) {std::cerr << x << '\n';}
template<class T>void err_space(const T &x) {std::cerr << x << ' ';}
template<class T, class ...Args>void err(const T &x, const Args &...args)
{
    err_space(x);
    err(args...);
}
// *INDENT-ON*
#endif

#include <cassert>
#include <cstring>
#include <cmath>
#include <ctime>
#include <climits>

#define mp std::make_pair
typedef std::pair<int, int> pii;

#endif //GENERAL_H
