#include <G10/GXJSON.h>

// TODO: Fully document function
int GXParseJSON( char* text, size_t len, size_t count, JSONValue_t* where )
{
	size_t currentWhere = 0;
	int ret = 0;
	int i = 1;
	
	// If we aren't passed a count, we figure out how many top level items there are in the JSON file.
	if (count == 0)
	{
		while (i < len)
		{
			if (text[i] == ':')                                                // Any time we hit a ':', we have come across a key/value pair; We increment ret.
				ret++;
			else if (text[i] == '{')                                           // Any time we hit a '{', we have come across an object, and need to skip over it. 
			{
				GXsize_t bDepth = 1;                                           // We can't count subobjects as it would break the recursive aspect of the parser, so  we need to keep track of how 
									                                           // deep we are into the subobjects. Of course if we hit a '{', we have encountered an object. 
				while (bDepth)
				{
					i++;
					if (text[i] == '{')                                        // Any subsequent '{'s will increment the depth. 
						bDepth++;
					else if (text[i] == '}')                                   // Any '}'s will deincrement the depth
						bDepth--;
					else if (text[i] == '\"')                                  // If '\"' is encountered, we will not count any '{'s inside of the string as an objects.
						while (text[++i] != '\"');                             // We can just ignore everything in the string and skip it.
				}
			}
			else if (text[i] == '\"')                                          // Any time we hit a '\"', we have come across a string, and we should just skip it.
				while (text[++i] != '\"');
			i++;
		}
		
		return ret;                                                            // Return the total number of key/value pairs we have found.
	}
	text;
	while (*++text)
	{
		// We've found a key
		if (*text == '\"')
		{
			where[currentWhere].name = text+1;                                 // Set a pointer to the start of the key
			while (*++text != '\"');

			*text = 0;                                                         // Set a null terminator at the end of the string, overwriting the '\"'
			text++;                                                            // Iterate past the null terminator
			while (*text++ != ':');                                            // Till the ':'
			while (*++text == ' ');                                            // and past any whitespaces

			if (*text == '\"')                                                 // Parse out a string
			{
				where[currentWhere].content.nWhere = ++text;                   // Skip past the '\"' 
				where[currentWhere].type  = GXJSONstring;                      // Set the type
				while (*++text != '\"');                                       // And skip to the next '\"'
				*text = 0;									                   // And set a null terminator
			}
			// Parse out an object
			else if (*text == '{')                                             
			{
				where[currentWhere].content.nWhere = text;
				where[currentWhere].type = GXJSONobject;
				text++;
				GXsize_t bDepth = 1;                                           // bDepth keeps track of the bracket depth
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
			// Parse out an array
			else if (*text == '[')
			{
				where[currentWhere].type = GXJSONarray;		                   // Set the type 
				GXsize_t c = 0;								                   // Create an index
				GXsize_t bDepth = 1;						                   // Similar to previous definition of bDepth, but for an array
				GXsize_t cDepth = 0;						                   // Similar to previous definition of cDepth, but for objects encountered in the array
				GXsize_t aEntries = 0;						                   // Elements in the array
				while (bDepth)
				{
					c++;
					if (text[c] == '[')						                   // Increment bDepth
						bDepth++;
					else if (text[c] == ']')				                   // Deincrement bDepth
						bDepth--;
					else if (text[c] == '{')				                   // Increment cDepth
						cDepth++;
					else if (text[c] == '}')				                   // Deincrement cDepth
						cDepth--;
					else if (text[c] == ',')				                   // We will either ignore it or increment aEntries depending on where the depths are
						((bDepth == 1) && (cDepth == 0)) ? aEntries++ : aEntries;
					else if (text[c] == '\"')				                   // If '\"' is encountered, skip over the string
						while (text[++c] != '\"');
				}
				if (aEntries)								                   // Add one to aEntries so that we have a null entry at the end
					aEntries++;

				// Allocate the space we need
				where[currentWhere].content.aWhere = malloc(sizeof(void*)*aEntries);
				text++;
				bDepth = 1, cDepth = 1, c = 0;
				while (bDepth)
				{
					if (*text == '\"')						                   // Parse out a string
					{
						where[currentWhere].content.aWhere[c] = ++text;
						while (*++text != '\"');
						*text = '\0';
						text++;
						c++;
						cDepth = 1;
					}
					else if (*text == '{')					                   // Parse out an object
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
					else if (*text == '[')					                   // Parse out an array
					{
						where[currentWhere].content.aWhere = text;
					}
					else if ((*text >= '0' && *text <= '9') || *text == '-')   // Parse out a primative
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

			}
			// Parse out a primative
			else if ((*text >= '0' && *text <= '9') || *text == '.' || *text == '-')
			{
				where[currentWhere].content.nWhere = text;					   // Set pointer to text
				where[currentWhere].type = GXJSONprimative;					   // Set the type
				while ((*++text >= '0' && *text <= '9')|| *text == '.');	   // Skip past any number or decimal place
				
			}
			currentWhere++;                                                    // Increment the token index
			if (currentWhere == (size_t)count)                                 // If we've gotten everything, clean up and leave
			{
				*++text = 0;								                   // Throw in a null terminator at the end of text for good measure
				return 0;
			}
			while (*text++ != ',');                                            // Else, just keep going
			*--text = 0;									                   // Set a null terminator
		}
	}

	return 0;
}
