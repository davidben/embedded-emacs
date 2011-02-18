// Copyright (c) 2010 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.
#ifndef INCLUDED_SCOPED_NPOBJECT_H_
#define INCLUDED_SCOPED_NPOBJECT_H_

#include "npapi-headers/npruntime.h"
#include "util.h"

namespace npapi {

template <class T>
class scoped_npobject {
  public:
    explicit scoped_npobject(T* obj = NULL) : obj_(obj) {
        reset(obj);
    }
    ~scoped_npobject() {
        reset();
    }
    T* get() {
        return obj_;
    }
    void reset(T* obj = NULL) {
        if (obj)
            NPN_RetainObject(obj);
        if (obj_)
            NPN_ReleaseObject(obj_);
        obj_ = obj;
    }
    T* release() {
        T* obj = obj_;
        obj_ = NULL;
        return obj;
    }
  private:
    T* obj_;
    DISALLOW_COPY_AND_ASSIGN(scoped_npobject);
};

}  // namespace npapi

#endif  // INCLUDED_SCOPED_NPOBJECT_H_
