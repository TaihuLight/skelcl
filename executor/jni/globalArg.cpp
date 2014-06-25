#include "opencl_executor_GlobalArg.h"
#include "handle.h"
#include "../executor.h"

jobject Java_opencl_executor_GlobalArg_createInput___3F(JNIEnv* env, jclass cls,
                                                        jfloatArray data)
{
  auto arrayPtr = env->GetFloatArrayElements(data, nullptr);
  auto ptr = GlobalArg::create(arrayPtr,
                               env->GetArrayLength(data) * sizeof(float));
  env->ReleaseFloatArrayElements(data, arrayPtr, JNI_ABORT);

  auto methodID = env->GetMethodID(cls, "<init>", "(J)V"); 
  auto obj = env->NewObject(cls, methodID, ptr);
  return obj;
}

jobject Java_opencl_executor_GlobalArg_createInput___3I(JNIEnv* env, jclass cls,
                                                        jintArray data)
{
  auto arrayPtr = env->GetIntArrayElements(data, nullptr);
  auto ptr = GlobalArg::create(arrayPtr,
                               env->GetArrayLength(data) * sizeof(int));
  env->ReleaseIntArrayElements(data, arrayPtr, JNI_ABORT);

  auto methodID = env->GetMethodID(cls, "<init>", "(J)V");
  auto obj = env->NewObject(cls, methodID, ptr);
  return obj;
}

jobject Java_opencl_executor_GlobalArg_createOutput(JNIEnv* env, jclass cls,
                                                    jint size)
{
  auto ptr = GlobalArg::create(size, true);
  auto methodID = env->GetMethodID(cls, "<init>", "(J)V"); 
  auto obj = env->NewObject(cls, methodID, ptr);
  return obj;
}

jfloat Java_opencl_executor_GlobalArg_at(JNIEnv* env, jobject obj, jint index)
{
  auto ptr = getHandle<GlobalArg>(env, obj);
  auto vec = ptr->data();
  vec.copyDataToHost();
  auto dataPtr = reinterpret_cast<float*>(vec.hostBuffer().data());
  return dataPtr[index];
}

jfloatArray Java_opencl_executor_GlobalArg_asFloatArray(JNIEnv* env,
                                                        jobject obj)
{
  auto ptr = getHandle<GlobalArg>(env, obj);
  auto vec = ptr->data();

  auto res = env->NewFloatArray(vec.size() / sizeof(float));
  if (res == nullptr) return nullptr;

  env->SetFloatArrayRegion(res, 0, vec.size() / sizeof(float),
                           reinterpret_cast<float*>(vec.hostBuffer().data()));
  return res;
}

jintArray Java_opencl_executor_GlobalArg_asIntArray(JNIEnv* env, jobject obj)
{
  auto ptr = getHandle<GlobalArg>(env, obj);
  auto vec = ptr->data();

  auto res = env->NewIntArray(vec.size() / sizeof(int));
  if (res == nullptr) return nullptr;

  env->SetIntArrayRegion(res, 0, vec.size() / sizeof(int),
                         reinterpret_cast<int*>(vec.hostBuffer().data()));
  return res;
}

