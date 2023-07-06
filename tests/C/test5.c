typedef char* (*ptrFun)(char*);

char* getCaption(char* caption)
{
    return caption;
}

int main()
{
    ptrFun getCaptionPtr = (ptrFun)&getCaption;

    char* caption = getCaptionPtr("Test ptr caption");

    return 0;
}