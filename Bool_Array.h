#ifndef BOOLEAN_ARRAY
#define BOOLEAN_ARRAY

template <typename Type>
	class Bool_Array
	{
		class Boolean_Array_Bool
		{
		public:
			Type* Data;
			unsigned int Index;
			Boolean_Array_Bool(Type* Datap, unsigned int Indexp)
			{
				Data = Datap;
				Index = Indexp;
			}

			void operator= (bool Value)
			{
				unsigned int Or_Mask = ((unsigned int)Value) << Index;
				unsigned int And_Mask = ~(1u << Index);
				*Data &= And_Mask;
				*Data |= Or_Mask;
			}

			operator bool() const { return (*Data >> Index) & 1u; } // Implicit type conversion
		};

	public:
		Type Data = 0u;
		Boolean_Array_Bool operator[] (Type Index)
		{
			return Boolean_Array_Bool(&Data, Index);
		}
	};

#endif