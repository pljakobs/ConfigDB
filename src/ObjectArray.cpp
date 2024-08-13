/**
 * ConfigDB/ObjectArray.cpp
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

#include "include/ConfigDB/ObjectArray.h"
#include "include/ConfigDB/Store.h"

namespace ConfigDB
{
ObjectArray::ObjectArray(Store& store, const ObjectInfo& typeinfo)
	: Object(), id(store.getObjectData<ArrayId>(typeinfo))
{
}

unsigned ObjectArray::getObjectCount() const
{
	return getStore().objectArrayPool[id].getCount();
}

void* ObjectArray::getObjectDataPtr(unsigned index)
{
	auto& array = getStore().objectArrayPool[id];
	if(index < array.getCount()) {
		return array[index].get();
	}
	auto& typeinfo = getTypeinfo();
	assert(typeinfo.objectCount == 1);
	return array.add(*typeinfo.objinfo[0]);
}

bool ObjectArray::removeItem(unsigned index)
{
	return getStore().objectArrayPool[id].remove(index);
}

} // namespace ConfigDB
