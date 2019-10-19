#ifndef VERSION_H
#define VERSION_H

class Version{
    public:
    static const char *versionString;
    static const char *versionMajor;
    static const char *versionMinor;
    static const char *versionPatch;
    static const char *versionBuild;
    static const char *lastCommitSha1;

    private:
    Version()=delete;
    ~Version()=delete;
};

#endif // VERSION_H
