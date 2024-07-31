typedef enum
{
    START = 0,
    OK = 1,
    ERRCOL = 2,
    INSERT = 10,
    SEARCH = 11,
    END = 30,
} MsgType;

const char* tr(MsgType msg) {
    switch (msg) {
        case START:   return "START";
        case OK:      return "OK";
        case ERRCOL:  return "NOT OK";
        case INSERT:  return "INSERT";
        case SEARCH:  return "SEARCH";
        case END:     return "END";
        default:      return "UNKNOWN";
    }
}