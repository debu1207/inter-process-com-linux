#define PathName "queue.h"
#define MsgLen	4
#define MsgCount	6
#define ProjectId 	123

typedef struct {
	long type;
	char payload[MsgLen + 1];
} queuedMessage;

