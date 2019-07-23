// Copyright 2018 SICK AG. All rights reserved.

#pragma warning(disable:4244)
#pragma once
#include <string>
#include <memory>


namespace ranger3calibration {

	/*! \brief Class for calibration output data
	*
	*	Contains the output data from the calibration filter. 
	*	Memory is managed by the object and is deleted when the object 
	*	is deleted or runs out of scope.
	*/
	class CalibrationData {
	public:

		/*! \brief Default constructor
		*
		*	If using the default constructor the resize function should be called after to allocated memory.
		*/
		CalibrationData() { width = 0; height = 0; } //Default constructor
		/*! \brief Constructor
		*
		*	Allocates memory to fit data of size width and height by calling resize.
		*/
		CalibrationData(int64_t width, int64_t height) {
			resize(width, height);
		}

		/*! \brief Copy-constructor
		*
		*	Performs a deep-copy of internal members rangeX and rangeR. 
		*/
		CalibrationData(const CalibrationData& other) {
			resize(other.width, other.height);
			for (int i = 0; i < width*height; i++) {
				this->rangeX[i] = other.rangeX[i];
				this->rangeR[i] = other.rangeR[i];
			}
		}

		/*! \brief assignment-operator
		*
		*	Performs a deep-copy of internal members rangeX and rangeR.
		*/
		CalibrationData& operator=(CalibrationData other) {
			resize(other.width, other.height);
			for (int i = 0; i < width*height; i++) {
				this->rangeX[i] = other.rangeX[i];
				this->rangeR[i] = other.rangeR[i];
			}
			return *this;
		}

		/*! \brief Resize's the buffer capacity
		*
		*	Allocates memory to fit data of size width and height and allocates memory. Previously held memory is released.
		*/
		void resize(int64_t aWidth, int64_t aHeight) {
			width = aWidth;
			height = aHeight;

			rangeX = std::make_unique<float[]>(width*height);
			rangeR = std::make_unique<float[]>(width*height);
		}

		int64_t width;
		int64_t height;

		std::unique_ptr<float[]> rangeX;/*!< The data for the rangeX. Each point in the array represents that points x-coordinate. */
		std::unique_ptr<float[]> rangeR;/*!< The data for the rangeR. Each point in the image represents that points range-value. This is usually the z-coordinate or height in mm. */
	};

	/*! \brief Struct for Calibration Parameters
	*
	*	Can be created and then initialized by using the loadFromFile function. 
	*/
	struct CalibrationParameters
	{
		//Parameters shared between version1 and version2 calibration
		double H[9];
		double opticalCenter[2];
		double lensDistortion[4];


		//Only for version2 calibration
		double scheimpflugRoll;
		double scheimpflugTilt;

		double coverage;

		int version = -1;
		/*! \brief Loads a calibration from the file specified by the filepath input argument
		*/
		int loadFromFile(std::string filepath);
	};

	/*! \brief Class for Sensor Traits
	*
	*	Specifies and passes around information concerning the sensor and the AOI used during laser-peak extraction
	*/
	class SensorTraits {
	public:
		int width; //Width of input buffer
		int height; // Height of input buffer
		float scaleX, scaleZ;
		float xOffset, zOffset;
		int aoiHeight;
		/*! \brief Default constructor with initialization of values
		*
		*	Constructs the SensorTraits object and sets all the needed values.
		*/
		SensorTraits(int aWidth, int aHeight, int aAoiHeight, float aXOffset, float aZOffset, float aScaleZ = 0.0625f, float aScaleX = 1.0f) :
			width(aWidth), height(aHeight), aoiHeight(aAoiHeight), xOffset(aXOffset), zOffset(aZOffset), scaleZ(aScaleZ), scaleX(aScaleX) {}
		/*! \brief Default constructor
		*
		*	Prefer to use the constructor which initializes all the values. 
		*/
		SensorTraits(){}
	};
	/*! \brief Struct for WorldRangeTraits
	*
	*	The values of the struct construct a 2-dimensional boundingbox within which all valid values after calibration should lie. 
	*	In addition can the pixel x-resolution be described as (upperBoundX - lowerBoundX)/bufferWidth after rectification
	*/
	struct WorldTraits {
		float lowerBoundX;
		float upperBoundX;
		float lowerBoundR;
		float upperBoundR;
	};

	/*! \brief Filter for applying a calibration file to data
	*
	*	A filter which given the CalibrationParameters and the SensorTraits and input data, converts 
	*   the range value expressed in pixels to range values expressed in mm's. 
	*
	*	As output RangeR and RangeX will be emitted. 
	*/
	class CalibrationFilter {
	public:
		CalibrationFilter() = delete;
		/*! \brief Constructor for the filter
		*
		*	Construct the filter by providing the CalibrationParameters p, and the SensorTraits s.
		*   WorldRangeTraits will be calculated upon initialization. 
		*	\param[in] missingDataValue - Change missing data (0) to this value. (e.g. 0 or NaN).
		*/
		CalibrationFilter(CalibrationParameters p, SensorTraits s, float _missingDataValue = 0.0f) : _mMissingDataValue(_missingDataValue) {
			setCalibrationParameters(p);
			setSensorTraits(s);
			calculateWorldTraits();
		}
		/*! \brief Sets the CalibrationParameters
		*
		*	Note. One need to run CalculateWorldRangeTraits manually after setting new CalibrationParameters
		*	\param[in] p - The calibration parameters for this filter.
		*/
		void setCalibrationParameters(CalibrationParameters p) { _mCalibrationPrms = p; }
		
		/*! \brief Sets the SensorTraits
		*
		*	Note. One need to run CalculateWorldRangeTraits manually after setting new SensorTraits
		*/
		void setSensorTraits(SensorTraits s) {
			sensorTraits = s;
		};

		/*! \brief Calculates the WorldRangeTraits
		*/
		void calculateWorldTraits();

		/*! \brief Performs calibration on the inData
		*	
		*	Apply the calibration on inData and output the result in data.
		*	The CalibrationData should be initialized prior to executing this function, 
		*   the convenience function generateOutputContainer is provided for this purpose. 
		*
		*	It's possible to run the calibration with SSE-optimization. This has better 
		*	better performance on certain hardware. SSE-optimization will be disabled internally 
		*	if the buffer width is not a multiple of 8. 
		*
		*	\param[in] inData - Pointer to the range data on 16 bit format. (not 12 bit). 
		*	\param[in,out] calibData - Contains the resulting calibrated data after calibration. Must be initialized beforehand.
		*
		*/
		void applyCalibration(const uint8_t* inData, CalibrationData& outputData, bool SSEEnabled = false);

		/*! \brief Get the missing data value for the calibration filter.
		*
		*/
		float getMissingDataValue();
		/*! \brief Set the missing data value for the calibration filter.
		*
		*/
		void setMissingDataValue(float newVal);

		/*! \brief Returns a output containter for calibration data
		*
		*	Convenience function for creating the output container needed for applyCalibration.
		*/
		CalibrationData generateOutputContainer();

		SensorTraits sensorTraits;
		WorldTraits worldTraits;

#ifndef UNIT_TEST //De-privatize internal functions during testing. 
	private:
#endif // !UNIT_TEST

		void applyCalibrationVersion1(const uint8_t* inData, CalibrationData& outputData, bool SSEEnabled = false);
		void applyCalibrationVersion2(const uint8_t* inData, CalibrationData& outputData, bool SSEEnabled = false);
		void calculateWorldTraitsV1();
		void calculateWorldTraitsV2();

		CalibrationParameters _mCalibrationPrms;
		float _mMissingDataValue = 0;
	
		/*! \brief Calibrates a single point
		*
		*	Calibrates a single point.
		*/
		inline void calibratePointFast(const int& col, const float& scaleX, const float& xOffset, const float& scaleZ, const float& zOffset, const unsigned short& data, float* xOut, float* rOut) {
			
			float x = (float)col*scaleX + xOffset;
			float y = (float)data*scaleZ + zOffset;

			double* pOC = _mCalibrationPrms.opticalCenter;
			double* pLensDist = _mCalibrationPrms.lensDistortion;
			float x3, y3, x2, y2, r2, xUD, yUD;

			x3 = x - pOC[0];
			y3 = y - pOC[1];
			x2 = x3 * x3;
			y2 = y3 * y3;
			r2 = x2 + y2;

			xUD = (float)((x + x3 * (pLensDist[0] * r2 + pLensDist[1] * r2*r2) + 2 * pLensDist[2] * x3*y3 + pLensDist[3] * (r2 + 2 * x2)));

			yUD = (float)((y + y3 * (pLensDist[0] * r2 + pLensDist[1] * r2*r2) + 2 * pLensDist[3] * x3*y3 + pLensDist[2] * (r2 + 2 * y2)));

			float alpha = _mCalibrationPrms.H[6] * xUD + _mCalibrationPrms.H[7] * yUD + _mCalibrationPrms.H[8];
			float xRes = (_mCalibrationPrms.H[0] * xUD + _mCalibrationPrms.H[1] * yUD + _mCalibrationPrms.H[2]) / alpha;
			float rRes = (_mCalibrationPrms.H[3] * xUD + _mCalibrationPrms.H[4] * yUD + _mCalibrationPrms.H[5]) / alpha;
			*xOut = xRes;
			*rOut = rRes;
		}

		/*! \brief Calibrates a single point
		*
		*	Calibrates a single point. Note, convenience function, slightly slower than calibratePointFast
		*/
		void calibratePoint(const unsigned short& col, const unsigned short & range, float & xOut, float & rangeOut);
	};
}