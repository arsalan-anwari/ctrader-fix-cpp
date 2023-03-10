#define EXPAND(...) __VA_ARGS__

#define REPEAT_LIST_1(X) X
#define REPEAT_LIST_2(X) X, X
#define REPEAT_LIST_3(X) X, X, X
#define REPEAT_LIST_4(X) X, X, X, X
#define REPEAT_LIST_5(X) X, X, X, X, X

#define __REPEAT_LIST(N, X) EXPAND(REPEAT_LIST_ ## N)(X)
#define _REPEAT_LIST(N, X) __REPEAT_LIST(N, X)
#define REPEAT_LIST(X, N) _REPEAT_LIST( EXPAND(N), X )