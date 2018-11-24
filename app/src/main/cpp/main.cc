#include <jni.h>
#include <string>
#include "jniwrap/jni_env_wrap.h"
#include "jniwrap/java_object.hpp"

class HashMap : public jniwrap::JavaObject {
public:
  HashMap() : jniwrap::JavaObject() {
    Init("java/util/HashMap", "()V");
  }

  DEFINE_METHOD(put, "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;", jobject, Object)
  DEFINE_METHOD(get, "(Ljava/lang/Object;)Ljava/lang/Object;", jobject, Object)
};

#ifdef __cplusplus
extern "C" {
#endif

  jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
      return -1;
    }

    jniwrap::JNIEnvWrap::SetJavaVM(vm);
    return JNI_VERSION_1_6;
  }

  jstring Java_net_neevek_jniwrap_MainActivity_getString(
    JNIEnv* env,
    jobject /* this */) {

    auto jstr = jniwrap::JavaObject{};
    assert(jstr.Init("java/lang/StringBuilder", "()V"));
    jstr.CallObjectMethod("append", "(I)Ljava/lang/StringBuilder;", 12345);
    jstr.CallObjectMethod("append", "(Ljava/lang/String;)Ljava/lang/StringBuilder;",
                          jniwrap::JavaObject::NewString("[测试]"));

    auto hashMap = HashMap{};
    auto key = jniwrap::JavaObject{};
    assert(key.Init("java/lang/Integer", "(I)V", 123));

    hashMap.put(*key, jniwrap::JavaObject::NewString("Hello World from C++"));
    auto s = hashMap.get(*key);

    return jniwrap::JavaObject(s).ToJString();
  }

#ifdef __cplusplus
}
#endif
