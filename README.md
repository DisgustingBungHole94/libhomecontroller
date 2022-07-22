# libhomecontroller

## Build Instructions

You must install [llhttp](https://github.com/nodejs/llhttp) and [RapidJSON](https://github.com/Tencent/rapidjson) to build libhomecontroller.

```sh
git clone https://github.com/DisgustingBungHole94/libhomecontroller.git
cd libhomecontroller
make
sudo make install
```

To uninstall libhomecontroller:

```sh
sudo make uninstall
```

## Usage

```C++
#include <homecontroller/device.h>

#include <iostream>
#include <vector>

class MyDevice : public hc::Device {
    public:
        MyDevice() {}
        ~MyDevice() {}

        /*
            Messages from the HomeController server are received as a vector of bytes.
            
            Overload the onMessage function and return a byte vector to respond to
            the server!
        */
        virtual std::vector<uint8_t> onMessage(const std::vector<uint8_t>& msg) {
            std::vector<uint8_t> response;

            switch(msg[0]) {
                case 0x00:
                    response.push_back(0x00);
                    break;
                default:
                    response.push_back(0x01);
                    break;
            }

            return response;
        }
};

int main() {
    MyDevice device;

    try {
        device.init();

        device.login("username", "password");

        /*
            A device only needs to be registered once.

            The registerDevice function will return a unique ID 
            that can be saved and reused. 

            For example, the ID can be saved to a file and retrieved.
        */
        std::string id = device.registerDevice("Device Name", "device_type");

        device.setId(id);

        device.run();
    } catch(hc::Exception& e) {
        std::cout << "error: " << e.what() << " (" << e.func() << ")" << std::endl;
    }

    device.cleanup();

    return 0;
}
```