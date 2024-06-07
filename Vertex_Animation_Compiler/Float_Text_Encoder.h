#ifndef FLOAT_TEXT_ENCODER
#define FLOAT_TEXT_ENCODER

#include <string>

namespace Encoder
{
	unsigned char Integer_To_Char(unsigned char Integer)
	{
		return Integer + 33;
	}

	unsigned char Char_To_Integer(unsigned char Char)
	{
		return (Char - 33);
	}

	//

	float Characters_To_Float(const char* Representation)
	{
		uint32_t Bits = 0;

		Bits |= (Char_To_Integer(Representation[0]) & 63u);
		Bits |= (Char_To_Integer(Representation[1]) & 63u) << 6;
		Bits |= (Char_To_Integer(Representation[2]) & 63u) << 12;
		Bits |= (Char_To_Integer(Representation[3]) & 63u) << 18;

		Bits |= (Char_To_Integer(Representation[4]) & 15u) << 24;
		Bits |= (Char_To_Integer(Representation[5]) & 15u) << 28;

		return *(float*)&Bits;	// We then convert the bits to a floating point value
	}

	std::string Float_To_Characters(float Value)
	{
		uint32_t Bits = *(uint32_t*)&Value;

		std::string Float_Representation = "123456"; // We need 6 digits per floating point number

		// 64, 64, 64, 64, 16, 16

		// We'll use 4 base-64 numbers and 2 hexadecimal numbers

		Float_Representation[0] = Integer_To_Char(Bits & 63u); // This gets the bits for all 64 combinations
		Float_Representation[1] = Integer_To_Char((Bits >> 6) & 63u);
		Float_Representation[2] = Integer_To_Char((Bits >> 12) & 63u);
		Float_Representation[3] = Integer_To_Char((Bits >> 18) & 63u);

		Float_Representation[4] = Integer_To_Char((Bits >> 24) & 15u); // This gets 16 combinations for hex values
		Float_Representation[5] = Integer_To_Char((Bits >> 28) & 15u);

		return Float_Representation;
	}
}

#endif