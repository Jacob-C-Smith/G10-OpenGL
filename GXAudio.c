// TODO: Implement audio system, integrate with project, write application 
//       layer tools to inter
//  with audio, (maybe a queue that holds 
//       sound clips)

#include <G10/GXAudio.h>
//Use g_print_log for the printf calls.

GXSample_t *create_sample ( void )
{
	GXSample_t *ret = calloc(1, sizeof(GXSample_t));
	
	// Argument check
	{
		#ifndef NDEBUG
			if (ret == (void*)0)
				goto no_mem;
		#endif	
	}

	return ret;

	// Error handling
	{

		// Standard library errors
		{
			#ifndef NDEBUG
			no_mem:
				g_print_error("[G10] [Audio] Failed to allocate memory in call to function \"%s\"\n",__FUNCSIG__);
				return 0;
			#endif
		}
	}
}

GXSample_t *load_sample    ( const char path[] )
{
	GXSample_t* ret = 0;
	


	return ret;
}

GXMusic_t  *load_music     ( const char path[] )
{
	GXMusic_t *ret = 0;
	


	return ret;
}

int         play_sample    ( GXSample_t *sample )
{

}

int         play_music     ( GXMusic_t  *music  )
{

}

int         fade_music     ( GXMusic_t  *music, float start_volume, float end_volume, float time )
{

}

//GXSample_t* destroy_sample ( GXSample_t* sample )
//{
//
//	// Argument check
//	{
//		#ifndef NDEBUG
//			if(sample == (void*)0)
//				goto 
//		#endif 
//	}
//
//	free(name);
//	Mix_FreeChunk(sample->sample);
//
//	// Error check
//	{
//		
//	}
//}