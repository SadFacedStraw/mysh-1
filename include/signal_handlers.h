#ifndef SIGNAL_H_
#define SIGNAL_H_

void catch_sigint(int signalNo);

void catch_sigtstp(int signalNo);

void catch_sigchld(int signalNo);

#endif // SIGNAL_H_
