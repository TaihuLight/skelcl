#include <string>
#include <vector>
#include <limits>
#include <iostream>
#include <algorithm>
#include <thread>
#include <cassert>

#include "opencl_executor_Executor.h"
#include "handle.h"
#include "../executor.h"

enum class Mode {
  Execute,
  Benchmark,
  Evaluate
};

jdouble
  executeOrBencmark(JNIEnv* env, jclass,
                    jstring jKernelSource,
                    jstring jKernelName,
                    jstring jBuildOptions,
                    jint localSize1, jint localSize2, jint localSize3,
                    jint globalSize1, jint globalSize2, jint globalSize3,
                    jobjectArray jArgs,
                    Mode mode,
                    jint iterations,
                    jdouble timeout)
{
  double runtime = 0;

  auto kernelSourcePtr = env->GetStringUTFChars(jKernelSource, nullptr);
  std::string kernelSource{kernelSourcePtr};
  auto kernelNamePtr = env->GetStringUTFChars(jKernelName, nullptr);
  std::string kernelName{kernelNamePtr};
  auto buildOptionsPtr = env->GetStringUTFChars(jBuildOptions, nullptr);
  std::string buildOptions{buildOptionsPtr};

  try {
        std::vector<KernelArg*> args(env->GetArrayLength(jArgs));
    int i = 0;
    for (auto& p : args) {
      auto obj = env->GetObjectArrayElement(jArgs, i);
      p = getHandle<KernelArg>(env, obj);
      ++i;
    }


    switch(mode) {
      case Mode::Execute:
        runtime = execute(kernelSource, kernelName, buildOptions,
          localSize1, localSize2, localSize3, globalSize1, globalSize2, globalSize3, args);
        break;
      case Mode::Benchmark:
        runtime = benchmark(kernelSource, kernelName, buildOptions,
          localSize1, localSize2, localSize3, globalSize1, globalSize2, globalSize3,
          args, iterations, timeout);
        break;
      case Mode::Evaluate:
        runtime = evaluate(kernelSource, kernelName, buildOptions,
          localSize1, localSize2, localSize3, globalSize1, globalSize2, globalSize3,
          args, iterations, timeout);
        break;
      default:
        return 0;
    }
  } catch(...) {
    jclass jClass = env->FindClass("opencl/executor/Executor$ExecutorFailureException");
    if(!jClass) std::cerr << "[JNI ERROR] Cannot find the exception class" << std::endl;
    env->ThrowNew(jClass, "Executor failure");
  }  
 
  env->ReleaseStringUTFChars(jKernelSource, kernelSourcePtr);
  env->ReleaseStringUTFChars(jKernelName, kernelNamePtr);

  return runtime;
}

void 
  Java_opencl_executor_Executor_nativeMatrixMultiply(JNIEnv *env, jclass,
                                                     jfloatArray a, 
                                                     jfloatArray b,
                                                     jfloatArray out,
                                                     jint N, jint, jint)
{
  float *aa = (float *)env->GetFloatArrayElements( a, NULL);
  float *bb = (float *)env->GetFloatArrayElements( b, NULL);
  float *cc = (float *)env->GetFloatArrayElements( out, NULL);

  /*for (int i=0; i<N; i++) {

    float kk[N];
    for (int j=0; j<N; j++) 
      kk[j] = 0;

    for (int k=0; k<N; k++)
      for (int j=0; j<N; j++)
        kk[j] += aa[i*N+k] * bb[k*N+j];

    for (int j=0; j<N; j++)
       cc[i*N+j] = kk[j];
  }*/
  std::vector < std::thread > threads;

  auto mmult = [&](int from, int to) {
    float kk[N];
    for (int i=from; i<to; i++) {
      for (int j=0; j<N; j++) 
        kk[j] = 0;

      for (int k=0; k<N; k++)
        for (int j=0; j<N; j++)
          kk[j] += aa[i*N+k] * bb[k*N+j];

      for (int j=0; j<N; j++)
         cc[i*N+j] = kk[j];
    }
  };

  int nthreads = std::thread::hardware_concurrency();
  int chunk = N / nthreads;
  for (auto tid = 0; tid < nthreads; tid++) {
    threads.push_back(std::thread([=]{mmult(tid*chunk, (tid+1)*chunk);}));
  }
  for (auto & t : threads) t.join();


  env->ReleaseFloatArrayElements(a, aa, 0);
  env->ReleaseFloatArrayElements(b, bb, 0);
  env->ReleaseFloatArrayElements(out, cc, 0); 
}

jdouble
  Java_opencl_executor_Executor_execute(JNIEnv* env, jclass jClass,
                                        jstring jKernelSource,
                                        jstring jKernelName,
                                        jstring jBuildOptions,
                                        jint localSize1, jint localSize2, jint localSize3,
                                        jint globalSize1, jint globalSize2, jint globalSize3,
                                        jobjectArray jArgs)
{
  return executeOrBencmark(env, jClass, jKernelSource, jKernelName, jBuildOptions,
        localSize1, localSize2, localSize3, globalSize1, globalSize2, globalSize3,
        jArgs, Mode::Execute, 0, 0.0);
}

jdouble
  Java_opencl_executor_Executor_benchmark(JNIEnv* env, jclass jClass,
                                        jstring jKernelSource,
                                        jstring jKernelName,
                                        jstring jBuildOptions,
                                        jint localSize1, jint localSize2, jint localSize3,
                                        jint globalSize1, jint globalSize2, jint globalSize3,
                                        jobjectArray jArgs,
                                        jint iterations,
                                        jdouble timeout)
{
  return executeOrBencmark(env, jClass, jKernelSource, jKernelName, jBuildOptions,
        localSize1, localSize2, localSize3, globalSize1, globalSize2, globalSize3,
        jArgs, Mode::Benchmark, iterations, timeout);
}

jdouble
  Java_opencl_executor_Executor_evaluate(JNIEnv* env, jclass jClass,
                                        jstring jKernelSource,
                                        jstring jKernelName,
                                        jstring jBuildOptions,
                                        jint localSize1, jint localSize2, jint localSize3,
                                        jint globalSize1, jint globalSize2, jint globalSize3,
                                        jobjectArray jArgs,
                                        jint iterations,
                                        jdouble timeout)
{
  return executeOrBencmark(env, jClass, jKernelSource, jKernelName, jBuildOptions,
        localSize1, localSize2, localSize3, globalSize1, globalSize2, globalSize3,
        jArgs, Mode::Evaluate, iterations, timeout);
}



void Java_opencl_executor_Executor_init(JNIEnv *, jclass,
                                        jint platformId,
                                        jint deviceId)
{
  initSkelCL(platformId, deviceId);
}

jstring Java_opencl_executor_Executor_getPlatformName(JNIEnv * env, jclass)
{
  auto name = getPlatformName();
  return env->NewStringUTF(name.c_str());
}

jlong Java_opencl_executor_Executor_getDeviceLocalMemSize(JNIEnv *, jclass)
{
  const auto value = getDeviceLocalMemSize();
  ASSERT_MESSAGE(value <= std::numeric_limits<long>::max(), "JNI cast overflow");
  return static_cast<long>(value);
}

jlong Java_opencl_executor_Executor_getDeviceGlobalMemSize(JNIEnv *, jclass)
{
  const auto value = getDeviceGlobalMemSize();
  ASSERT_MESSAGE(value <= std::numeric_limits<long>::max(), "JNI cast overflow");
  return static_cast<long>(value);
}

jlong Java_opencl_executor_Executor_getDeviceMaxMemAllocSize(JNIEnv *, jclass)
{
  const auto value = getDeviceMaxMemAllocSize();
  ASSERT_MESSAGE(value <= std::numeric_limits<long>::max(), "JNI cast overflow");
  return static_cast<long>(value);
}

jlong Java_opencl_executor_Executor_getDeviceMaxWorkGroupSize(JNIEnv *, jclass)
{
  const auto value = getDeviceMaxWorkGroupSize();
  ASSERT_MESSAGE(value <= std::numeric_limits<long>::max(), "JNI cast overflow");
  return static_cast<long>(value);
}

jstring Java_opencl_executor_Executor_getDeviceName(JNIEnv * env, jclass)
{
  auto name = getDeviceName();
  return env->NewStringUTF(name.c_str());
}

jstring Java_opencl_executor_Executor_getDeviceType(JNIEnv * env, jclass)
{
  auto type = getDeviceType();
  return env->NewStringUTF(type.c_str());
}

jboolean Java_opencl_executor_Executor_supportsDouble(JNIEnv *, jclass)
{
  auto supports = supportsDouble();
  return static_cast<bool>(supports);
}

void Java_opencl_executor_Executor_init__(JNIEnv *, jclass)
{
  initSkelCL("ANY");
}

void Java_opencl_executor_Executor_shutdown(JNIEnv *, jclass)
{
  shutdownSkelCL();
}

