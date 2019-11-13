#ifndef __SONG_H__
#define __SONG_H__

#include <string>

#include "song_core.h"


class SONG_API Song
{
public:
    Song() {}
    Song(const std::string& name, double length);
    virtual ~Song() {}

    void SetName(const std::string& name);
    void SetLength(const double length);

    std::string GetName() const;
    double GetLength() const;

    void PrintMessage() const;

private:
    std::string m_name;
    double m_length;
};

#endif