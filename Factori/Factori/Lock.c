#include "Lock.h"


Lock* InitializeLock() {
	Lock* lock = (Lock*)malloc(sizeof(Lock));

	if (lock != NULL) {
		lock->readers_count = 0;
		lock->read_mutex = CreateMutex(NULL, FALSE, NULL);
		lock->write_mutex = CreateMutex(NULL, FALSE, NULL);

		if (lock->read_mutex == NULL || lock->write_mutex == NULL)
			free(lock);
	}

	return lock;
}


DWORD read_lock(Lock* lock) {
	DWORD return_code;

	//Enter critical part 
	return_code = WaitForSingleObject(lock->read_mutex, MAX_WAIT_IN_MILISECONDS);
	if (return_code != WAIT_OBJECT_0)
		return return_code;
	
	//Lock for write
	if (lock->readers_count == 0) {
		return_code = WaitForSingleObject(lock->write_mutex, MAX_WAIT_IN_MILISECONDS);
		if (return_code != WAIT_OBJECT_0)
			return return_code;
	}
	//Update count
	lock->readers_count++;

	//Exit critical part
	ReleaseMutex(lock->read_mutex);

	return return_code;
}


void read_release(Lock* lock) {
	DWORD return_code;

	//Enter critical part 
	return_code = WaitForSingleObject(lock->read_mutex, MAX_WAIT_IN_MILISECONDS);
	if (return_code != WAIT_OBJECT_0)
		printf("Error - Failed on WaitForSingleObject on read_release");

	//Update count
	lock->readers_count--;

	//Unlock write if there's no more readers
	if (lock->readers_count == 0) {
		ReleaseMutex(lock->write_mutex);
	}

	//Exit critical part
	ReleaseMutex(lock->read_mutex);
}


DWORD write_lock(Lock* lock) {
	return WaitForSingleObject(lock->write_mutex, MAX_WAIT_IN_MILISECONDS);
}


void write_release(Lock* lock) {
	ReleaseMutex(lock->write_mutex);
}


void DestroyLock(Lock* lock) {
	CloseHandle(lock->read_mutex);
	CloseHandle(lock->write_mutex);
	free(lock);
	lock = NULL;
}

