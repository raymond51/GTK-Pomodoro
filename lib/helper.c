#include "helper.h"

/*Func to pass correct formatted file dir to glade file for builder*/
const char *prg_path(char *file_path, const char *file_loc)
{
    char absPath[ONE_KB];
    char concat_str[2 * ONE_KB];
    char old_char = '\\';
    char new_char = '/';

    getcwd(absPath, sizeof(absPath)); //equiv to pwd - to grab cur file path location
    for (int i = 0; i < (int)strlen(absPath); i++)
    {
        if (absPath[i] == old_char)
        {
            absPath[i] = new_char;
        }
    }

    snprintf(concat_str, sizeof(concat_str), "%s%s", absPath, file_loc);
    strncpy(file_path, concat_str, 2 * ONE_KB); //safe copy to prevent buffer overflow

#ifdef DEBUG_PRINT
    g_print("Combined Current working dir: %s\n", concat_str);
#endif
    return file_path;
}