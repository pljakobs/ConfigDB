#include <basic-config.h>
#include <JSON/StreamingParser.h>
#include <Data/Format/Standard.h>

namespace
{
class ConfigListener : public JSON::Listener
{
public:
	using Element = JSON::Element;

	ConfigListener(ConfigDB::Database& db, Print& output) : db(db), output(output)
	{
	}

	const ConfigDB::ObjectInfo* rootSearch(const String& key)
	{
		// Find store
		auto& dbinfo = db.getTypeinfo();
		int i = dbinfo.stores.indexOf(key);
		if(i >= 0) {
			store = &dbinfo.stores[i];
			Serial << "Found STORE " << store->getName() << endl;
			return &store->object;
		}
		// Search in root store
		auto& root = dbinfo.stores[0].object;
		i = root.objinfo->indexOf(key);
		if(i < 0) {
			return nullptr;
		}
		store = &dbinfo.stores[0];
		return &root.objinfo->valueAt(i);
	}

	bool startElement(const Element& element) override
	{
		String key = element.getKey();

		Serial << "LEVEL " << element.level << ": " << key << endl;
		Serial << (element.container.isObject ? "OBJ" : "ARR") << '(' << element.container.index << ") " << key <<endl;

		if(element.type == Element::Type::Object || element.type == Element::Type::Array) {
			const ConfigDB::ObjectInfo* obj;
			if(element.level == 0) {
				obj = rootSearch(key);
			} else {
				auto parent = info[element.level - 1].object;
				if(!parent->objinfo) {
					Serial << "No objects for " << key << endl;
					return true;
				}
				int i = parent->objinfo->indexOf(key);
				if(i >= 0) {
					obj = &parent->objinfo->valueAt(i);
				} else if(element.level == 1) {
					obj = rootSearch(key);
				}
			}
			if(!obj) {
				Serial << "Missing object " << key << endl;
				return false;
			}
			Serial << "Found OBJECT " << obj->getName() << endl;
			info[element.level].object = obj;
		} else {
			auto obj = info[element.level - 1].object;
			if(!obj->propinfo) {
				Serial << "No properties for " << key << endl;
			} else {
				int i = obj->propinfo->indexOf(key);
				if(i < 0) {
					Serial << "Missing PROPERTY " << key;
					return false;
				}
				auto prop = obj->propinfo->data() + i;
				Serial << "Found " << toString(prop->getType()) << " PROPERTY " << prop->getName() << endl;
			}
		}

		output << (element.container.isObject ? "OBJ" : "ARR") << '(' << element.container.index << ") ";
		if(element.keyLength > 0) {
			output.write(element.key, element.keyLength);
			output.print(": ");
		}
		String value(element.value, element.valueLength);
		switch(element.type) {
		case Element::Type::Object:
			output.println('{');
			return true;
		case Element::Type::Array:
			output.println('[');
			return true;
		case Element::Type::String:
			// Format::standard.quote(value);
			[[fallthrough]];
		default:
			output << toString(element.type) << " = " << value << endl;
		}

		// Continue parsing
		return true;
	}

	bool endElement(const Element& element) override
	{
		indentLine(element.level);
		switch(element.type) {
		case Element::Type::Object:
			output.println('}');
			break;

		case Element::Type::Array:
			output.println(']');
			break;

		default:; //
		}

		// Continue parsing
		return true;
	}

private:
	void indentLine(unsigned level)
	{
		output << String().pad(level * 2);
	}

	ConfigDB::Database& db;
	Print& output;
	const ConfigDB::StoreInfo* store{};
	struct Info {
		const ConfigDB::ObjectInfo* object;
	};
	Info info[JSON::StreamingParser::maxNesting]{};
};

} // namespace

void parseJson(Stream& stream)
{
	BasicConfig db("test");
	ConfigListener listener(db, Serial);
	JSON::StaticStreamingParser<128> parser(&listener);
	auto status = parser.parse(stream);
	Serial << _F("Parser returned ") << toString(status) << endl;
	// return status == JSON::Status::EndOfDocument;
}
