#pragma once

#include <utils/clock.h>
#include <string>

#ifdef OPEN_TRACE
#define PROF_BEGIN startGlobalTimer();
#define PROF_START(name) const TimePoint name = startTimer();
#define PROF_END(name) logEndTimer(#name, name);
#define PROF_SUMMARY printTimerTotals();
#else
#define PROF_BEGIN
#define PROF_START(name)
#define PROF_END(name)
#define PROF_SUMMARY
#endif

TimePoint startTimer();

long getTimeDiffNanos(const TimePoint &begin);

long getTimeDiffMicros(const TimePoint &begin);

double getTimeDiffMillis(const TimePoint &begin);

void logEndTimer(const std::string &label,
                 const TimePoint &begin);

void startGlobalTimer();

void printTimerTotals();

uint64_t timespecToNanos(struct timespec *nativeTimespec);

void nanosToTimespec(uint64_t nanos, struct timespec *nativeTimespec);
