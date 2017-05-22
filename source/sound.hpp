#include "libs/audiosys.h"
#include "libs/assetsys.h"
#include "ogg.hpp"


bool load_sound(assetsys_t* assetsys, char* name, audiosys_audio_source_t* loaded_audio)
{
	if (!name) return false;
	if (!assetsys) return false;

	assetsys_file_t file;
	assetsys_file(assetsys, name, &file);
	int size = assetsys_file_size(assetsys, file);
	if (size <= 0) return false;
	char* data = (char*)malloc((size_t)size);
	if (!data) return false;

	assetsys_file_load(assetsys, file, data);
	*loaded_audio = ogg_load(data, size);
	free(data);
	return true;
}

void free_sound(audiosys_audio_source_t* audio)
{
	ogg_free(*audio);
}