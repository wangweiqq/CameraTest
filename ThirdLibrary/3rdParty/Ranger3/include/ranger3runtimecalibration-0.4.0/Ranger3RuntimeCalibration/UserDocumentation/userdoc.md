# Runtime calibration of Ranger3 data
## How To Use
The code included in the runtime calibration package, together with this document, is intended for calibration of Ranger3 cameras in C++. 
The instructions will help you set it up for use in the Ranger3Samples provided in the Ranger3SDK. It assumes a little bit of familiarity with 
the samples and that you already have a calibration. The calibration shall be of .xml not .lut format. 

The code to get access to the calibration and rectification filters are basically two class-files (.cpp/.h) and the xml-reader dependencies. 

## Configuring your project
Before starting it's a good idea to verify that the Ranger3Samples build and run.
If they do not build or do not run, refer to the ReadMe file for Ranger3's samples and return here when they build. 

In this guide we'll look at inserting the new calibration and rectification code into the existing SampleContinuousAcquisition sample.

You need to include the following files to your project. 
In this case we will add them directly to SampleContinuousAcquisition, next to SampleContinuousAcquisition.cpp
The files which needs to be added to the project are: 
* CalibrationFilter.(cpp|h)
* RectificationFilter.(cpp|h)
* tinyxml2.(cpp|h)

Add includes to CalibrationFilter and RectificationFilter at the top of SampleContinuousAcquisition.cpp and that is all you should need to configure the project.
Verify that your project builds and runs. 
```cpp
#include "CalibrationFilter.h"
#include "RectificationFilter.h"
```

## In Code
In the sections below we'll look at the necessary code modification needed to the use calibration in the SampleContinuousAcquisition sample. 
### Modifications needed to the code
The loading of calibration parameters and initialization of the filters should be placed outside of the grab-loop, so that they are not re-initialized for each new frame. In this case we have placed it just before the announcing of the queue buffers to the producer, this exact location is not that important, the key point is to keep this code outside of the grab-loop. 
However the application of the calibration should be placed inside the grab-loop as we want to apply it on each new frame. 
The calibration code always operates on 16-bit range, therefore we want to make sure we run our calibration and rectification filters after the conversion from (possibly) 12-bit range has happened. 
In the sample this happens in the 'save to disk' if statement.

```cpp
   if (saveToDisk)
        {
          // Convert Range data to 16-bit if needed
          if (part0Info.mPartDataFormat == PFNC_Coord3D_C12p
              || part0Info.mPartDataFormat == PFNC_Mono12p)
          {
            GenIRanger::convert12pTo16(part0Info.mPartDataPointer,
                                       part0Info.mPartDataSize,
                                       pBuffer16,
                                       &buffer16Size);
          } else
          {
            // Just copy the data to buffer16 pointer
            assert(part0Info.mPartDataSize
                   <= static_cast<uint64_t>(buffer16Size));
            std::memcpy(pBuffer16, part0Info.mPartDataPointer,
                        part0Info.mPartDataSize);
          }
          [...]
        }
```

Since we want to calibrate even if we do not want to save to disk, we'll move the conversion code to above the
if(saveToDisk) statement. 

```cpp
    // Convert Range data to 16-bit if needed
    if (part0Info.mPartDataFormat == PFNC_Coord3D_C12p
        || part0Info.mPartDataFormat == PFNC_Mono12p)
    {
    GenIRanger::convert12pTo16(part0Info.mPartDataPointer,
                                part0Info.mPartDataSize,
                                pBuffer16,
                                &buffer16Size);
    } else
    {
    // Just copy the data to buffer16 pointer
    assert(part0Info.mPartDataSize
            <= static_cast<uint64_t>(buffer16Size));
    std::memcpy(pBuffer16, part0Info.mPartDataPointer,
                part0Info.mPartDataSize);
    }

    //We'll add some of our calibration and rectification code here

    if (saveToDisk)
    {
        
        [...]
    }
```

### Loading a calibration file
[This is placed outside of the grab-loop]  
We need to load a calibration file (.xml) from file. This can be done using the following code snippet. The return value of the function should be checked to verify that the file was loaded properly. 
```cpp
//Setup the calibration parameters
ranger3calibration::CalibrationParameters params;
int res = params.loadFromFile("..\\calibrationFile.xml");
if (res != 0) {
    std::cerr << "Error loading calibration file" << std::endl;
    return -1;
}
```
### Setting up the sensor traits
[This is placed outside of the grab-loop]  
We need to specify the sensor traits before performing calibration. Depending on the setting of RangeAxis, Standard or Reversed, the originZ will change. Here a fixed value is coded into the sample, in practice one should read this from the configuration through GenICam. 

```cpp
//Setup traits
bool rangeAxisStandard = true;
float scaleZ = rangeAxisStandard ? 0.0625f : -0.0625f;
float originZ = rangeAxisStandard ? (float)aoiOffsetY + 2.0f * scaleZ : aoiOffsetY + aoiHeight - 2.0f * scaleZ;
ranger3calibration::SensorTraits traits{ (int)bufferWidth, (int)bufferHeight, (int)aoiHeight, aoiOffsetX, originZ, scaleZ};
```
### Initializing the filters
[This is placed outside of the grab-loop]  
```cpp 
//Initialize the calibrationFilter
float missingDataValue = NAN;
ranger3calibration::CalibrationFilter calFilter{ params, traits, missingDataValue };

//Initialize rectification filter
const unsigned int rectificationWidth = 2560;
ranger3calibration::RectificationFilter rectFilter{ calFilter, rectificationWidth };

```
### Initializing the data storage
[This is placed outside of the grab-loop]  
When we apply the calibration and rectification filter we need a data-container to put the data into. 
We use the convenience functions to generate these containers. For each new buffer we can reuse these data-containers. 
```cpp
try{
//Get container for data 
ranger3calibration::CalibrationData calibrationData = calFilter.generateOutputContainer();

//Get container for data 
ranger3calibration::RectificationData rectificationData = rectFilter.generateOutputContainer(/*possible to disable reflectance component here */);
}catch(std::runtime_error e){
    std::cerr << e.what() << std::endl;
    //Error in generating output containers.
}
```
For the rectification version it's possible to send in false to avoid generating the reflectance component. This should be used when the camera is configured to not send any reflectance and we do not need any storage for that component. 

### Applying the Calibration and Rectification 
[This is placed inside of the grab-loop]  
```cpp
try {

//Apply Calibration
calFilter.applyCalibration(pBuffer16, calibrationData, false /*SSE Optimization available on some platforms*/); 

//Apply Rectification
rectFilter.applyRectification(part1Info.mPartDataPointer, calibrationData, rectificationData, ranger3calibration::RectificationMethod::Mean);
}
catch (std::runtime_error e) {
    std::cerr << e.what() << std::endl;
    //Error in rectification filter or calibration filter
}
```

### Reading the result
```cpp
//Reading the 10th pixel on the 150th row
float rangeData = rectificationData.range.get()[10 + 150 * bufferWidth];
float reflectanceData = rectificationData.reflectance.get()[10 + 150 * bufferWidth];
```
## Project Dependencies
Tested against Ranger3-2.0.0.1660
Uses TinyXML2

## Where to get a Calibration file
One can create the calibration files used in this sample using SICK provided software such as EasyRanger Calibration. 
With an account on the SupportPortal one can find the software here: 
https://supportportal.sick.com/downloads/easy-ranger-calibration/

## Licenses
### TinyXML2
License
-------

TinyXML-2 is released under the zlib license:

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must
not claim that you wrote the original software. If you use this
software in a product, an acknowledgment in the product documentation
would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source
distribution.



