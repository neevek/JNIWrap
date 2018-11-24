/*******************************************************************************
**          File: java_object.hpp
**        Author: neevek <i@neevek.net>.
** Creation Time: 2018-11-17 Sat 11:44 AM
**   Description: wrapper of an java object through JNI
*******************************************************************************/
#ifndef JAVA_OBJECT_H_
#define JAVA_OBJECT_H_
#include <jni.h>
#include <string>
#include <map>
#include "jniwrap/jni_env_wrap.h"
#include "log/log.hpp"

namespace jniwrap {

  class JavaObject {
    public:
      JavaObject() : jni_env_(JNIEnvWrap::GetThreadLocalJNIEnv()) { }
      JavaObject(jobject jobj) :
        jni_env_(JNIEnvWrap::GetThreadLocalJNIEnv()),
        jobj_weakref_(jobj){

        assert(jobj);
        jclz_ = (jclass)jni_env_->NewGlobalRef(jni_env_->GetObjectClass(jobj));
      }

      virtual ~JavaObject() {
        if (jobj_) {
          jni_env_->DeleteGlobalRef(jobj_);
        }
        if (jclz_) {
          jni_env_->DeleteGlobalRef(jclz_);
        }
      }

      template <typename ...Args>
      bool Init(
        const std::string &class_name,
        const std::string &ctor_signature,
        Args ...args) {

        if (jobj_ || jobj_weakref_) {
          return true;
        }

        auto jclz = jni_env_->FindClass(class_name.c_str());
        if (!jclz) {
          LOG_E("failed to create find class: %s", class_name.c_str());
          return false;
        }
        jclz_ = (jclass)jni_env_->NewGlobalRef(jclz);

        auto method = FindMethod("<init>", ctor_signature.c_str());
        if (!method) {
          return false;
        }

        auto jobj_local = jni_env_->NewObject(
          jclz_, method, std::forward<Args>(args)...);
        if (!jobj_local) {
          LOG_E("failed to init object: %s", class_name.c_str());
          return false;
        }

        jobj_ = jni_env_->NewGlobalRef(jobj_local);
        if (!jobj_) {
          LOG_E("failed to create global ref of %s", class_name.c_str());
          jni_env_->DeleteLocalRef(jobj_local);
          return false;
        }

        jni_env_->DeleteLocalRef(jobj_local);
        return true;
      }

      jobject operator *() const {
        return jobj_ ? jobj_ : jobj_weakref_;
      }

      jstring ToJString() {
        return reinterpret_cast<jstring>(
          CallObjectMethod("toString", "()Ljava/lang/String;"));
      }

      #define CALL_METHOD(return_type, method_type) \
        template <typename ...Args> \
        return_type Call##method_type##Method( \
            const std::string &method_name, \
            const std::string &signature, \
            Args ...args) { \
          auto method = FindMethod(method_name, signature); \
          if (method) { \
            return jni_env_->Call##method_type##Method( \
                jobj_ ? jobj_ : jobj_weakref_, method, std::forward<Args>(args)...); \
          } \
          return return_type(); \
        }

      #define GET_METHOD(return_type, method_type) \
        template <typename ...Args> \
        std::function<return_type(Args...)> Get##method_type##Method( \
          const std::string &method_name, \
          const std::string &signature) { \
          auto method = FindMethod(method_name, signature); \
          if (method) { \
            return [this, method](Args ...args) { \
              return jni_env_->Call##method_type##Method( \
                  jobj_ ? jobj_ : jobj_weakref_, method, std::forward<Args>(args)...); \
            }; \
          } \
          return nullptr; \
        }

      CALL_METHOD(void, Void)
      CALL_METHOD(jint, Int)
      CALL_METHOD(jshort, Short)
      CALL_METHOD(jlong, Long)
      CALL_METHOD(jdouble, Double)
      CALL_METHOD(jbyte, Byte)
      CALL_METHOD(jfloat, Float)
      CALL_METHOD(jchar, Char)
      CALL_METHOD(jboolean, Boolean)
      CALL_METHOD(jobject, Object)

      GET_METHOD(void, Void)
      GET_METHOD(jint, Int)
      GET_METHOD(jshort, Short)
      GET_METHOD(jlong, Long)
      GET_METHOD(jdouble, Double)
      GET_METHOD(jbyte, Byte)
      GET_METHOD(jfloat, Float)
      GET_METHOD(jchar, Char)
      GET_METHOD(jboolean, Boolean)
      GET_METHOD(jobject, Object)

      static jstring NewString(const char *cstr) {
        return JNIEnvWrap::GetThreadLocalJNIEnv()->NewStringUTF(cstr);
      }

    protected:
      #define DEFINE_METHOD(method_name, signature, return_type, method_type) \
        template <typename ...Args> \
        return_type method_name(Args ...args) { \
          static auto method = FindMethod(#method_name, signature); \
          assert(method); \
          return jniwrap::JNIEnvWrap::GetThreadLocalJNIEnv()->Call##method_type##Method( \
              this->operator*(), method, std::forward<Args>(args)...); \
        }

      jmethodID FindMethod(
          const std::string &method_name,
          const std::string &signature) {
        auto key = method_name + signature;
        auto method_it = method_map_.find(key);
        jmethodID method;
        if (method_it != method_map_.end()) {
          method = method_it->second;

        } else {
          method = jni_env_->GetMethodID(
              jclz_, method_name.c_str(), signature.c_str());
          if (!method) {
            LOG_E("failed to find method: %s - %s",
                  method_name.c_str(), signature.c_str());
            return nullptr;
          }

          method_map_[key] = method;
        }

        return method;
      }

    private:
      JNIEnv *jni_env_{nullptr};
      jobject jobj_weakref_{nullptr};
      jobject jobj_{nullptr};
      jclass jclz_{nullptr};
      std::map<std::string, jmethodID> method_map_;
  };

} /* end of namspace: jniwrap */

#endif /* end of include guard: JAVA_OBJECT_H_ */
