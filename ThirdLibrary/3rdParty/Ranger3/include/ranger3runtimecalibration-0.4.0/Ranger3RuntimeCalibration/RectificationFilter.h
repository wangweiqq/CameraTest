// Copyright 2018 SICK AG. All rights reserved.
#pragma once
#include "CalibrationFilter.h"

namespace ranger3calibration {

	/*! \brief Class for rectification output data
	*
	*	Contains the output data from the rectification filter.
	*	Memory is managed by the object and is deleted when the object
	*	is deleted or runs out of scope.
	*/
	class RectificationData {
	public:
		/*! \brief Default constructor
		*
		*	If using the default constructor the resize function should be called after to allocated memory.
		*/
		RectificationData() { width = 0; height = 0; } //Default constructor
		/*! \brief Constructor
		*
		*	Allocates memory to fit data of size width and height by calling resize.
		*/
		RectificationData(int64_t width, int64_t height, bool generateReflectance) {
			resize(width, height, generateReflectance);
		}
		/*! \brief Copy-constructor
		*
		*	Performs a deep-copy of internal members range, weight and reflectance (if present).
		*/
		RectificationData(const RectificationData& other) {
			bool reflectanceEnabled = (other.reflectance != nullptr);
			resize(other.width, other.height, reflectanceEnabled);
			for (int i = 0; i < width*height; i++) {
				this->range[i] = other.range[i];
				this->weight[i] = other.weight[i];
				if (reflectanceEnabled) {
					this->reflectance[i] = other.reflectance[i];
				}
			}
		}

		/*! \brief assignment-operator
		*
		*	Performs a deep-copy of internal members range, weight and reflectance (if present).
		*/
		RectificationData& operator=(RectificationData other) {
			bool reflectanceEnabled = (other.reflectance != nullptr);
			resize(other.width, other.height,reflectanceEnabled);
			for (int i = 0; i < width*height; i++) {
				this->range[i] = other.range[i];
				this->weight[i] = other.weight[i];
				if (reflectanceEnabled) {
					this->reflectance[i] = other.reflectance[i];
				}
			}
			return *this;
		}
		/*! \brief Resize's the buffer capacity
		*
		*	Allocates memory to fit data of size width and height and allocates memory. Previously held memory is released.
		*/
		void resize(int64_t aWidth, int64_t aHeight, bool generateReflectance) {
			width = aWidth;
			height = aHeight;

			reflectance = generateReflectance ? std::make_unique<float[]>(width*height) : nullptr;
			weight = std::make_unique<float[]>(width*height);
			range = std::make_unique<float[]>(width*height);
		}

		int64_t width;
		int64_t height;

		std::unique_ptr<float[]> range;/*!< The data for the range. Each point in the array represents that points z-coordinate. This is usually height after calibration and rectification */
		std::unique_ptr<float[]> reflectance;/*!< Contains the rectified reflectance.*/
		std::unique_ptr<float[]> weight;/*!< The data for the weight. Used internally during mean rectification.*/


	};

	/*! \brief Enum for various rectification methods
	*
	*	Top		- Selects for each output pixel the highest range value
	*	Bottom	- Selects for each output pixel the lowest range value
	*	Mean	- Selects for each output pixel the weighted mean value of all range-values for that pixel.
	*/
	enum class RectificationMethod {
		Top,
		Bottom,
		Mean
	};
	/*! \brief Filter for applying rectification to calibration results.
	*
	*	A filter which given a CalibrationFilter performs Rectification
	*   If providing reflectance data as well this data will be rectified as well.
	*
	*	As output Range and possibly Reflectance.
	*/
	class RectificationFilter {
	public:
		/*! \brief Constructor
		*	Default constructor. Use the constructor which accpets the CalibrationFilter as input if possible.
		*/
		RectificationFilter() {}
		/*! \brief Constructor
		*	Accepts a CalibrationFilter and will extract the information the filter needs from the calibration filter.
		*	Optionally accepts a rectification width, if omitted the buffer width will be used.
		*/
		RectificationFilter(CalibrationFilter& f, const int _rectificationWidth = 0) {
			rectificationWidth = _rectificationWidth != 0 ? _rectificationWidth : f.sensorTraits.width;
			width = f.sensorTraits.width;
			height = f.sensorTraits.height;
			setBounds(f.worldTraits.lowerBoundX, f.worldTraits.upperBoundX);
			_mMissingDataValue = f.getMissingDataValue();
		}
		~RectificationFilter() { delete xTrans; }
		/*! \brief Apply Rectification
		*	Applys rectification to the data in calibData and places the result in rectData using the RectificationMethod specified by method.
		*
		*	The RectificationData should be initialized prior to calling this function, see the convenience function generateOutputContainer.
		*
		*	\param[in] calibData - Contains the data which will be rectified
		*	\param[in,out] rectData - Contains the rectified data after rectification. Must be initialized beforehand.
		*/
		int applyRectification(const CalibrationData& calibData, RectificationData& rectData, const RectificationMethod method = RectificationMethod::Mean);
		/*! \brief Apply Rectification
		*	Applys rectification to the reflectancedata and thedata in calibData and places the result in rectData using the RectificationMethod specified by method.
		*
		*	The RectificationData should be initialized prior to calling this function, see the convenience function generateOutputContainer.
		*	Note that the RectificationData container should contain a reflectance component.
		*
		*	\param[in] reflectance - Pointer to reflectance data.
		*	\param[in] calibData - Contains the data which will be rectified
		*	\param[in,out] rectData - Contains the rectified data after rectification. Must be initialized beforehand.
		*/
		int applyRectification(const uint8_t* reflectance, const CalibrationData& calibData, RectificationData& rectData, const RectificationMethod method = RectificationMethod::Mean);
		/*! \brief Generates an output container where the rectification result can be placed
		*
		*	Will by default allocate memory for reflectance as well. By passing generateReflectance = false to the function memory allocation for
		*	reflectance can be disabled. This is only valid if performing applyRectification without reflectance.
		*/
		RectificationData generateOutputContainer(bool generateReflectance = true);

		/*! \brief Get the missing data value for the calibration filter.
		*
		*/
		float getMissingDataValue();
		/*! \brief Set the missing data value for the calibration filter.
		*
		*/
		void setMissingDataValue(float newVal);

		/*! \brief Specified min and max values of the buffer in mm in x-direction.
		*
		*/
		int setBounds(const float xMin, const float xMax);

		void setRectificationWidth(const int val) { rectificationWidth = val; }
		void setWidth(const int val) { width = val; }
		void setHeight(const int val) { height = val; }

	private:
		class XTransformation; //Forward declaration to be able to keep XTransformation at the end of the class for readability.
		XTransformation * xTrans = nullptr;
		int rectificationWidth = 0;
		int width = 0;
		int height = 0;
		unsigned int xOutOfRange = 0;

		float _mMissingDataValue = 0.0f;

		/*! \brief Class for transforming data in the rectification filter.
		*
		*/
		class XTransformation
		{
		public:
			XTransformation(const float xMin, const float xMax, const unsigned long width) : xMin(xMin), xMax(xMax)
			{
				xPreOffset = -xMin;
				xPostScale = (float)width / (xMax - xMin);
			}

			inline float apply(const float &x) const
			{
				return (x + xPreOffset) * xPostScale;
			}

			inline bool inRange(const float &x) const
			{
				return x >= xMin && x <= xMax;
			}

			inline float inverse(const float &i) const {
				return i / xPostScale - xPreOffset;
			}

		private:
			float xMin;
			float xMax;

			float xPreOffset;
			float xPostScale;

		};

	};
}