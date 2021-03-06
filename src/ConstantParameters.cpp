#include "ConstantParameters.h"

namespace rgbd
{
	const float ICPConstParam::MIN_DEPTH = 0.1f;
	const float ICPConstParam::MAX_DEPTH = 1.0f;

	const float ICPConstParam::DST_THRESH = 0.01f;
	const float ICPConstParam::ANG_THRESH = 0.9f;

	const int ICPConstParam::MAX_LEVEL = 3;
	const int ICPConstParam::MAX_ITR_NUM[MAX_LEVEL] = { 1, 2, 6 }; // is this the correct way around?

}