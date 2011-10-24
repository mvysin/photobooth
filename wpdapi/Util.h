// Util.h

using namespace System;

namespace WPD
{
	public ref class Util
	{
	public:
		static void COMError(String ^msg, HRESULT hr)
		{
			throw gcnew System::Runtime::InteropServices::COMException(msg, hr);
		}

		static Guid ^CreateGuid(const GUID &guid)
		{
			return gcnew Guid(guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1],
				guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
		}

		static void CreateNativeGuid(Guid ^guid, GUID &out)
		{
			array<unsigned char>^ guidBytes = guid->ToByteArray();
			out.Data1 = guidBytes[0] | guidBytes[1] << 8 | guidBytes[2] << 16 | guidBytes[3] << 24;
			out.Data2 = guidBytes[4] | guidBytes[5] << 8;
			out.Data3 = guidBytes[6] | guidBytes[7] << 8;
			out.Data4[0] = guidBytes[8];
			out.Data4[1] = guidBytes[9];
			out.Data4[2] = guidBytes[10];
			out.Data4[3] = guidBytes[11];
			out.Data4[4] = guidBytes[12];
			out.Data4[5] = guidBytes[13];
			out.Data4[6] = guidBytes[14];
			out.Data4[7] = guidBytes[15];
		}
	};
}