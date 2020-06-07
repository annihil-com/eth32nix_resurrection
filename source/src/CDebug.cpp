// ETH32nix - an Enemy Territory cheat for Linux
// Copyright (c) 2008 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "CDebug.h"
#include "unixtypes.h"
#include <signal.h>

CDebug::CDebug()
{

}

CDebug::~CDebug()
{
}

void CDebug::Init()
{
	pathset = false;
	dbgFileFirstOpen = true;
	this->mutex = new pthread_mutex_t;
	pthread_mutex_init(mutex,NULL);
}

// set path and create empty file
void CDebug::SetLogFile(char *logFile)
{
	strcpy(path, logFile);
	FILE *fd = fopen(path, "w");
	fclose(fd);
}

void CDebug::Log(const char *logmsg, ...)
{
	// debug logging is very important, so *force* a lock
	if (pthread_mutex_lock(mutex)) FATAL_ERROR("could not lock mutex")

	static char strLogMsg[102400];

	va_list arglist;
	va_start(arglist, logmsg );
	vsprintf(strLogMsg, logmsg, arglist);
	va_end(arglist);

	if (!dbgFileFirstOpen) {
		dbgFile = fopen(path, "a");
	} else {
		dbgFile = fopen(path, "w");
		dbgFileFirstOpen = false;
	}

	if (dbgFile) {
		fprintf(dbgFile, "%s\n", strLogMsg);
		fclose(dbgFile);
	}

	if (pthread_mutex_unlock(mutex)) FATAL_ERROR("could not unlock mutex")
}

void CDebug::LogData(void *buf, size_t s_buf)
{
	if (pthread_mutex_lock(mutex)) FATAL_ERROR("could not lock mutex")

	if (!dbgFileFirstOpen) {
		dbgFile = fopen(path, "a");
	} else {
		dbgFile = fopen(path, "w");
		dbgFileFirstOpen = false;
	}

	if (dbgFile) {
		fwrite( buf, 1, s_buf, dbgFile );
		fclose(dbgFile);
	}

	if (pthread_mutex_unlock(mutex)) FATAL_ERROR("could not unlock mutex")
}
