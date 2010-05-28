#ifndef H_MMP_H_UTILS_H
#define H_MMP_H_UTILS_H

#ifdef __cplusplus
#   define MMP_CEXTERN_BEGIN    extern "C" {
#   define MMP_CEXTERN_END      }
#else
#   define MMP_CEXTERN_BEGIN
#   define MMP_CEXTERN_END
#endif

#ifndef UNREFERENCED_PARAMETER
#   define UNREFERENCED_PARAMETER(_P) ((void)(_P))
#endif

#endif /* H_MMP_H_UTILS_H */
