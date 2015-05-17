///
/// OmniTuneClient.cpp
///
/// \author Chris Cummins <chrisc.101@gmail.com>
#include "SkelCL/detail/OmniTuneClient.h"

#include <cstdint>
#include <utility>

#include <pvsutil/Logger.h>

namespace omnitune {

#define OMNITUNE_BUS_NAME                      "org.omnitune"
#define OMNITUNE_SKELCL_INTERFACE              "org.omnitune.skelcl"
#define OMNITUNE_SKELCL_INTERFACE_OBJECT_PATH  "/SkelCLProxy"

#define STENCIL_WORKGROUP_SIZE_C 32
#define STENCIL_WORKGROUP_SIZE_R 4

// Global variable for omnitune proxy.
Glib::RefPtr<Gio::DBus::Proxy> proxy;

void init() {
  // Do nothing if we're already initialised.
  if (proxy)
    return;

  Glib::init();
  Gio::init();

  Glib::RefPtr<Gio::DBus::Connection> bus
      = Gio::DBus::Connection::get_sync(Gio::DBus::BUS_TYPE_SESSION);
  proxy = Gio::DBus::Proxy::create_sync(bus,
                                        OMNITUNE_BUS_NAME,
                                        OMNITUNE_SKELCL_INTERFACE_OBJECT_PATH,
                                        OMNITUNE_SKELCL_INTERFACE);
}

namespace stencil {

// Request a workgroup size from the proxy server and set the response
// to local.
void requestWgSize(const std::string &deviceName,
                   const std::string &source,
                   cl_uint *const local) {
  init();

  // Create input arguments.
  std::vector<Glib::VariantBase> _args;
  _args.push_back(Glib::Variant<std::string>::create(deviceName));
  _args.push_back(Glib::Variant<std::string>::create(source));
  Glib::VariantContainerBase args
      = Glib::VariantContainerBase::create_tuple(_args);

  // Synchronously call "RequestWorkgroupSize()".
  Glib::VariantContainerBase response
      = proxy->call_sync("RequestWorkgroupSize", args);
  Glib::VariantIter iterator(response.get_child(0));
  Glib::Variant<int16_t> var;

  // Set response values.
  iterator.next_value(var);
  local[0] = var.get();
  iterator.next_value(var);
  local[1] = var.get();
}

void getLocalSize(const cl::Kernel &kernel,
                  const cl::Device &device,
                  const std::string &source,
                  cl_uint *const local) {
  try {
    requestWgSize(device.getInfo<CL_DEVICE_NAME>(), source, local);
  } catch (Glib::Error e) {
    // Fall-back state, set default values.
    LOG_ERROR(e.what());
    local[0] = STENCIL_WORKGROUP_SIZE_C;
    local[1] = STENCIL_WORKGROUP_SIZE_R;
  }

  // Ensure that workgroup size is within kernel-enforced maximum.
  const size_t max = kernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(device);
  while (local[0] * local[1] > max) {
      OMNILOG("Target workgroup size of (", local[0], ", ", local[1],
              ") exceeds kernel maximum of ", max, ". Shrinking.");
      local[0] /= 2;
      local[1] /= 2;
  }

  OMNILOG("Workgroup size (", local[0], ", ", local[1], ")");
}

}  // namespace stencil

}  // namespace omnitune
