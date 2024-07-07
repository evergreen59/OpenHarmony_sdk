# Location<a name="EN-US_TOPIC_0000001080646118"></a>

-   [Introduction](#section11660541593)
-   [Directory Structure](#section161941989596)
-   [Constraints](#section119744591305)
-   [Usage](#section1312121216216)
    -   [Available APIs](#section1551164914237)
    -   [Usage Guidelines](#section129654513264)

-   [Repositories Involved](#section1371113476307)

## Introduction<a name="section11660541593"></a>

People take their mobile devices wherever they go. Mobile devices have become a necessity in people's daily routines, whether it be for looking at the weather forecast, browsing news, hailing a taxi, navigating, or recording data from a workout. All these activities are so much associated with the location services on mobile devices.

With the location awareness capability offered by OpenHarmony, mobile devices will be able to obtain real-time, accurate location data. Building location awareness into your application can also lead to a better contextual experience for application users.

Your application can call location-specific APIs to obtain the location information of a mobile device for offering location-based services such as drive navigation and motion track recording.

**Basic Concepts**

Location awareness helps determine where a mobile device locates. The system identifies the location of a mobile device with its coordinates, and uses location technologies such as Global Navigation Satellite System \(GNSS\) and network positioning \(for example, base station positioning or WLAN/Bluetooth positioning\) to provide diverse location-based services. These advanced location technologies make it possible to obtain the accurate location of the mobile device, regardless of whether it is indoors or outdoors.

-   **Coordinate**

    A coordinate describes a location on the earth using the longitude and latitude in reference to the World Geodetic Coordinate System 1984.

-   **GNSS positioning**

    GNSS positioning locates a mobile device by using the location algorithm offered by the device chip to compute the location information provided by the Global Navigation Satellite System, for example, GPS, GLONASS, BeiDou, and Galileo. Whichever positioning system will be used during the location process depends on a hardware capability of the device. 

-   **Base station positioning**

    Base station positioning estimates the current location of a mobile device based on the location of the resident base station in reference to the neighboring base stations. This technology provides only a low accuracy and requires access to the cellular network.

-   **WLAN or Bluetooth positioning**

    WLAN or Bluetooth positioning estimates the current location of a mobile device based on the locations of WLANs and Bluetooth devices that can be discovered by the device. The location accuracy of this technology depends on the distribution of fixed WLAN access points \(APs\) and Bluetooth devices around the device. A high density of WLAN APs and Bluetooth devices can produce a more accurate location result than base station positioning. This technology also requires access to the network. 


**Figure  1**  Location subsystem architecture<a name="fig4460722185514"></a>  

![](figures/En-1.png)


## Directory Structure<a name="section161941989596"></a>

```
/base/location      # Source code directory structure：
  ├── figures       # Store the architecture diagram in readme
  ├── frameworks    # Location framework code
  ├── interfaces    # External interface
  ├── sa_profile    # SA's profile
  ├── services      # Directory of SA codes for location services
  ├── test          # Test Code Directory
```

## Constraints<a name="section119744591305"></a>

 - Your application can use the location function only after the user has granted the permission and turned on the function. If the location function is off, the system will not provide the location service for any application.

 - Since the location information is considered sensitive, your application still needs to obtain the location access permission from the user even if the user has turned on the location function. The system will provide the location service for your application only after it has been granted the permission to access the device location information.

## Usage<a name="section1312121216216"></a>

### Available APIs<a name="section1551164914237"></a>

https://gitee.com/openharmony/interface_sdk-js/blob/master/api/@ohos.geolocation.d.ts


### Usage Guidelines<a name="section129654513264"></a>

https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis/js-apis-geolocation.md


## Repositories Involved<a name="section1371113476307"></a>

base_location

