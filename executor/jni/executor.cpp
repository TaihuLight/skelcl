#include <string>
#include <vector>

#include "opencl_executor_Executor.h"
#include "handle.h"
#include "../executor.h"

jdouble
  Java_opencl_executor_Executor_execute(JNIEnv* env, jclass,
                                        jstring jKernelSource,
                                        jstring jKernelName,
                                        jint localSize1, jint localSize2, jint localSize3,
                                        jint globalSize1, jint globalSize2, jint globalSize3,
                                        jobjectArray jArgs)
{
  auto kernelSourcePtr = env->GetStringUTFChars(jKernelSource, nullptr);
  std::string kernelSource{kernelSourcePtr};
  auto kernelNamePtr = env->GetStringUTFChars(jKernelName, nullptr);
  std::string kernelName{kernelNamePtr};

  std::vector<KernelArg*> args(env->GetArrayLength(jArgs));
  int i = 0;
  for (auto& p : args) {
    auto obj = env->GetObjectArrayElement(jArgs, i);
    p = getHandle<KernelArg>(env, obj);
    ++i;
  }

  auto runtime = execute(kernelSource, kernelName,
   localSize1, localSize2, localSize3, globalSize1, globalSize2, globalSize3, args);

  env->ReleaseStringUTFChars(jKernelSource, kernelSourcePtr);
  env->ReleaseStringUTFChars(jKernelName, kernelNamePtr);
  return runtime;
}

void Java_opencl_executor_Executor_init__II(JNIEnv *, jclass,
                                            jint platformId,
                                            jint deviceId)
{
  initSkelCL(platformId, deviceId);
}

void Java_opencl_executor_Executor_init__(JNIEnv *, jclass)
{
  initSkelCL("ANY");
}

void Java_opencl_executor_Executor_shutdown(JNIEnv *, jclass)
{
  shutdownSkelCL();
}

