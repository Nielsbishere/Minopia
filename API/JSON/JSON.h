/*
 * JSON.h
 *
 *  Created on: May 22, 2016
 *      Author: Niels Brunekreef
 *      API owner: Osomi Lunar
 *      License: Only licensed for: Osomi.net/license/JSON
 */

#ifndef JSON_H_
#define JSON_H_

using namespace std;

#include <algorithm>
#include <sstream>
#include <fstream>

bool startsWith(string whole, string part){
	if(whole.size()<part.size())return false;
	return whole.substr(0, part.size()) == part;
}

class JSON {
public:
	enum Priority {
		DISABLED = 1,
		LOWEST = 3,
		LOW = 7,
		MEDIUM = 15,
		HIGH = 31,
		HIGHEST = 63,
		ALL = 127
	};
	const static Priority debug;
	struct JSONObject {
	private:
		string key, type;
		void* value;
	public:
		JSONObject(string key, string type, void* value) {
			this->key = key;
			this->type = type;
			this->value = value;
		}
		JSONObject() :
				JSONObject("", "", nullptr) {

		}
		string getKey() {
			return key;
		}
		string getType() {
			return type;
		}
		void *getValue() {
			return value;
		}
	};
	struct JSONList {
	private:
		vector<JSONObject> objects;
	public:
		JSONList(vector<JSONObject> objects) {
			this->objects = objects;
		}
		void *getObject(int i) {
			return objects[i].getValue();
		}
		int getObjects() {
			return objects.size();
		}
		string getType(int i) {
			return objects[i].getType();
		}
		vector<string> strings() {
			for (unsigned int i = 0; i < objects.size(); i++)
				if (getType(i) != "String")
					return vector<string>();
			vector<string> res;
			for (unsigned int i = 0; i < objects.size(); i++)
				res.push_back(*static_cast<string*>(objects[i].getValue()));
			return res;
		}
	};
	JSON() :
			JSON(0, nullptr) {

	}
private:
	struct Query {
	private:
		string type;
		void *value;
	public:
		Query(string t, void *v) {
			type = t;
			value = v;
		}
		string getType() {
			return type;
		}
		void *getValue() {
			return value;
		}
		JSONObject toJSONObject(string key) {
			return JSONObject(key, type, value);
		}
	};
	JSONObject *obj;
	int length, index;
	bool editable;
	JSON(int length, JSONObject *obj) {
		this->length = length;
		this->obj = obj;
		this->index = 0;
		this->editable = false;
	}
public:
	~JSON(){
		if(obj!=nullptr)delete[] obj;
	}
private:
	static vector<string> split(string splitee, char splitter,
			bool ignoreWhenEnclosed = false) {
		vector<string> strings;
		unsigned int i = 0, j = 0;
		string s;
		bool enclosingString = false, enclosingChar = false;
		int enclosingBrackets = 0, enclosingBraces = 0;
		while (i < splitee.size()) {
			char splitted = splitee[i];
			if (splitted == '[' && !enclosingString && !enclosingChar
					&& enclosingBraces == 0)
				enclosingBrackets++;
			else if (splitted == ']' && !enclosingString && !enclosingChar
					&& enclosingBraces == 0)
				enclosingBrackets--;
			else if (splitted == '{' && !enclosingString && !enclosingChar
					&& enclosingBrackets == 0)
				enclosingBraces++;
			else if (splitted == '}' && !enclosingString && !enclosingChar
					&& enclosingBrackets == 0)
				enclosingBraces--;
			else if (splitted == '"' && !enclosingChar && enclosingBrackets == 0
					&& enclosingBraces == 0)
				enclosingString = !enclosingString;
			else if (splitted == '\'' && !enclosingString
					&& enclosingBrackets == 0 && enclosingBraces == 0)
				enclosingChar = !enclosingChar;
			if (splitted == splitter
					&& ((!enclosingString && !enclosingChar
							&& enclosingBrackets == 0 && enclosingBraces == 0)
							|| !ignoreWhenEnclosed)) {
				stringstream ss;
				ss << s;
				strings.push_back(ss.str());
				j++;
				s = "";
			} else {
				s += splitted;
			}
			i++;
			if (i == splitee.size()) {
				stringstream ss;
				ss << s;
				strings.push_back(ss.str());
			}
		}
		return strings;
	}
	static JSONList *parseList(string s) {
		if (s.size() < 3)
			return nullptr;
		vector<string> strings = JSON::split(s, ',', true);
		vector<JSONObject> objects;
		for (unsigned int i = 0; i < strings.size(); i++)
			objects.push_back(parse(trim(strings[i])).toJSONObject("" + i));
		return new JSONList(objects);
	}
	static string trim(string s) {
		while (s.size() > 0 && (s[0] == ' ' || s[0] == '\t'))
			s = s.substr(1);
		while (s.size() > 0 && (s[s.size() - 1] == ' ' || s[s.size() - 1] == '\t'))
			s = s.substr(0, s.size() - 1);
		return s;
	}
	static Query parse(string s) {
		if (s.size() < 1) {
			cout << "JSON logic error: the value is less than one character!"
					<< endl;
			return Query("JSON", nullptr);
		}
		string type = getType(s);
		void *val = nullptr;
		if (JSON::debug >= MEDIUM)
			cout << "Parsing a JSON key with type: " << type << "(From: '" << s
					<< "')" << endl;
		if (type == "String")
			val = new string(s.substr(1, s.size() - 2));
		else if (type == "Char")
			val = new char(s[1]);
		else if (type == "List")
			val = parseList(s.substr(1, s.size() - 2));
		else if (type == "JSON")
			val = parseJSON(s);
		//TODO: Add support for the double notation with e and E and stuff (+-)
		else if (type == "Bool")
			val = new bool(s == "true");
		else if (type == "Double")
			val = new double(atof(s.c_str()));
		else if (type == "Int")
			val = new int(atoi(s.c_str()));
		return Query(type, val);
	}
	static string getType(string s) {
		if (s.size() < 1)
			return "null";
		if (s.size() >= 2) {
			if (s[0] == '"' && s[s.size() - 1] == '"')
				return "String";
			else if (s[0] == '\'' && s[s.size() - 1] == '\'') {
				if (s.size() != 3)
					return "null";
				return "Char";
			} else if (s[0] == '[' && s[s.size() - 1] == ']')
				return "List";
			else if (s[0] == '{' && s[s.size() - 1] == '}')
				return "JSON";
		}
		if (s == "false")
			return "Bool";
		else if (s == "true")
			return "Bool";
		else if (s == "null")
			return "null";
		else if (s.find('.') != string::npos)
			return "Double";
		else
			return "Int";
	}

	Query prop(string s) {
		vector<string> strings = split(s, '.');
		if (strings.size() == 0)
			return Query("JSON", nullptr);
		string leftover;
		for (unsigned int i = 1; i < strings.size(); i++)
			leftover += (i != 1 ? "." : "") + strings[i];
		for (int i = 0; i < length; i++) {
			if (obj[i].getKey() == strings[0]) {
				if (obj[i].getType() != "JSON")
					return Query(obj[i].getType(), obj[i].getValue());
				//TODO: else if("List")
				else {
					if (strings.size() >= 2)
						return static_cast<JSON*>(obj[i].getValue())->prop(
								leftover);
					else
						return Query(obj[i].getType(), obj[i].getValue());
				}
			}
		}
		return Query("JSON", nullptr);
	}
	void set(JSONObject o) {
		if (debug == ALL)
			cout << "Started setting " << o.getKey() << " of type "
					<< o.getType() << endl;
		bool found = false;
		for (int i = 0; i < index; i++) {
			if (obj[i].getKey() == o.getKey()) {
				if (debug == HIGH)
					cout << "Found and replaced an object at " << o.getKey()
							<< endl;
				obj[i] = o;
				found = true;
				break;
			}
		}
		if (!found) {
			if (debug >= HIGHEST)
				cout << "Inserting " << o.getKey() << " at " << index << endl;
			if(index+1>=length){
				JSONObject *New = new JSONObject[length++];
				for(int i=0;i<length-1;i++)New[i] = obj[i];
				delete[] obj;
				obj = New;
				if (debug == MEDIUM)
					cout << "Inserting extra element " << o.getKey() << " at " << index
						<< " with type " << o.getType() << endl;
			}
			obj[index++] = o;
			if (debug == HIGH)
				cout << "Inserted key " << o.getKey() << " at " << (index - 1)
						<< " with type " << o.getType() << endl;
		}
	}
	bool exists(string s){
		vector<string> strings = split(s, '.');
		if (strings.size() == 0)return false;
		JSON *json = this;
		while(strings.size()>0){
			if(json==nullptr||json->getKeys()==0)return false;
			if(json->getIndex(strings[0])!=-1){
				JSONObject jo = json->obj[json->getIndex(strings[0])];
				string type = jo.getType();
				if(type == "JSON")json = static_cast<JSON*>(jo.getValue());
				else return strings.size()==1;
			}else return false;
			strings.erase(strings.begin());
		}
		return true;
	}
public:
	vector<string> getKeylist() {
		vector<string> str;
		for (int i = 0; i < length; i++)
			str.push_back(obj[i].getKey());
		return str;
	}
	string getKey(int i){
		if(i>=index)return "";
		return obj[i].getKey();
	}
	static JSON *parseJSON(string s) {
		//TODO: Parsing: \\ \. \' and \"
		//\/ \b \f \n \r \t \u
		//TODO: Add e support
		if (debug == HIGHEST)
			cout << "Reading JSON: " << s << endl;
		if (s.size() < 2) {
			cout
					<< "JSON reading error (#0): The JSON is shorter than 2 characters!"
					<< endl;
			return nullptr;
		}

		while (s.find("\n") != string::npos) {
			s.erase(s.find("\n"), 1);
		}
		s = s.substr(1, s.size() - 2);
		while (startsWith(s, "\t") || startsWith(s, " "))s = s.substr(1);
		if (debug == ALL)
			cout << "Splitting JSON strings" << endl;
		vector<string> strings = JSON::split(s, ',', true);
		if (debug == ALL)
			cout << "Splitted JSON strings: " << strings.size()
					<< ", started creating JSON.." << endl;
		for(unsigned int i=0;i<strings.size();i++)strings[i] = trim(strings[i]);
		JSON *json = new JSON(strings.size(), new JSONObject[strings.size()]);
		if (debug == ALL)
			cout << "Created JSON" << endl;
		for (unsigned int i = 0; i < strings.size(); i++) {
			if (debug == ALL)
				cout << "Started reading element " << i << endl;
			vector<string> kv = JSON::split(strings[i], ':', true);
			if (debug == ALL)
				cout << "The element exists out of " << kv.size() << " parts!"
						<< endl;
			for (unsigned int j = 0; j < kv.size(); j++)
				if (debug >= MEDIUM)
					cout << j << " (" << kv[j] << ")" << endl;
			if (kv.size() > 0 && debug >= LOW)
				cout << kv[0] << endl;
			if (kv.size() != 2 || kv[0].size() < 3) {
				cout
						<< "JSON reading error (#1): A JSON key is shorter than 3 characters, or there are no keys or values!"
						<< endl;
				return nullptr;
			}
			string key = trim(kv[0]);
			if (key[0] != '"' || key[key.size() - 1] != '"') {
				cout
						<< "JSON reading error (#2): The JSON key doesn't start and end with a double quote! "
						<< endl;
				return nullptr;
			}
			key = key.substr(1, key.size() - 2);
			string value = trim(kv[1]);
			if (debug == ALL)
				cout << value << endl;
			if (debug == ALL)
				cout << "Parsing JSON key '" << key << "' (" << value << ")"
						<< endl;
			json->set(parse(value).toJSONObject(key));
			if (debug == ALL)
				cout << "Parsed JSON key '" << key << "'" << endl;
		}
		if (debug == HIGHEST)
			cout << "Parsed JSON!" << endl;
		return json;
	}
	static JSON createEditable(){
		JSON json = JSON(0, nullptr);
		json.editable = true;
		return json;
	}
	string _str(string s) {
		if(obj==nullptr || !exists(s))return "";
		Query qr = prop(s);
		if (qr.getType() != "String" || qr.getValue() == nullptr)
			return "";
		return *static_cast<string*>(qr.getValue());
	}
	int _int(string s) {
		if(obj==nullptr || !exists(s))return 0;
		Query qr = prop(s);
		if (qr.getType() != "Int" || qr.getValue() == nullptr)
			return 0;
		return *static_cast<int*>(qr.getValue());
	}
	int _nint(string s) {
		int nint = _int(s);
		return nint < 1 ? 1 : nint;
	}
	double _double(string s) {
		if(obj==nullptr || !exists(s))return 0;
		Query qr = prop(s);
		if ((qr.getType() != "Double" && qr.getType() != "Int" ) || qr.getValue() == nullptr)
			return 0;
		if(qr.getType() == "Int")return (double)(*static_cast<int*>(qr.getValue()));
		return *static_cast<double*>(qr.getValue());
	}
	bool _bool(string s) {
		if(obj==nullptr || !exists(s))return false;
		Query qr = prop(s);
		if (qr.getType() != "Bool" || qr.getValue() == nullptr)
			return false;
		return *static_cast<bool*>(qr.getValue());
	}
	char _char(string s) {
		if(obj==nullptr || !exists(s))return (char)0;
		Query qr = prop(s);
		if (qr.getType() != "Char" || qr.getValue() == nullptr)
			return (char) 0;
		return *static_cast<char*>(qr.getValue());
	}
	JSON _json(string s) {
		if(obj==nullptr || !exists(s))return JSON(0, nullptr);
		Query qr = prop(s);
		if (qr.getType() != "JSON" || qr.getValue() == nullptr)
			return JSON(0, nullptr);
		return *static_cast<JSON*>(qr.getValue());
	}
	JSONList _list(string s) {
		if(obj==nullptr || !exists(s))return JSONList(vector<JSONObject>());
		Query qr = prop(s);
		if (qr.getType() != "List" || qr.getValue() == nullptr)
			return JSONList(vector<JSONObject>());
		return *static_cast<JSONList*>(qr.getValue());
	}
	vector<string> _strvec(string s) {
		if(obj==nullptr || !exists(s))return vector<string>();
		JSONList jlist = _list(s);
		return jlist.strings();
	}

	int getKeys() {
		return length;
	}
	int getIndex(string s){
		for(int i=0;i<getKeys()-1;i++)if(getKey(i)==s)return i;
		return -1;
	}

	void _set(JSONObject o){
		if(!editable)return;
		set(o);
	}

	bool hasKey(string s){
		return getIndex(s)!=-1;
	}

	static JSON read(string file){
		ifstream i(file.c_str());
		if(i.is_open()){
			string f = "";
			string line;
			while(getline(i,line)){
				f = f.append((f.size()!=0?"\n":"")).append(line);
			}
			i.close();
			if(JSON::debug>=Priority::HIGH)cout << "Finished parsing file, started parsing JSON" << endl;
			return *parseJSON(f);
		}else cout << "Failed to read the 'json file': " << file << endl;
		return JSON();
	}
};

#endif /* JSON_H_ */
