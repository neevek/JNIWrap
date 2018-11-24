/*******************************************************************************
**          File: jni_env_wrap.h
**        Author: neevek <i@neevek.net>.
** Creation Time: 2018-11-17 Sat 11:57 AM
**   Description: a helper class to get JNIEnv object from any thread
*******************************************************************************/
#ifndef JNI_ENV_WRAP_H_
#define JNI_ENV_WRAP_H_
#include <jni.h>
#include <thread>
#include <cassert>
#include "log/log.hpp"

namespace jniwrap {

  class JNIEnvWrap final {
    public:
      static void SetJavaVM(JavaVM *jvm);
      static JavaVM *GetJavaVM();
      static JNIEnv *GetThreadLocalJNIEnv();
    private:
      static JavaVM *jvm_;
  };

} /* end of namspace: jniwrap */

#endif /* end of include guard: JNI_ENV_WRAP_H_ */
