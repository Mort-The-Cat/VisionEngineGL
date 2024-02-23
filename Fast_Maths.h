#pragma once

#ifndef FAST_MATHS_H
#define FAST_MATHS_H

#include <cstdint>
#include <math.h>

constexpr const float DTR = 3.14159f / 180.0f;

namespace Fast
{

#define Delta_Constant 0x1FBD3F7Du
#define Epsilon_Constant 0x7EF4FDF4u
#define Zeta_Constant 1064992510u

#define DTR_Constant 48934896u

	float Add_Epsilon(float Value, int Epsilon)
	{
		uint32_t Long = *(uint32_t*)&Value;

		Long += Epsilon;

		return *(float*)&Long;
	}

	float Inverse_Sqrt(float Value)
	{
		uint32_t Long = *(uint32_t*)&Value;

		Long >>= 1;

		Long += Delta_Constant;

		Long = Epsilon_Constant - Long;

		return *(float*)&Long;
	}

	float Sqrt(float Value)
	{
		uint32_t Long = *(uint32_t*)&Value;

		Long >>= 1;

		Long += Delta_Constant;

		return *(float*)&Long;
	}

	float Division(float Value, float Denominator)
	{
		uint32_t Long = *(uint32_t*)&Value;

		uint32_t I = *(uint32_t*)&Denominator;

		Long -= I;

		Long += Zeta_Constant;

		return *(float*)&Long;
	}

	float Inverse(float Value)
	{
		uint32_t Long = *(uint32_t*)&Value;
		Long = Epsilon_Constant - Long;
		return *(float*)&Long;
	}

	float Old_Pow(float Value, float Exponent)
	{
		uint32_t Long = (*(uint32_t*)&Value) - Zeta_Constant;

		Long *= Exponent;

		Long += Zeta_Constant;

		return *(float*)&Long;
	}

	float Log_Two(float Value)
	{
		int64_t Long = *(uint32_t*)&Value;
		float Exponent = (Long >> 23) - 128;
		uint32_t Mantissa = (Long & 8388607) | 1065353216;
		Exponent += *(float*)&Mantissa;
		return Exponent;
	}

	float Pow_2(float Exponent)
	{
		uint32_t Long = 127;
		Long += Exponent;
		Long <<= 23;

		Exponent -= floorf(Exponent) - 1;
		uint32_t Mantissa = *(uint32_t*)&Exponent;

		Long |= Mantissa & 8388607u;

		return *(float*)&Long;
	}

	float Pow(float Value, float Exponent)
	{
		Value = Log_Two(Value);

		return Pow_2(Exponent * Value);
	}

	float Sigmoid(float Value)
	{
		uint32_t Long = 11762006;

		Long *= -Value; // float * int (not ideal)

		Long += Zeta_Constant; // integre

		float Denomenator = 1.0f + *(float*)&Long; //float

		Long = *(uint32_t*)&Denomenator;

		Long = Epsilon_Constant - Long; // integre

		return *(float*)&Long;
	}

	float Sigmoid_Derivative(float Value)
	{
		float Sigmoid_X = Sigmoid(Value);

		return Sigmoid_X * (1 - Sigmoid_X);
	}

	bool Is_Decimal(float Value) // This returns whether or not the input is a whole number
	{
		uint32_t Long = *(uint32_t*)&Value;

		int8_t Exponent = (Long >> 23) + 1; // This should get the transformed exponent value with simple ALU instructions

		//Exponent -= 127;

		uint32_t Mantissa_Mask = 8388607u; //((1 << 23) - 1);

		return !(Long & (Mantissa_Mask >> Exponent)) && Exponent < 0;
	}

	float Floor(float Value)
	{
		uint32_t Long = *(uint32_t*)&Value;

		int8_t Exponent = (Long >> 23) - 127;

		uint32_t Float_Mask = 8388607u >> Exponent; // Creates a mask for the float, unsetting the bits which contribute to the part of the number

		Long &= ~(Float_Mask); // Applies the mask

		uint8_t Negative_Exp = (Exponent < 0) << 1;

		Long >>= Negative_Exp; // If the exponent is negative (and thereby the value is between -1 and 1) then the float is basically set to zero

		return *(float*)&Long;
	}

	float To_Radians(float Value)
	{
		uint32_t Long = *(uint32_t*)&Value;

		Long -= DTR_Constant; // Equivalent to / (3.14159 / 180)

		return *(float*)&Long;
	}

#undef Delta_Constant
#undef Zeta_Constant
#undef Epsilon_Constant
#undef DTR_Constant

}

#endif
