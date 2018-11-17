/*******************************************************************************
**          File: jni_env_wrapper.hpp
**        Author: neevek <i@neevek.net>.
** Creation Time: 2018-11-17 Sat 11:57 AM
**   Description: a helper class to get JNIEnv object from any thread
*******************************************************************************/
#ifndef JNI_ENV_WRAPPER_H_
#define JNI_ENV_WRAPPER_H_
#include <jni.h>
#include <thread>
#include <cassert>
#include "../log/log.hpp"

namespace jniwrap {

  class JNIEnvWrap final {
    public:
      static void SetJavaVM(JavaVM *jvm) {
        jvm_ = jvm;
      }

      static JavaVM *GetJavaVM() {
        return jvm_;
      }

      static JNIEnv *GetThreadLocalJNIEnv() {
        assert(jvm_);

        thread_local struct JNIEnvWrapper {
          JNIEnv *jni_env{nullptr};

          JNIEnvWrapper() {
            LOG_D("AttachCurrentThreadAsDaemon: %zu",
                  std::hash<std::thread::id>()(std::this_thread::get_id()));
            JNIEnvWrap::GetJavaVM()->AttachCurrentThreadAsDaemon(&jni_env, nullptr);
          }

          ~JNIEnvWrapper() {
            LOG_D("DetachCurrentThread: %zu",
                  std::hash<std::thread::id>()(std::this_thread::get_id()));
            JNIEnvWrap::GetJavaVM()->DetachCurrentThread();
          }
        } wrapper;

        return wrapper.jni_env;
      }

    private:
      static JavaVM *jvm_;
  };

  JavaVM *JNIEnvWrap::jvm_{nullptr};

} /* end of namspace: jniwrap */

#endif /* end of include guard: JNI_ENV_WRAPPER_H_ */
