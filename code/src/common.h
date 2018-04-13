#ifndef CODE_COMMON_H
#define CODE_COMMON_H

#define PROGRAM_VERSION    "1.0.0"
#define VERBOSE         0

#define log_verbose(fmt, ...) \
            do { if (VERBOSE) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

#endif //CODE_COMMON_H
