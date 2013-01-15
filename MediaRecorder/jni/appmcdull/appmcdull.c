// appmcdull.c

#include <stdio.h>
#include <stdlib.h>

int sprintf_safe(char* _Dest, const char* _Format, char filename[])
{
#ifdef _MSC_VER
    return sprintf_s(_Dest, 80, _Format, filename);
#else
    return sprintf(_Dest, _Format, filename);
#endif
}

int fopen_safe(FILE** _FILE, const char* _Filename, const char* _Mode)
{
#ifdef _MSC_VER
    return fopen_s(_FILE, _Filename, _Mode);
#else
    *_FILE = fopen(_Filename, _Mode);
    if (*_FILE == NULL) return -1;

    return 0;
#endif
}

int main_264(int argc, char* argv[])
;
//int main_mp4(int argc, char* argv[])
//;

int main_millet(int argc, char* argv[]);


int main(int argc, char* argv[])
{
	return main_millet(argc, argv);

//	return main_264(argc, argv);
	//return main_mp4(argc, argv);
}
