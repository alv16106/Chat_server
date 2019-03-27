static void handleError(char* message);
static void *ping(void *data);
static void *handleSession(void *data);
static char *sendMessage(char *message, int from, int to);
static char *sendDiffusion(char *message, int id);
