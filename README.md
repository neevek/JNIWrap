JNIWrap
=======

JNIWrap is a small library that eases the process of calling Java from C++ through JNI.

Example:
--------

```c++
// define HashMap class in C++
class HashMap : public jniwrap::JavaObject {
  public:
    HashMap() : jniwrap::JavaObject() {
      Init("java/util/HashMap", "()V");
    }

    DEFINE_METHOD(put, "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;", jobject, Object)
    DEFINE_METHOD(get, "(Ljava/lang/Object;)Ljava/lang/Object;", jobject, Object)
    ...
};

// use HashMap

auto hashMap = HashMap{};

auto key = jniwrap::JavaObject{};
assert(key.Init("java/lang/Integer", "(I)V", 123));

hashMap.put(*key, jniwrap::JavaObject::NewString("Hello World from C++"));
jstring value = reinterpret_cast<jstring>(hashMap.get(*key));

// return value to Java

```

Under MIT license
-----------------
```
Copyright (c) 2018 neevek <i@neevek.net>
```
