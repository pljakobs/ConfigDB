/**
 * ConfigDB/Json/Store.cpp
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

#include <ConfigDB/Json/Store.h>
#include <Data/CStringArray.h>
#include <Data/Stream/MemoryDataStream.h>

namespace ConfigDB::Json
{
JsonObject Store::getJsonObject(const String& path)
{
	String s(path);
	s.replace('.', '\0');
	CStringArray csa(std::move(s));
	auto obj = doc.isNull() ? doc.to<JsonObject>() : doc.as<JsonObject>();
	for(auto key : csa) {
		if(!obj) {
			break;
		}
		auto child = obj[key];
		if(!child.is<JsonObject>()) {
			child = obj.createNestedObject(const_cast<char*>(key));
		}
		obj = child;
	}
	return obj;
}

JsonObjectConst Store::getJsonObjectConst(const String& path) const
{
	String s(path);
	s.replace('.', '\0');
	CStringArray csa(std::move(s));
	auto obj = doc.as<JsonObjectConst>();
	if(obj.isNull()) {
		return obj;
	}
	for(auto key : csa) {
		if(!obj) {
			break;
		}
		auto child = obj[key];
		if(!child.is<JsonObjectConst>()) {
			return JsonObjectConst{};
		}
		obj = child;
	}
	return obj;
}

} // namespace ConfigDB::Json
