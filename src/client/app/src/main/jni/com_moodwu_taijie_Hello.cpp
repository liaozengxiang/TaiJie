//
// Created by Administrator on 2018/1/12.
//

#include "com_moodwu_taijie_Hello.h"

/*
 * Class:     com_moodwu_taijie_Hello
 * Method:    helloWorld
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_moodwu_taijie_Hello_helloWorld(JNIEnv *env, jclass) {
    return env->NewStringUTF("HelloWorld from CPP");
}

/*
 * Class:     com_moodwu_taijie_Hello
 * Method:    add
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_moodwu_taijie_Hello_add(JNIEnv *, jclass, jint a, jint b) {
    return a + b;
}
