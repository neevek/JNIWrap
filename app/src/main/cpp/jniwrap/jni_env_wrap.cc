/*******************************************************************************
**          File: jni_env_wrap.cc
**        Author: neevek <i@neevek.net>.
** Creation Time: 2018-11-19 Mon 06:54 PM
**   Description: see the header file
*******************************************************************************/
#include "jniwrap/jni_env_wrap.h"

namespace jniwrap {

  JavaVM *JNIEnvWrap::JNIEnvWrap::jvm_{nullptr};

  void JNIEnvWrap::SetJavaVM(JavaVM *jvm) {
    jvm_ = jvm;
  }

  JavaVM *JNIEnvWrap::GetJavaVM() {
    return jvm_;
  }

  JNIEnv *JNIEnvWrap::GetThreadLocalJNIEnv() {
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

} /* end of namespace: jniwrap */
