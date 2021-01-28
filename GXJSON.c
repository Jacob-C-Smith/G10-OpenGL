#include <G10/GXJSON.h>

// TODO: Fully document function
int GXParseJSON ( char* text, size_t len, size_t count, JSONValue_t* where )
{
	size_t currentWhere = 0;
	int    ret          = 0,
		   i            = 1;

	// If we aren't passed a count, we figure out how many top level items there are in the JSON file.
	if (count == 0)
	{
		while (i < len)
		{
			if (text[i] == ':')        // Any time we hit a ':', we have come across a key/value pair; We increment ret.
				ret++;
			else if (text[i] == '{')   // Any time we hit a '{', we have come across an object, and need to skip over it. 
			{
				GXsize_t bDepth = 1;
				while (bDepth)
				{
					i++;
					if (text[i] == '{')
						bDepth++;
					else if (text[i] == '}')
						bDepth--;
					else if (text[i] == '\"')
						while (text[++i] != '\"');
				}
			}
			else if (text[i] == '\"')  // Any time we hit a '\"', we have come across a string, and we should just skip it.
				while (text[++i] != '\"');
			i++;
		}

		return ret;                    // Return the total number of key/value pairs we have found.
	}
	text;
	while (*++text)
	{
		if (*text == '\"')                                  // We've found a key
		{
			where[currentWhere].name = text + 1;              // Set a pointer to the start of the key
			while (*++text != '\"');

			*text = 0;                                      // Set a null terminator at the end of the string, overwriting the '\"'
			text++;                                         // Iterate past the null terminator
			while (*text++ != ':');                         // Till the ':'
			while (*++text == ' ');                         // and past any whitespaces

			if (*text == '\"')                              // Parse out a string
			{
				where[currentWhere].content.nWhere = ++text;
				where[currentWhere].type = GXJSONstring;
				while (*++text != '\"');
				*text = 0;
			}
			else if (*text == '{')                          // Parse out an object
			{
				where[currentWhere].content.nWhere = text;
				where[currentWhere].type = GXJSONobject;
				text++;
				GXsize_t bDepth = 1;                        // bDepth keeps track of the bracket depth
				while (bDepth)
				{
					text++;
					if (*text == '{')
						bDepth++;
					else if (*text == '}')
						bDepth--;
					else if (*text == '\"')
						while (*++text != '\"');
				}
			}
			else if (*text == '[')                          // Parse out an array
			{
				where[currentWhere].type = GXJSONarray;
				GXsize_t c = 0;
				GXsize_t bDepth = 1;
				GXsize_t cDepth = 0;
				GXsize_t aEntries = 1;
				while (bDepth)
				{
					c++;
					if (text[c] == '[')
						bDepth++;
					else if (text[c] == ']')
						bDepth--;
					else if (text[c] == '{')
						cDepth++;
					else if (text[c] == '}')
						cDepth--;
					else if (text[c] == ',')
						((bDepth == 1) && (cDepth == 0)) ? aEntries++ : aEntries;
					else if (text[c] == '\"')
						while (text[++c] != '\"');
				}
				if (aEntries > 1)
					aEntries++;
				where[currentWhere].content.aWhere = malloc(sizeof(void*) * (aEntries+1));
				text++;
				bDepth = 1, cDepth = 1, c = 0;
				while (bDepth)
				{
					if (*text == '\"')
					{
						where[currentWhere].content.aWhere[c] = ++text;
						while (*++text != '\"');
						*text = '\0';
						text++;
						c++;
						cDepth = 1;
					}
					else if (*text == '{')
					{
						where[currentWhere].content.aWhere[c] = text;
						while (cDepth)
						{
							text++;
							if (*text == '{')
								cDepth++;
							else if (*text == '}')
								cDepth--;
							else if (*text == '\"')
								while (*++text != '\"');
						}
						*++text = '\0';
						c++;
						cDepth = 1;
					}
					else if (*text == '[')
					{
						where[currentWhere].content.aWhere = text;
					}
					else if ((*text >= '0' && *text <= '9') || *text == '-')
					{
						where[currentWhere].content.aWhere[c] = text;
						while ((*++text >= '0' && *text <= '9') || *text == '.' || *text == '-');
						c++;
						*text = '\0';
					}
					if (*text == ']')
					{
						bDepth--;
					}
					text++;
				}
				where[currentWhere].content.aWhere[c] = (void*)0;
			}
			else if ((*text >= '0' && *text <= '9') || *text == '.' || *text == '-')            // Parse out a primative
			{
				where[currentWhere].content.nWhere = text;
				where[currentWhere].type = GXJSONprimative;
				while ((*++text >= '0' && *text <= '9') || *text == '.');

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