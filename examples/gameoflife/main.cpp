/*
 * glaettenMapOverlap.cpp
 *
 *  Created on: 02.03.2011
 *      Author: Stefan Breuer
 */
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>

#include <pvsutil/CLArgParser.h>
#include <pvsutil/Logger.h>

#include <SkelCL/SkelCL.h>
#include <SkelCL/IndexMatrix.h>
#include <SkelCL/MapOverlap.h>
#include <SkelCL/Stencil.h>
#include <SkelCL/detail/Padding.h>

using namespace skelcl;

long long get_time() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec * 1000000) + tv.tv_usec;
}

int main(int argc, char** argv) {
    long long time0;
    long long time1;
    long long time5;

    using namespace pvsutil::cmdline;
    pvsutil::CLArgParser cmd(Description("Computation of the Gaussian blur."));

    auto deviceCount = Arg<int>(Flags(Long("device_count")),
            Description("Number of devices used by SkelCL."), Default(1));


    auto size = Arg<int>(Flags(Long("problemsize")),
            Description("Number of devices used by SkelCL."), Default(5));

    auto deviceType = Arg<device_type>(Flags(Long("device_type")),
            Description("Device type: ANY, CPU, "
                    "GPU, ACCELERATOR"), Default(device_type::GPU));

    auto iterationen = Arg<int>(Flags(Long("iterationen")),
                                   Description("The number of iterations"), Default(1));

    cmd.add(&deviceCount, &deviceType, &iterationen, &size);
    cmd.parse(argc, argv);

    int numcols = size;
    int numrows = size;
    timeval t1;
    gettimeofday(&t1, NULL);
    srand(t1.tv_usec * t1.tv_sec);
    std::vector<int> img(1,1);

    for(unsigned int i=0 ; i<numcols*numrows ; i++) {
        img.push_back(random() > (INT_MAX / 2));
    }

    time0 = get_time();

    skelcl::init(skelcl::nDevices(deviceCount).deviceType(deviceType));

    time1 = get_time();

    Matrix<int> inputImage(img, numcols);

    skelcl::MapOverlap<int(int)> s(std::ifstream { "./gameoflife.cl" }, 1,
                        detail::Padding::NEUTRAL, 0, "func");

    for (int iter=0; iter<iterationen; iter++){
        inputImage = s(inputImage);
        inputImage.copyDataToHost();
        inputImage.resize(inputImage.size());
    }

    //Get time
    time5=get_time();

    printf("Init time : %.12f\n", (float) (time1-time0) / 1000000);
    printf("Total time : %.12f\n", (float) (time5-time0) / 1000000);
     printf("Total without init time : %.12f\n", (float) (time5-time1) / 1000000);


    skelcl::terminate();

}
