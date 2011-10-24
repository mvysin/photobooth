// Enum.h

#pragma once

using namespace System;

namespace WPD
{
	public enum class FunctionCategory
	{
		Any,
		AudioCapture,
		Device,
		NetworkConfiguration,
		RenderingInformation,
		StillImageCapture,
		Storage,
		SMS,
		VideoCapture
	};
}