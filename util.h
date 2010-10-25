#ifndef INCLUDED_UTIL_H_
#define INCLUDED_UTIL_H_

#define DISALLOW_COPY_AND_ASSIGN(Class) \
    Class(const Class&); \
    void operator=(const Class&);

#define G_FORWARD_DECLARE(GTypeName) typedef struct _ ## GTypeName GTypeName

#endif  // INCLUDED_UTIL_H_
