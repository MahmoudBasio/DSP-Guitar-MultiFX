#ifndef CORE_DIAGNOSTICS_H
#define CORE_DIAGNOSTICS_H

#ifndef MULTIFX_DIAGNOSTICS
#define MULTIFX_DIAGNOSTICS 0
#endif

namespace Diagnostics {
#if MULTIFX_DIAGNOSTICS
void init();
void poll();
#else
inline void init() {}
inline void poll() {}
#endif
}
#endif
