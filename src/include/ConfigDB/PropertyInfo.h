/**
 * ConfigDB/PropertyInfo.h
 *
 * Copyright 2024 mikee47 <mike@sillyhouse.net>
 *
 * This file is part of the ConfigDB Library
 *
 * This library is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, version 3 or later.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this library.
 * If not, see <https://www.gnu.org/licenses/>.
 *
 ****/

#pragma once

#include <WString.h>
#include <Data/Range.h>
#include "Number.h"

/**
 * @brief Property types with storage size
 */
#define CONFIGDB_PROPERTY_TYPE_MAP(XX)                                                                                 \
	XX(Boolean, 1)                                                                                                     \
	XX(Int8, 1)                                                                                                        \
	XX(Int16, 2)                                                                                                       \
	XX(Int32, 4)                                                                                                       \
	XX(Int64, 8)                                                                                                       \
	XX(UInt8, 1)                                                                                                       \
	XX(UInt16, 2)                                                                                                      \
	XX(UInt32, 4)                                                                                                      \
	XX(UInt64, 8)                                                                                                      \
	XX(Number, 4)                                                                                                      \
	XX(String, sizeof(StringId))                                                                                       \
	XX(Object, sizeof(ObjectInfo*))

namespace ConfigDB
{
DEFINE_FSTR_LOCAL(fstr_empty, "")

enum class PropertyType : uint32_t {
#define XX(name, ...) name,
	CONFIGDB_PROPERTY_TYPE_MAP(XX)
#undef XX
};

/**
 * @brief Defines contained string data using index into string pool
 */
using StringId = uint16_t;

struct ObjectInfo;

/**
 * @brief Property metadata
 */
struct PropertyInfo {
	template <typename T, typename U = T> struct RangeTemplate {
		T minimum;
		T maximum;

		U clip(U value) const
		{
			return TRange(U(minimum), U(maximum)).clip(value);
		}
	};
	template <typename T> struct RangePtrTemplate {
		const T* minimum;
		const T* maximum;

		T clip(T value) const
		{
			if(minimum || maximum) {
				return TRange(minimum ? *minimum : 0, maximum ? *maximum : 0).clip(value);
			}
			return value;
		}
	};
	// Variant property information depends on type
	union Variant {
		const FlashString* defaultString;
		const ObjectInfo* object;
		RangeTemplate<int32_t> int32;
		RangeTemplate<uint32_t> uint32;
		RangeTemplate<const_number_t, number_t> number;
		RangePtrTemplate<int64_t> int64;
		RangePtrTemplate<uint64_t> uint64;
	};

	PropertyType type;
	const FlashString& name;
	uint32_t offset; ///< Location of property data in parent object
	Variant variant;

	static const PropertyInfo empty;

	explicit operator bool() const
	{
		return this != &empty;
	}

	/**
	 * @brief Get number of bytes required to store this property value within a structure
	 */
	uint8_t getSize() const
	{
		switch(type) {
#define XX(tag, size)                                                                                                  \
	case PropertyType::tag:                                                                                            \
		return size;
			CONFIGDB_PROPERTY_TYPE_MAP(XX)
#undef XX
		}
		return 0;
	}
};

} // namespace ConfigDB

String toString(ConfigDB::PropertyType type);
