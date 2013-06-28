#include <CL/cl.h>
#include <CL/cl.hpp>
#include <iostream>
#include <boost/program_options.hpp>

#ifndef __CL_ENABLE_EXCEPTIONS
#define __CL_ENABLE_EXCEPTIONS
#endif

std::set<std::string> blacklist;

void rot13 (char *buf)
{
    int index=0;
    char c=buf[index];
    while (c!=0) {
        if (c<'A' || c>'z' || (c>'Z' && c<'a')) {
            buf[index] = buf[index];
        } else {
            if (c>'m' || (c>'M' && c<'a')) {
	        buf[index] = buf[index]-13;
	    } else {
	        buf[index] = buf[index]+13;
	    }
        }
	c=buf[++index];
    }
}

void evaluatePlatform(cl::Platform platform)
{
    std::string platformName;
    platform.getInfo(CL_PLATFORM_NAME, &platformName);

    if (blacklist.count(platformName)) {
        return;
    }

    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

    for (int j = 0; j < devices.size(); ++j) {
        cl_device_type deviceType;
        cl_uint deviceMaxComputeUnits;
        std::string deviceName;
        cl_ulong deviceGlobalMemSize;

        cl::Device& device = devices[j];
        device.getInfo(CL_DEVICE_TYPE,              &deviceType);
        device.getInfo(CL_DEVICE_MAX_COMPUTE_UNITS, &deviceMaxComputeUnits);
        device.getInfo(CL_DEVICE_NAME,              &deviceName);
        device.getInfo(CL_DEVICE_GLOBAL_MEM_SIZE,   &deviceGlobalMemSize);

        if (blacklist.count(deviceName)) {
            continue;
        }

        std::string typeString = "";
        if (deviceType & CL_DEVICE_TYPE_CPU) {
            typeString += "CPU ";
        }
        if (deviceType & CL_DEVICE_TYPE_GPU) {
            typeString += "GPU ";
        }
        if (deviceType & CL_DEVICE_TYPE_ACCELERATOR) {
            typeString += "ACCELERATOR ";
        }

        std::cout << "    - device[" << j << "]\n"
                  << "      - name:  " << deviceName << "\n"
                  << "      - type:  " << typeString << "\n"
                  << "      - units: " << deviceMaxComputeUnits << "\n"
                  << "      - mem:   " << deviceGlobalMemSize << "\n";
    }
}

void listPlatforms()
{
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    std::cout << "got " << platforms.size() << " platforms\n\n";

    for (int i = 0; i < platforms.size(); ++i) {
        std::string platformName, platformProfile, platformVersion, platformVendor, platformExtensions;
        cl::Platform& platform = platforms[i];

        platform.getInfo(CL_PLATFORM_NAME,    &platformName);
        platform.getInfo(CL_PLATFORM_PROFILE, &platformProfile);
        platform.getInfo(CL_PLATFORM_VERSION, &platformVersion);
        platform.getInfo(CL_PLATFORM_VENDOR,  &platformVendor);

        std::vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

        std::cout << "- platform[" << i << "]\n"
                  << "  - name:    " << platformName    << "\n"
                  << "  - profile: " << platformProfile << "\n"
                  << "  - version: " << platformVersion << "\n"
                  << "  - vendor:  " << platformVendor  << "\n"
                  << "  - devices: " << devices.size()  << "\n";

        evaluatePlatform(platform);
    }
}

namespace po = boost::program_options;

int main(int argc, char **argv)
{
    po::options_description description("Commandline options");
    description.add_options()
        ("help",
         "this help message")
        ("blacklist",
         po::value<std::vector<std::string> >()->composing(),
         "don't probe platform/device with given name");

    po::variables_map varMap;
    po::store(po::parse_command_line(argc, argv, description), varMap);
    po::notify(varMap);

    if (varMap.count("help")) {
        std::cout << description << "\n";
        return 0;
    }

    if (varMap.count("blacklist")) {
        std::vector<std::string> entries = varMap["blacklist"].as<std::vector<std::string> >();
        for (int i = 0; i < entries.size(); ++i) {
            std::cout << "blacklist: " << entries[i] << "\n";
            blacklist.insert(entries[i]);
        }
    }

    listPlatforms();

    return 0;
}
