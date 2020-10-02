#include <G10/GXJSON.h>

int GXParseJSON(char* text, size_t len, size_t count, JSONValue_t* where)
{
	size_t currentWhere = 0;
	int ret = 0;
	int i = 1;
	
	// If we aren't passed a count, we figure out how many top level items there are in the JSON file.
	if (count == 0)
	{
		while (i < len)
		{
			if (text[i] == ':')        // Any time we hit a ':', we have come across a key/value pair; We increment ret.
				ret++;
			else if (text[i] == '{')   // Any time we hit a '{', we have come across an object, and need to skip over it. 
				while (text[i] != '}')
					i++;
			else if (text[i] == '\"')  // Any time we hit a '\"', we have come across a string, and we should just skip it.
				while (text[++i] != '\"');
			i++;
		}
		
		return ret;                    // Return the total number of key/value pairs we have found.
	}

	while (*++text)
	{
		if (*text == '\"')                                  // We've found a key
		{
			where[currentWhere].name = text+1;              // Set a pointer to the start of the key
			while (*++text != '\"');

			*text = 0;                                      // Set a null terminator at the end of the string, overwriting the '\"'
			text++;                                         // Iterate past the null terminator
			while (*text++ != ':');                         // Till the ':'
			while (*++text == ' ');                         // and past any whitespaces

			if (*text == '\"')                              // Parse out a string
			{
				where[currentWhere].where = ++text;
				where[currentWhere].type  = GXJSONstring;
				while (*++text != '\"');
				*text = 0;
			}
			else if (*text == '{')                          // Parse out an object
			{
				where[currentWhere].where = text;
				where[currentWhere].type = GXJSONobject;
				while (*++text != '}');
				
			}
			else if (*text == '[')                          // Parse out an array
			{
				//TODO: parse out array
			}
			else if (*text > '0' && *text < '9')            // Parse out a primative
			{
				where[currentWhere].where = text;
				where[currentWhere].type = GXJSONprimative; 
				while (*++text >= '0' && *text <= '9');
				
			}
			currentWhere++;                                 // Increment the token index
			if (currentWhere == (size_t)count)              // If we've gotten everything, clean up and leave
			{
				*++text = 0;
				return 0;
			}
			while (*text++ != ',');                         // Else, just keep going
			*--text = 0;
		}
	}

	return 0;
}
