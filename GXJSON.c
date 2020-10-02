#include <G10/GXJSON.h>

int GXParseJSON(char* text, size_t len, size_t count, JSONValue_t* where)
{
	size_t currentWhere = 0;
	int ret = 0;
	int i = 1;
	if (count == 0)
	{
		while (i < len)
		{
			if (text[i] == ':')
				ret++;
			else if (text[i] == '{')
				while (text[i] != '}')
					i++;
			else if (text[i] == '\"')
				while (text[++i] != '\"');
			i++;
		}
		
		return ret;
	}
	while (*++text)
	{
		if (*text == '\"')
		{
			where[currentWhere].name = text+1;
			while (*++text != '\"');

			*text = 0;
			text++;
			while (*text++ != ':');
			while (*++text == ' ');

			if (*text == '\"')
			{
				where[currentWhere].where = ++text;
				where[currentWhere].type  = GXJSONstring;
				while (*++text != '\"');
				*text = 0;
			}
			else if (*text == '{')
			{
				where[currentWhere].where = text;
				where[currentWhere].type = GXJSONobject;
				while (*++text != '}');
				
			}
			else if (*text == '[')
			{
				//TODO: parse out array
			}
			else if (*text > '0' && *text < '9')
			{
				where[currentWhere].where = text;
				where[currentWhere].type = GXJSONprimative;
				while (*++text >= '0' && *text <= '9');
				
			}
			currentWhere++;
			if (currentWhere == (size_t)count)
			{
				*++text = 0;
				return 0;
			}
			while (*text++ != ',');
			*--text = 0;

		}
	}
	return 0;
}
