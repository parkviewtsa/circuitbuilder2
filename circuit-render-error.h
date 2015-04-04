#pragma once
#define crError_NoProtoPath 1
#define crError_NoProtoFile 2
#define crError_BadProtoFile 3
#define crError_FailedAlloc 4
#define crError_SDL_InitFailure 5
#define crError_SDL_NoWindow 6
#define crError_SDL_NoRenderer 7
crIndex crLastError = 0;
crIndex crGetError ()
{
	crIndex err = crLastError;
	crLastError = 0;
	return err;
};
