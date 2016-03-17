#ifndef __mytypes_h_included
#define __mytypes_h_included 1

/* some types and macros I miss in C89 */

typedef enum {false, true } bool;
/*标准 C 中没有 bool 类型 */
#define Min(x,y) ((x)<=(y)?(x):(y))
#define Max(x,y) ((x)>=(y)?(x):(y))
 
#endif /* __mytypes_h_included */
