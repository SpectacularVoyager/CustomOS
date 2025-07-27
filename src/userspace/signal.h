#include "include/signal.h"

int sigprocmask(int how, const sigset_t * set,
		sigset_t *oldset);
