struct memory {
	long totalMemory;
	long freeMemory;
	long sharedMemory;
	long bufferedMemory;
};

int getMemoryUse(struct memory *mem);